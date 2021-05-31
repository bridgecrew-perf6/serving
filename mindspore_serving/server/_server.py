# Copyright 2021 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ============================================================================
"""Interface for start up servable"""
import logging
import os
import time
import threading
import signal

import mindspore_serving.log as logger
from mindspore_serving.server.worker import init_mindspore
from mindspore_serving.server.master import start_master_server, stop_on_except, stop, at_stop_list
from mindspore_serving.server._servable_common import WorkerContext
from mindspore_serving.server._servable_local import ServableStartConfig, ServableContextData, merge_config
from mindspore_serving.server.distributed._servable_distributed import DistributedStartConfig, DistributedContextData
from mindspore_serving._mindspore_serving import ExitSignalHandle_


@stop_on_except
def start_servables(servable_configs):
    r"""
    Start up servables.

    It can be used to start multiple different servables. One servable can be deployed on multiple chips,
    and each chip runs a servable copy.

    On Ascend 910 hardware platform, each copy of each servable owns one chip. Different servables or different versions
    of the same servable need to be deployed on different chips.
    On Ascend 310 and GPU hardware platform, a chip can be shared, and different servables or different versions of
    the same servable can be deployed on the same chip to realize chip reuse.

    Args:
        servable_configs (Union[ServableStartConfig, list[ServableStartConfig], tuple[ServableStartConfig]]): The
            startup configs of one or more servables.

    Raises:
        RuntimeError: Failed to start one or more servables.

    Examples:
        >>> import os
        >>> from mindspore_serving import server
        >>>
        >>> servable_dir = os.path.abspath(".")
        >>> resnet_config = server.ServableStartConfig(servable_dir, "resnet", device_ids=(0,1))
        >>> add_config = server.ServableStartConfig(servable_dir, "add", device_ids=(2,3))
        >>> server.start_servables(servable_configs=(resnet_config, add_config))
    """
    if isinstance(servable_configs, (ServableStartConfig, DistributedStartConfig)):
        servable_configs = (servable_configs,)
    if not isinstance(servable_configs, (tuple, list)):
        raise RuntimeError(f"Parameter '{servable_configs}' should be ServableStartConfig, list or tuple of "
                           f"ServableStartConfig, but actually {type(servable_configs)}")
    for config in servable_configs:
        if not isinstance(config, (ServableStartConfig, DistributedStartConfig)):
            raise RuntimeError(
                f"The item of parameter '{servable_configs}' should be ServableStartConfig, but actually "
                f"{type(config)}")

    init_mindspore.set_mindspore_cxx_env()
    # merge ServableStartConfig with same servable name and running version number
    try:
        servable_configs = merge_config(servable_configs)
    except RuntimeError as e:
        logging.exception(e)
        raise RuntimeError(f"Start servables failed: {str(e)}")
    logger.info("Servable configs:")
    for config in servable_configs:
        if isinstance(config, ServableStartConfig):
            logger.info(f"servable directory: {config.servable_directory}, servable name: {config.servable_name}, "
                        f"running version number: {config.version_number}, device ids:{config.device_ids}, "
                        f"device type: {config.device_type}")
        if isinstance(config, DistributedStartConfig):
            logger.info(f"distributed servable, servable directory: {config.servable_directory}, "
                        f"servable name: {config.servable_name}, rank table json file: {config.rank_table_json_file}, "
                        f"running version number: {config.version_number}, "
                        f"distributed address:{config.distributed_address}, "
                        f"wait agents time: {config.wait_agents_time_in_seconds}s")

    master_pid = os.getpid()
    unix_socket_dir = "unix_socket_files"
    try:
        os.mkdir(unix_socket_dir)
    except FileExistsError:
        pass
    master_address = f"unix:{unix_socket_dir}/serving_master_{master_pid}"
    start_master_server(address=master_address)

    signal.signal(signal.SIGCHLD, signal.SIG_IGN)
    worker_list = []
    for config in servable_configs:
        if isinstance(config, ServableStartConfig):
            for device_id in config.device_ids:
                try:
                    context_data = ServableContextData(config, device_id, master_address)
                    worker_pid = context_data.new_worker_process()
                    worker_context = WorkerContext(context_data, master_address, worker_pid)
                except RuntimeError as e:
                    _send_exit_signal_to_children(worker_list)
                    raise RuntimeError(f"Start worker failed: {e}")
                worker_list.append(worker_context)
        elif isinstance(config, DistributedStartConfig):
            try:
                context_data = DistributedContextData(config, master_address)
                worker_pid = context_data.new_worker_process()
                worker_context = WorkerContext(context_data, master_address, worker_pid)
            except RuntimeError as e:
                _send_exit_signal_to_children(worker_list)
                raise RuntimeError(f"Start worker failed: {e}")
            worker_list.append(worker_context)

    _listening_workers_when_startup(worker_list)
    _listening_workers_after_startup(worker_list)


def _send_exit_signal_to_children(worker_list):
    """Send exit signal to all child processes, and terminate all child processes when they are still alive
    in some seconds later"""
    if not worker_list:
        return
    for worker in worker_list:
        worker.send_exit_signal(signal.SIGINT)
    wait_seconds = 10
    for i in range(wait_seconds * 100):  # 10s
        all_exit = True
        for worker in worker_list:
            if worker.is_alive():
                if i % 100 == 0:
                    logger.warning(f"Wait for all worker processes to exit, otherwise they will be forcibly killed in "
                                   f"{wait_seconds - (i // 100)} seconds.")
                all_exit = False
                break
        if all_exit:
            logger.info(f"All Child process exited")
            return
        time.sleep(0.01)

    for worker in worker_list:
        worker.send_exit_signal(signal.SIGKILL)


def _listening_workers_when_startup(worker_list):
    """Listening child process"""
    time_last = time.time()
    while True:
        time.sleep(0.1)
        if ExitSignalHandle_.has_stopped():
            logger.warning("Fail to start workers because of signal SIGINT or SIGTERM")
            _send_exit_signal_to_children(worker_list)
            raise RuntimeError("Fail to start workers because of signal SIGINT or SIGTERM")

        all_ready = True
        for worker in worker_list:
            if not worker.is_alive() or worker.has_error_notified():
                for _ in range(100):
                    if worker.has_error_notified():
                        logger.warning(f"Fail to start workers: {worker.get_notified_error()}")
                        _send_exit_signal_to_children(worker_list)
                        raise RuntimeError(f"Fail to start workers: {worker.get_notified_error()}")
                    time.sleep(0.01)  # wait 1s for error msg
                logger.error(f"Fail to start workers because of death of one worker")
                _send_exit_signal_to_children(worker_list)
                raise RuntimeError("Fail to start workers because of death of one worker")
            if not worker.ready():
                if time.time() - time_last > 1:
                    time_last = time.time()
                    worker.print_status()
                all_ready = False
        if all_ready:
            break
    logger.info("All workers is ready")


def _listening_workers_after_startup(worker_list):
    """Listening agent status after success start up of agents"""

    def listening_thread_fun():
        while True:
            time.sleep(0.01)
            if ExitSignalHandle_.has_stopped():
                logger.warning("Serving server begin to exit: receive exit signal")
                break
            alive_count = 0
            for worker in worker_list:
                if worker.is_in_process_switching:
                    alive_count += 1
                    continue
                if worker.is_alive():
                    alive_count += 1
                    if worker.is_unavailable():
                        worker.restart_worker()
                    continue
                # not alive
                if not worker.is_fault_exit():
                    continue
                if worker.can_be_restart():
                    logger.warning(f"detect worker process has exited, try to restart, servable: {worker.to_string()}")
                    worker.restart_worker()

            if not alive_count:
                logger.warning("Serving server begin to exit: all worker processes have exited")
                break

        _send_exit_signal_to_children(worker_list)
        stop()

    thread = threading.Thread(target=listening_thread_fun)
    thread.start()

    def join_thread():
        if thread != threading.current_thread():
            thread.join()
            return True
        return False

    at_stop_list.append(join_thread)

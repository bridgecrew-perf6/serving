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
"""Start worker process with single core servable"""

import os
import signal
import argparse

from mindspore_serving.server import worker
from mindspore_serving.server.common import check_type
from mindspore_serving._mindspore_serving import ExitSignalHandle_
from mindspore_serving._mindspore_serving import Worker_


def start_worker(servable_directory, servable_name, version_number,
                 device_type, device_id, master_address, dec_key, dec_mode, listening_master, enable_lite):
    """Start worker process with single core servable"""
    signal.signal(signal.SIGCHLD, signal.SIG_DFL)  # for ccec compiler
    check_type.check_str('servable_directory', servable_directory)
    check_type.check_str('servable_name', servable_name)
    check_type.check_int('version_number', version_number, 0)
    check_type.check_str('device_type', device_type)
    check_type.check_int('device_id', device_id, 0)

    check_type.check_str('master_address', master_address)
    check_type.check_bool('listening_master', listening_master)
    check_type.check_bool('enable_lite', enable_lite)

    ExitSignalHandle_.start()  # Set flag to running and receive Ctrl+C message

    # for servable_config.py to get device id of current worker.
    os.environ["SERVING_DEVICE_ID"] = str(device_id)
    worker_pid = os.getpid()
    unix_socket_dir = "unix_socket_files"
    try:
        os.mkdir(unix_socket_dir)
    except FileExistsError:
        pass
    worker_address = f"unix:{unix_socket_dir}/serving_worker_{servable_name}_device{device_id}_{worker_pid}"
    if len(worker_address) > 107:  # maximum unix domain socket address length
        worker_address = worker_address[:50] + "___" + worker_address[-50:]
    try:
        worker.start_servable(servable_directory=servable_directory, servable_name=servable_name,
                              version_number=version_number, device_type=device_type, device_id=device_id,
                              master_address=master_address, worker_address=worker_address,
                              dec_key=dec_key, dec_mode=dec_mode, enable_lite=enable_lite)
    except Exception as ex:
        Worker_.notify_failed(master_address,
                              f"{{servable name:{servable_name}, device id:{device_id}, <{ex}>}}")
        raise


def parse_args_and_start():
    """Parse args and start distributed worker"""
    parser = argparse.ArgumentParser(description="Serving start extra worker")
    parser.add_argument('--servable_directory', type=str, required=True, help="servable directory")
    parser.add_argument('--servable_name', type=str, required=True, help="servable name")
    parser.add_argument('--version_number', type=int, required=True, help="version numbers")
    parser.add_argument('--device_type', type=str, required=True, help="device type")
    parser.add_argument('--device_id', type=str, required=True, help="device id")
    parser.add_argument('--master_address', type=str, required=True, help="master address")
    parser.add_argument('--enable_lite', type=str, required=True, help="enable lite")
    parser.add_argument('--dec_key_pipe_file', type=str, required=True, help="dec key pipe file")
    parser.add_argument('--dec_mode', type=str, required=True, help="dec mode")
    parser.add_argument('--listening_master', type=str, required=True, help="whether listening master")
    args = parser.parse_args()

    servable_directory = args.servable_directory
    servable_name = args.servable_name
    version_number = int(args.version_number)
    device_type = args.device_type
    device_id = int(args.device_id)
    master_address = args.master_address
    dec_key_pipe = args.dec_key_pipe_file
    if dec_key_pipe != "None":
        with open(dec_key_pipe, "rb") as fp:
            dec_key = fp.read()
        prefix = "serving_temp_dec_"
        if dec_key_pipe[:len(prefix)] == prefix:
            os.remove(dec_key_pipe)
    else:
        dec_key = None
    dec_mode = args.dec_mode
    # pylint: disable=simplifiable-if-expression
    listening_master = True if args.listening_master.lower() == "true" else False

    # pylint: disable=simplifiable-if-expression
    enable_lite = True if args.enable_lite.lower() == "true" else False
    start_worker(servable_directory, servable_name, version_number, device_type, device_id, master_address,
                 dec_key, dec_mode, listening_master, enable_lite)


if __name__ == '__main__':
    parse_args_and_start()

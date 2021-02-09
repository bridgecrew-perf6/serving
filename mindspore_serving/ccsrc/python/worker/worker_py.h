/**
 * Copyright 2020 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINDSPORE_SERVING_WORKER_PY_H
#define MINDSPORE_SERVING_WORKER_PY_H

#include <string>
#include <unordered_map>
#include "worker/worker.h"
#include "worker/task_queue.h"
#include "python/tensor_py.h"

namespace mindspore::serving {

class MS_API PyWorker {
 public:
  static void InitServingLog();
  static void StartServable(const std::string &model_directory, const std::string &model_name, uint32_t version_number,
                            const std::string &master_ip, uint32_t master_port, const std::string &host_ip,
                            uint32_t host_port);

  static void StartServableInMaster(const std::string &model_directory, const std::string &model_name,
                                    uint32_t version_number);

  static void StartDistributedServable(const std::string &servable_directory, const std::string &servable_name,
                                       const std::string &rank_table_json_file, uint32_t version_number,
                                       const std::string &worker_ip, uint32_t worker_port, const std::string &master_ip,
                                       uint32_t master_port, uint32_t wait_agents_time_in_seconds);

  static void StartDistributedServableInMaster(const std::string &servable_directory, const std::string &servable_name,
                                               const std::string &rank_table_json_file, uint32_t version_number,
                                               const std::string &worker_ip, uint32_t worker_port,
                                               uint32_t wait_agents_time_in_seconds);

  static int GetBatchSize();
  static void WaitAndClear();
  static void StopAndClear();
  static TaskItem GetPyTask();
  static TaskItem TryGetPreprocessPyTask();
  static TaskItem TryGetPostprocessPyTask();
  static void PushPreprocessPyResult(const py::tuple &output_batch);
  static void PushPreprocessPyFailed(int count);

  static void PushPostprocessPyResult(const py::tuple &output_batch);
  static void PushPostprocessPyFailed(int count);

 private:
  static void OnEndStartServable(const std::string &servable_directory, const std::string &servable_name,
                                 uint32_t spec_version_number, uint32_t started_version_number);
};

}  // namespace mindspore::serving

#endif  // MINDSPORE_SERVING_WORKER_PY_H

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

#ifndef MINDSPORE_SERVING_WROKER_LOCAL_NOTIFY_H
#define MINDSPORE_SERVING_WROKER_LOCAL_NOTIFY_H
#include <vector>
#include "worker/notfiy_master/base_notify.h"

namespace mindspore {
namespace serving {

class MS_API LocalNotifyMaster : public BaseNotifyMaster {
 public:
  LocalNotifyMaster() = default;
  ~LocalNotifyMaster() override = default;
  Status Register(const std::vector<WorkerSpec> &worker_specs) override;
  Status Unregister() override;
  Status AddWorker(const WorkerSpec &worker_spec) override;
  Status RemoveWorker(const WorkerSpec &worker_spec) override;
};

}  // namespace serving
}  // namespace mindspore

#endif  // MINDSPORE_SERVING_WROKER_LOCAL_NOTIFY_H

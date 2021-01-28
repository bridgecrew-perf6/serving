/**
 * Copyright 2021 Huawei Technologies Co., Ltd
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

#ifndef MINDSPORE_SERVING_WORKER_NOTIFY_WORKER_H
#define MINDSPORE_SERVING_WORKER_NOTIFY_WORKER_H
#include <vector>
#include <string>
#include <memory>
#include "worker/distributed_worker/notify_distributed/base_notify_worker.h"
#include "proto/ms_master.pb.h"
#include "proto/ms_master.grpc.pb.h"
#include "proto/ms_distributed.pb.h"
#include "proto/ms_distributed.grpc.pb.h"
namespace mindspore {
namespace serving {

class MS_API GrpcNotfiyDistributeWorker : public BaseNotifyDistributeWorker {
 public:
  GrpcNotfiyDistributeWorker(const std::string &master_ip, uint32_t master_port, const std::string &host_ip,
                             uint32_t host_port);
  ~GrpcNotfiyDistributeWorker() override;
  Status Register(const std::vector<WorkerAgentSpec> &worker_specs) override;
  Status Unregister() override;

 private:
  std::string distributed_worker_ip_;
  uint32_t distributed_worker_port_;
  std::string host_ip_;
  uint32_t host_port_;
  std::string agent_address_;
  std::string distributed_worker_address_;
  std::unique_ptr<proto::MSDistributedWorker::Stub> stub_;
  std::atomic<bool> is_stoped_{false};
};

}  // namespace serving
}  // namespace mindspore

#endif  // MINDSPORE_SERVING_WORKER_NOTIFY_WORKER_H

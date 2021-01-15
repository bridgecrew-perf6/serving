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

#ifndef MINDSPORE_SERVING_MASTER_DISPACTHER_H
#define MINDSPORE_SERVING_MASTER_DISPACTHER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <shared_mutex>
#include "proto/ms_worker.grpc.pb.h"
#include "common/serving_common.h"
#include "common/instance.h"
#include "common/servable.h"
#include "master/notify_worker/base_notify.h"
#include "master/grpc/grpc_client.h"

namespace mindspore::serving {

struct DispatcherWorkerContext {
  WorkerSpec worker_spec;
  std::shared_ptr<BaseNotifyWorker> notify_worker_ = nullptr;
};

class MS_API Dispatcher {
 public:
  Dispatcher();
  ~Dispatcher();
  Status Dispatch(const proto::PredictRequest &request, proto::PredictReply *reply);
  Status DispatchAsync(const proto::PredictRequest &request, proto::PredictReply *reply, DispatchCallback callback);

  Status RegisterServable(const proto::RegisterRequest &request, proto::RegisterReply *reply);
  Status UnregisterServable(const proto::ExitRequest &request, proto::ExitReply *reply);

  Status AddServable(const proto::AddWorkerRequest &request, proto::AddWorkerReply *reply);
  Status RemoveServable(const proto::RemoveWorkerRequest &request, proto::RemoveWorkerReply *reply);

  void Clear();

  Status RegisterLocalServable(const std::vector<WorkerSpec> &worker_specs);
  Status UnregisterLocalServable();
  Status AddLocalServable(const WorkerSpec &worker_spec);
  Status RemoveLocalServable(const WorkerSpec &worker_spec);

 private:
  std::unordered_map<std::string, std::vector<DispatcherWorkerContext>> servable_map_{};

  std::shared_mutex servable_shared_lock_;
  // avoid invoke Clear and then UnregisterServable is invoked by Clear in other thread
  std::atomic_bool clearing_flag = false;

  DispatcherWorkerContext GetWorkSession(const RequestSpec &request_spec) const;

  using CreateNotifyWorkerFunc = std::function<std::shared_ptr<BaseNotifyWorker>(const WorkerSpec &worker_spec)>;

  Status RegisterServableCommon(const std::vector<WorkerSpec> &worker_specs, CreateNotifyWorkerFunc func);
  Status UnregisterServableCommon(const std::string &worker_address);
  Status AddServableCommon(const WorkerSpec &worker_spec, CreateNotifyWorkerFunc func);
  Status RemoveServableCommon(const WorkerSpec &worker_spec);
};

}  // namespace mindspore::serving

#endif  // MINDSPORE_SERVING_MASTER_DISPACTHER_H

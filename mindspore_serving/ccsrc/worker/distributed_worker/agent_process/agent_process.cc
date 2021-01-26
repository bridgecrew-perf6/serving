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

#include "worker/distributed_worker/agent_process/agent_process.h"

namespace mindspore {
namespace serving {
grpc::Status MSAgentImpl::Exit(grpc::ServerContext *context, const proto::ExitRequest *request,
                               proto::ExitReply *reply) {
  MSI_LOG(INFO) << "Distributed Worker Exit";
  // to do : need WorkerAgent support stop funcition
  return grpc::Status::OK;
}

grpc::Status MSAgentImpl::Predict(grpc::ServerContext *context, const proto::PredictRequest *request,
                                  proto::PredictReply *reply) {
  MSI_LOG(INFO) << "Begin call service Eval";
  // to do : need WorkerAgent support run funcition
  return grpc::Status::OK;
}

}  // namespace serving
}  // namespace mindspore

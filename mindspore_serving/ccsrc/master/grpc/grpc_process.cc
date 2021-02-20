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

#include "master/grpc/grpc_process.h"
#include <string>
#include "master/dispacther.h"

namespace mindspore {
namespace serving {

namespace {
std::string GetProtorWorkerSpecRepr(const proto::WorkerSpec &worker_spec) {
  std::stringstream str;
  str << "{name:" << worker_spec.name() << ", version:" << worker_spec.version_number() << ", method:[";
  for (int k = 0; k < worker_spec.methods_size(); k++) {
    str << worker_spec.methods(k).name();
    if (k + 1 < worker_spec.methods_size()) {
      str << ",";
    }
  }
  str << "]}";
  return str.str();
}
}  // namespace

Status MSServiceImpl::PredictAsync(const proto::PredictRequest *request, proto::PredictReply *reply,
                                   DispatchCallback callback) {
  MSI_EXCEPTION_IF_NULL(request);
  MSI_EXCEPTION_IF_NULL(reply);
  Status status(FAILED);
  auto on_status = [reply](Status status) {
    if (status != SUCCESS) {
      reply->clear_error_msg();
      auto proto_error_msg = reply->add_error_msg();
      proto_error_msg->set_error_code(FAILED);
      std::string error_msg = status.StatusMessage();
      if (error_msg.empty()) {
        proto_error_msg->set_error_msg("Predict failed");
      } else {
        proto_error_msg->set_error_msg(error_msg);
      }
    }
  };
  DispatchCallback callback_with_status_handle = [callback, on_status](Status status) {
    on_status(status);
    callback(status);
  };
  try {
    MSI_TIME_STAMP_START(Predict)
    status = dispatcher_->DispatchAsync(*request, reply, callback_with_status_handle);
    MSI_TIME_STAMP_END(Predict)
  } catch (const std::bad_alloc &ex) {
    MSI_LOG(ERROR) << "Serving Error: malloc memory failed";
    std::cout << "Serving Error: malloc memory failed" << std::endl;
  } catch (const std::runtime_error &ex) {
    MSI_LOG(ERROR) << "Serving Error: runtime error occurred: " << ex.what();
    std::cout << "Serving Error: runtime error occurred: " << ex.what() << std::endl;
  } catch (const std::exception &ex) {
    MSI_LOG(ERROR) << "Serving Error: exception occurred: " << ex.what();
    std::cout << "Serving Error: exception occurred: " << ex.what() << std::endl;
  } catch (...) {
    MSI_LOG(ERROR) << "Serving Error: exception occurred";
    std::cout << "Serving Error: exception occurred";
  }
  MSI_LOG(INFO) << "Finish call service Eval";

  if (status != SUCCESS) {
    on_status(status);
    return status;
  }
  return SUCCESS;
}

grpc::Status MSMasterImpl::Register(grpc::ServerContext *context, const proto::RegisterRequest *request,
                                    proto::RegisterReply *reply) {
  MSI_EXCEPTION_IF_NULL(request);
  MSI_EXCEPTION_IF_NULL(reply);
  auto worker_sig = [request]() {
    std::stringstream str;
    str << "worker address: " << request->address() << ", servables: [";
    for (int i = 0; i < request->worker_spec_size(); i++) {
      str << GetProtorWorkerSpecRepr(request->worker_spec(i));
      if (i + 1 < request->worker_spec_size()) {
        str << ", ";
      }
    }
    str << "]";
    return str.str();
  };
  Status status(FAILED);
  status = dispatcher_->RegisterServable(*request, reply);
  if (status != SUCCESS) {
    MSI_LOG_ERROR << "Register servable failed, " << worker_sig();
    return grpc::Status::OK;
  }
  watcher_->StartWatch(request->address());
  MSI_LOG(INFO) << "Register success: " << worker_sig();
  return grpc::Status::OK;
}

grpc::Status MSMasterImpl::AddWorker(grpc::ServerContext *context, const proto::AddWorkerRequest *request,
                                     proto::AddWorkerReply *reply) {
  MSI_EXCEPTION_IF_NULL(request);
  MSI_EXCEPTION_IF_NULL(reply);
  auto worker_sig = [request]() {
    std::stringstream str;
    str << "worker address: " << request->address()
        << ", servables: " << GetProtorWorkerSpecRepr(request->worker_spec());
    return str.str();
  };
  Status status(FAILED);
  status = dispatcher_->AddServable(*request, reply);
  if (status != SUCCESS) {
    MSI_LOG_ERROR << "Add servable failed, " << worker_sig();
    return grpc::Status::OK;
  }
  watcher_->StartWatch(request->address());
  MSI_LOG(INFO) << "Add success, " << worker_sig();
  return grpc::Status::OK;
}

grpc::Status MSMasterImpl::RemoveWorker(grpc::ServerContext *context, const proto::RemoveWorkerRequest *request,
                                        proto::RemoveWorkerReply *reply) {
  MSI_EXCEPTION_IF_NULL(request);
  MSI_EXCEPTION_IF_NULL(reply);
  auto worker_sig = [request]() {
    std::stringstream str;
    str << "worker address: " << request->address()
        << ", servables: " << GetProtorWorkerSpecRepr(request->worker_spec());
    return str.str();
  };
  Status status(FAILED);
  watcher_->StopWatch(request->address());
  status = dispatcher_->RemoveServable(*request, reply);
  if (status != SUCCESS) {
    MSI_LOG_ERROR << "Add servable failed, " << worker_sig();
    return grpc::Status::OK;
  }
  MSI_LOG(INFO) << "Add success, " << worker_sig();
  return grpc::Status::OK;
}

grpc::Status MSMasterImpl::Exit(grpc::ServerContext *context, const proto::ExitRequest *request,
                                proto::ExitReply *reply) {
  MSI_EXCEPTION_IF_NULL(request);
  MSI_EXCEPTION_IF_NULL(reply);
  auto worker_sig = [request]() {
    std::stringstream str;
    str << "worker address: " << request->address();
    return str.str();
  };

  MSI_LOG(INFO) << "Worker Exit, " << worker_sig();
  Status status(FAILED);
  watcher_->StopWatch(request->address());
  status = dispatcher_->UnregisterServable(*request, reply);
  if (status != SUCCESS) {
    MSI_LOG_ERROR << "UnRegister servable failed, " << worker_sig();
    return grpc::Status::OK;
  }
  return grpc::Status::OK;
}
grpc::Status MSMasterImpl::Ping(grpc::ServerContext *context, const proto::PingRequest *request,
                                proto::PingReply *reply) {
  MSI_EXCEPTION_IF_NULL(request);
  MSI_EXCEPTION_IF_NULL(reply);
  watcher_->RecvPing(request->address());
  return grpc::Status::OK;
}

grpc::Status MSMasterImpl::Pong(grpc::ServerContext *context, const proto::PongRequest *request,
                                proto::PongReply *reply) {
  MSI_EXCEPTION_IF_NULL(request);
  MSI_EXCEPTION_IF_NULL(reply);
  watcher_->RecvPong(request->address());
  return grpc::Status::OK;
}
}  // namespace serving
}  // namespace mindspore

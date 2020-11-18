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

#ifndef MINDSPORE_RESTFUL_REQUEST_H
#define MINDSPORE_RESTFUL_REQUEST_H

#include <event2/event.h>
#include <event2/http.h>
#include <string>
#include <memory>
#include <nlohmann/json.hpp>
#include "common/serving_common.h"

namespace mindspore {
namespace serving {

class DecomposeEvRequest {
 public:
  explicit DecomposeEvRequest(struct evhttp_request *request, int max_msg_size);
  ~DecomposeEvRequest();
  std::string UrlQuery(const std::string &url, const std::string &key);
  Status CheckRequestMethodValid();
  Status Decompose();
  Status GetPostMessageToJson();

  evhttp_request *event_request_;
  std::string request_method_;
  std::string model_name_;
  std::string url_;
  std::string service_method_;
  uint32_t version_{};
  uint32_t max_msg_size_{};
  nlohmann::json request_message_;
};

class RestfulRequest {
 public:
  explicit RestfulRequest(std::shared_ptr<DecomposeEvRequest> request);
  ~RestfulRequest();

  RestfulRequest(const RestfulRequest &other) = delete;
  RestfulRequest &operator=(const RestfulRequest &other) = delete;

  Status RestfulReplayBufferInit();
  Status RestfulReplay(const std::string &replay);
  Status ErrorMessage(Status status);
  std::shared_ptr<DecomposeEvRequest> decompose_event_request() { return decompose_event_request_; }

 private:
  std::shared_ptr<DecomposeEvRequest> decompose_event_request_{nullptr};
  evbuffer *replay_buffer_ = nullptr;
};

}  // namespace serving
}  // namespace mindspore

#endif  // MINDSPORE_RESTFUL_REQUEST_H

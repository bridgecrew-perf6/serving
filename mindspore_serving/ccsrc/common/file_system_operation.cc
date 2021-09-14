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
#include "common/file_system_operation.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <ctime>
#include "common/serving_common.h"

namespace mindspore {
namespace serving {
bool DirOrFileExist(const std::string &file_path) {
  int ret = access(file_path.c_str(), 0);
  return (ret == -1) ? false : true;
}
}  // namespace serving
}  // namespace mindspore

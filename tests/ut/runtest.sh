#!/bin/bash
# Copyright 2019 Huawei Technologies Co., Ltd
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

set -e

CURRPATH=$(
  cd "$(dirname $0)" || exit
  pwd
)

if [ $# -gt 0 ]; then
  if [ $1 == "python" ]; then
    echo "run python ut"
    bash ${CURRPATH}/python/runtest.sh $2
  elif [ $1 == "cpp" ]; then
    echo "run cpp ut"
    bash ${CURRPATH}/cpp/runtest.sh
  fi
else
  echo "run all ut"
  # 1.run python testcases
  bash ${CURRPATH}/python/runtest.sh $2

  # 2.run c++ ut testcases
  bash ${CURRPATH}/cpp/runtest.sh
fi

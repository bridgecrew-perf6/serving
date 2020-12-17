# Copyright 2020 Huawei Technologies Co., Ltd
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
"""add model servable config"""

from mindspore_serving.worker import register
import numpy as np


# define preprocess pipeline, the function arg is multi instances, every instance is the tuple of inputs
# this example has one input and one output
def add_trans_datatype(instances):
    """preprocess python implement"""
    for instance in instances:
        x1 = instance[0]
        x2 = instance[1]
        yield x1.astype(np.float32), x2.astype(np.float32)


# when with_batch_dim is set to False, only 2x2 add is supported
# when with_batch_dim is set to True(default), Nx2 add is supported, while N is viewed as batch
# float32 inputs/outputs
register.declare_servable(servable_file="tensor_add.mindir", model_format="MindIR", with_batch_dim=False)


# register add_common method in add
@register.register_method(output_names=["y"])
def add_common(x1, x2):  # only support float32 inputs
    """method add_common data flow definition, only call model servable"""
    y = register.call_servable(x1, x2)
    return y


# register add_cast method in add
@register.register_method(output_names=["y"])
def add_cast(x1, x2):
    """method add_cast data flow definition, only call preprocess and model servable"""
    x1, x2 = register.call_preprocess(add_trans_datatype, x1, x2)  # cast input to float32
    y = register.call_servable(x1, x2)
    return y

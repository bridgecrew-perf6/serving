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
"""Servable register interface, used in servable_config.py of one servable.
See how to configure servable_config.py file, please refer to
`Servable Provided Through Model Configuration <https://www.mindspore.cn/serving/docs/zh-CN/master/serving_model.html>`_."""

from .servable import declare_servable, AclOptions, GpuOptions
from .method import register_pipeline, register_method, call_preprocess, call_servable, call_postprocess
from .method import call_preprocess_pipeline, call_postprocess_pipeline
from .pipeline import PipelineServable

__all__ = []
__all__.extend([
    "declare_servable",
    "AclOptions",
    "GpuOptions",
    "PipelineServable",
    "register_pipeline",
    'register_method',
    'call_preprocess',
    'call_preprocess_pipeline',
    'call_servable',
    'call_postprocess',
    'call_postprocess_pipeline'
])

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
#ifndef MINDSPORE_INCLUDE_API_MODEL_H
#define MINDSPORE_INCLUDE_API_MODEL_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <utility>
#include "include/api/status.h"
#include "include/api/types.h"
#include "include/api/graph.h"
#include "include/api/context.h"
#include "include/api/callback/callback.h"
#include "include/api/cell.h"
#include "include/api/cfg.h"
#include "include/api/dual_abi_helper.h"

namespace mindspore {
class ModelImpl;
class Metrics;

namespace dataset {
class Dataset;
}  // namespace dataset
/// \brief The Model class is used to define a MindSpore model, facilitating computational graph management.
class MS_API Model {
 public:
  Model();
  ~Model();
  Model(const Model &) = delete;
  void operator=(const Model &) = delete;
  /// \brief Builds a model so that it can run on a device.
  ///
  /// \param[in] graph GraphCell is a derivative of Cell. Cell is not available currently. GraphCell can be constructed
  /// from Graph, for example, model.Build(GraphCell(graph), context).
  /// \param[in] model_context A context used to store options during execution.
  /// \param[in] train_cfg A config used by training.
  ///
  /// \return Status.
  Status Build(GraphCell graph, const std::shared_ptr<Context> &model_context = nullptr,
               const std::shared_ptr<TrainCfg> &train_cfg = nullptr);

  /// \brief Resizes the shapes of inputs.
  ///
  /// \param[in] inputs A vector that includes all input tensors in order.
  /// \param[in] dims Defines the new shapes of inputs, should be consistent with inputs.
  ///
  /// \return Status.
  Status Resize(const std::vector<MSTensor> &inputs, const std::vector<std::vector<int64_t>> &dims);

  /// \brief Inference model.
  ///
  /// \param[in] inputs A vector where model inputs are arranged in sequence.
  /// \param[out] outputs Which is a pointer to a vector. The model outputs are filled in the container in sequence.
  /// \param[in] before CallBack before predict.
  /// \param[in] after CallBack after predict.
  ///
  /// \return Status.
  Status Predict(const std::vector<MSTensor> &inputs, std::vector<MSTensor> *outputs,
                 const MSKernelCallBack &before = nullptr, const MSKernelCallBack &after = nullptr);

  /// \brief Obtains all input tensors of the model.
  ///
  /// \return The vector that includes all input tensors.
  std::vector<MSTensor> GetInputs();
  /// \brief Obtains the input tensor of the model by name.
  ///
  /// \return The input tensor with the given name, if the name is not found, an invalid tensor is returned.
  inline MSTensor GetInputByTensorName(const std::string &tensor_name);

  Status InitMetrics(std::vector<Metrics *> metrics);
  std::vector<Metrics *> GetMetrics();

  /// \brief Obtains all output tensors of the model.
  ///
  /// \return The vector that includes all output tensors.
  std::vector<MSTensor> GetOutputs();
  /// \brief Obtains names of all output tensors of the model.
  ///
  /// \return A vector that includes names of all output tensors.
  inline std::vector<std::string> GetOutputTensorNames();
  /// \brief Obtains the output tensor of the model by name.
  ///
  /// \return The output tensor with the given name, if the name is not found, an invalid tensor is returned.
  inline MSTensor GetOutputByTensorName(const std::string &tensor_name);
  inline std::vector<MSTensor> GetOutputsByNodeName(const std::string &tensor_name);

  /// \brief Inference model.
  ///
  /// \param[in] device_type Device type，options are kGPU, kAscend910, etc.
  /// \param[in] model_type The type of model file, options are ModelType::kMindIR, ModelType::kOM.
  ///
  /// \return Is supported or not.
  static bool CheckModelSupport(enum DeviceType device_type, ModelType model_type);

  Status SetTrainMode(bool train);
  bool GetTrainMode() const;
  Status Train(int epochs, std::shared_ptr<dataset::Dataset> ds, std::vector<TrainCallBack *> cbs);
  Status Evaluate(std::shared_ptr<dataset::Dataset> ds, std::vector<TrainCallBack *> cbs);
  Status Build(const void *model_data, size_t data_size, ModelType model_type,
               const std::shared_ptr<Context> &model_context = nullptr, const Key &dec_key = {},
               const std::string &dec_mode = kDecModeAesGcm);
  Status Build(const std::string &model_path, ModelType model_type,
               const std::shared_ptr<Context> &model_context = nullptr, const Key &dec_key = {},
               const std::string &dec_mode = kDecModeAesGcm);

 private:
  friend class Serialization;
  // api without std::string
  MSTensor GetInputByTensorName(const std::vector<char> &tensor_name);
  std::vector<std::vector<char>> GetOutputTensorNamesChar();
  MSTensor GetOutputByTensorName(const std::vector<char> &tensor_name);
  std::vector<MSTensor> GetOutputsByNodeName(const std::vector<char> &node_name);

  std::shared_ptr<ModelImpl> impl_;
};

MSTensor Model::GetInputByTensorName(const std::string &tensor_name) {
  return GetInputByTensorName(StringToChar(tensor_name));
}

std::vector<std::string> Model::GetOutputTensorNames() { return VectorCharToString(GetOutputTensorNamesChar()); }

MSTensor Model::GetOutputByTensorName(const std::string &tensor_name) {
  return GetOutputByTensorName(StringToChar(tensor_name));
}

std::vector<MSTensor> Model::GetOutputsByNodeName(const std::string &tensor_name) {
  return GetOutputsByNodeName(StringToChar(tensor_name));
}
}  // namespace mindspore
#endif  // MINDSPORE_INCLUDE_API_MODEL_H

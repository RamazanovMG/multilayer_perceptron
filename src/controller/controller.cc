/*******************************************************************************
 * @copyright Copyright [2023] <Morris Roggo, Phylis Cristobal>
 * @brief Реализация многослойного перцептрона CPP7_MLP-1.
 * @file controller.h
 * @date 2023-09-20
 * @author morrisro, phyliscr
 ********************************************************************************/
#include "controller.h"

namespace s21 {

Controller::Controller(int model_type, int input_amount_of_hidden_layers,
                       double input_learning_rate,
                       int input_size_of_hidden_layers)
    : model_graph_(nullptr), model_matrix_(nullptr), model_type_(model_type) {
  if (model_type_ == Controller::GRAPH) {
    model_graph_ =
        new ModelGraph(input_amount_of_hidden_layers, input_learning_rate,
                       input_size_of_hidden_layers);
    connect(model_graph_, &ModelGraph::SignalProgress, this,
            &Controller::SlotProgress);
    connect(model_graph_, &ModelGraph::SignalGraph, this,
            &Controller::SlotGraph);
  }
  if (model_type_ == Controller::MATRIX) {
    model_matrix_ =
        new ModelMatrix(input_amount_of_hidden_layers, input_learning_rate,
                        input_size_of_hidden_layers);
    connect(model_matrix_, &ModelMatrix::SignalProgress, this,
            &Controller::SlotProgress);
    connect(model_matrix_, &ModelMatrix::SignalGraph, this,
            &Controller::SlotGraph);
  }
}

Controller::Controller(const std::string filename)
    : model_graph_(nullptr),
      model_matrix_(nullptr),
      model_type_(Controller::GRAPH) {
  model_graph_ = new ModelGraph(filename);
}

Controller::~Controller() {
  delete model_graph_;
  delete model_matrix_;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                 TRAIN                  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
std::vector<double> Controller::Train(int input_amount_of_epochs,
                                      const std::string file_path_train,
                                      const std::string file_path_test) {
  std::vector<double> result;
  if (model_type_ == Controller::GRAPH)
    result = model_graph_->train(input_amount_of_epochs, file_path_train,
                                 file_path_test);

  if (model_type_ == Controller::MATRIX)
    result = model_matrix_->train(input_amount_of_epochs, file_path_train,
                                  file_path_test);
  return result;
}

std::vector<double> Controller::Train(int input_amount_of_epochs,
                                      const std::string file_path_train) {
  std::vector<double> result;
  if (model_type_ == Controller::GRAPH)
    result =
        model_graph_->train_k_fold(input_amount_of_epochs, file_path_train);
  if (model_type_ == Controller::MATRIX)
    result =
        model_matrix_->train_k_fold(input_amount_of_epochs, file_path_train);
  return result;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                 OTHERS                 //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void Controller::test(const std::string &line) {}

std::vector<std::vector<double>> Controller::partial_testing(
    double fraction, const std::string file_path_test) {
  std::vector<std::vector<double>> result;
  if (model_type_ == Controller::GRAPH)
    result = model_graph_->partial_testing(fraction, file_path_test);
  if (model_type_ == Controller::MATRIX)
    result = model_matrix_->partial_testing(fraction, file_path_test);
  return result;
}

int Controller::test(int input_layer[kInputLayerSize]) {
  int result = -1;
  if (model_type_ == Controller::GRAPH)
    result = model_graph_->test(input_layer);
  if (model_type_ == Controller::MATRIX)
    result = model_matrix_->test(input_layer);
  return result;
}

void Controller::save_weights(const std::string filename) {
  if (model_type_ == Controller::GRAPH) model_graph_->save_weights(filename);
  if (model_type_ == Controller::MATRIX) model_matrix_->save_weights(filename);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                 SLOTS                  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void Controller::SlotProgress(int progress) { emit SignalProgress(progress); }

void Controller::SlotGraph(int epoch, double test_percent) {
  emit SignalGraph(epoch, test_percent);
}

}  // namespace s21

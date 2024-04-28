/*******************************************************************************
 * @copyright Copyright [2023] <Phylis Cristobal, Morris Roggo>
 * @brief Реализация многослойного перцептрона CPP7_MLP-1.
 * @file model_matrix.h
 * @date 2023-09-20
 * @author morrisro, phyliscr
 ********************************************************************************/

#ifndef CPP7_MLP_MODEL_MODEL_MATRIX_H
#define CPP7_MLP_MODEL_MODEL_MATRIX_H
const int kInputLayerSizeMatrix = 784;
const int kOutputLayerSizeMatrix = 26;

#ifndef TESTS
#include <QObject>
#endif
#include <string>
#include <vector>

#include "s21_matrix_oop.h"

namespace s21 {
#ifdef TESTS
class ModelMatrix {
#else
class ModelMatrix : public QObject {
  Q_OBJECT
#endif

 public:
  ModelMatrix(int, double, int);
  explicit ModelMatrix(const std::string &);
  std::vector<double> train(int, std::string file_path_train,
                            std::string file_path_test);
  std::vector<double> train_k_fold(int, std::string file_path_train);
  int test(const int[kInputLayerSizeMatrix]);
  int global_counter_;
  void save_weights(std::string);
  std::vector<std::vector<double>> partial_testing(double, std::string);

#ifndef TESTS
 signals:
  void SignalProgress(int);
  void SignalGraph(int, double);
#endif

 private:
  int amount_of_hidden_layers_;
  double learning_rate_;
  int size_of_hidden_layers_;
  std::vector<S21Matrix> layers_;
  std::vector<S21Matrix> error_terms_;
  std::vector<S21Matrix> weights_;
  std::vector<S21Matrix> gradient_descent_;
  std::vector<std::vector<int>> confusion_matrix_;
  void load_weights(std::string);
  void gradient_step(const std::string &);
  void test(const std::string &);

  void init_random_weights_and_biases();
  void apply_sigmoid(S21Matrix &);
  void apply_gradient_descent(int);
};
}  // namespace s21

#endif  // CPP7_MLP_MODEL_MODEL_MATRIX_H

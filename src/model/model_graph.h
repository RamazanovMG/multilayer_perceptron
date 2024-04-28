/*******************************************************************************
 * @copyright Copyright [2023] <Phylis Cristobal, Morris Roggo>
 * @brief Реализация многослойного перцептрона CPP7_MLP-1.
 * @file model_graph.h
 * @date 2023-09-20
 * @author morrisro, phyliscr
 ********************************************************************************/

#ifndef CPP7_MLP_MODEL_MODEL_GRAPH_H
#define CPP7_MLP_MODEL_MODEL_GRAPH_H

#ifndef TESTS
#include <QObject>
#endif
#include <string>
#include <vector>
const int kInputLayerSize = 784;
const int kOutputLayerSize = 26;

namespace s21 {

#ifdef TESTS
class ModelGraph {
#else
class ModelGraph : public QObject {
  Q_OBJECT
#endif

 public:
  ModelGraph(int, double, int);
  explicit ModelGraph(const std::string &);
  ~ModelGraph();

  std::vector<double> train(int, std::string file_path_train,
                            std::string file_path_test);
  std::vector<double> train_k_fold(int, std::string);
  int test(const int[kInputLayerSize]);
  std::vector<std::vector<double>> partial_testing(double, std::string);
  void save_weights(std::string);

#ifndef TESTS
 signals:
  void SignalProgress(int);
  void SignalGraph(int, double);
#endif

 private:
  class Node {
   public:
    Node(double, double);

    double value;
    double error;
    std::vector<double> weights;
  };

  void gradient_step(const std::string &);
  double apply_sigmoid(const double &) noexcept;
  void calculate_and_apply_gradient(int);
  void test(const std::string &);
  void load_weights(std::string);

  int amount_of_hidden_layers_;
  double learning_rate_;
  int size_of_hidden_layers_;
  std::vector<std::vector<Node *>> nodes;

  std::vector<std::vector<int>> confusion_matrix_;
};
}  // namespace s21

#endif  // CPP7_MLP_MODEL_MODEL_MATRIX_H

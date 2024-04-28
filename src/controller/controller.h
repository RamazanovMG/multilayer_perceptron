/*******************************************************************************
 * @copyright Copyright [2023] <Phylis Cristobal, Morris Roggo>
 * @brief Реализация многослойного перцептрона CPP7_MLP-1.
 * @file controller.h
 * @date 2023-09-20
 * @author phyliscr, morrisro
 ********************************************************************************/

#ifndef CPP7_MLP_MODEL_CONTROLLER_H_
#define CPP7_MLP_MODEL_CONTROLLER_H_

#include "../model/model_graph.h"
#include "../model/model_matrix.h"

namespace s21 {

#ifdef TESTS
class Controller {
#else
class Controller : public QObject {
  Q_OBJECT
#endif

public:
  enum Model { GRAPH, MATRIX };

  Controller(int, int, double, int);
  explicit Controller(std::string);
  ~Controller();

  Controller(const Controller &) = delete;
  Controller &operator=(const Controller &) = delete;

  // Train type - EPOCH
  std::vector<double> Train(int, std::string, std::string);
  // Train type - KFOLD
  std::vector<double> Train(int, std::string);
  int test(int[kInputLayerSize]);
  void test(const std::string &line);
  std::vector<std::vector<double>> partial_testing(double, std::string);
  void save_weights(std::string);

#ifndef TESTS
signals:
  void SignalProgress(int);
  void SignalGraph(int, double);

public slots:
  void SlotProgress(int);
  void SlotGraph(int, double);
#endif

private:
  int model_type_;
  ModelMatrix *model_matrix_;
  ModelGraph *model_graph_;
};

} // namespace s21
#endif // CPP7_MLP_MODEL_CONTROLLER_H_

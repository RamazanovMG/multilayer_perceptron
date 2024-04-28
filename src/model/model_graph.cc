/*******************************************************************************
 * @copyright Copyright [2023] <Morris Roggo, Phylis Cristobal>
 * @brief Реализация многослойного перцептрона CPP7_MLP-1.
 * @file model_graph.cc
 * @date 2023-09-20
 * @author morrisro, phyliscr
 ********************************************************************************/

#include "model_graph.h"

#include <clocale>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

namespace s21 {

ModelGraph::Node::Node(double input_value, double input_error)
    : value(input_value), error(input_error) {}

ModelGraph::ModelGraph(int input_amount_of_hidden_layers,
                       double input_learning_rate,
                       int input_size_of_hidden_layers)
    : amount_of_hidden_layers_(input_amount_of_hidden_layers),
      learning_rate_(input_learning_rate),
      size_of_hidden_layers_(input_size_of_hidden_layers) {
  confusion_matrix_.resize(26);
  for (int i = 0; i < 26; ++i) confusion_matrix_[i].resize(4);
  for (int i = 0; i < input_amount_of_hidden_layers + 2;
       ++i) {  // Инициализируем входной, выходной и внутренние слои
    std::vector<Node *> next_local_layer;
    if (i == 0) {
      for (int j = 0; j < kInputLayerSize + 1;
           ++j) {  // резервируем первый нейрон под биас с фиксированным
        // значением 1
        Node *local_node = new Node(1, 0);
        next_local_layer.push_back(local_node);
      }
    } else if (i != input_amount_of_hidden_layers + 1) {  // Внутренние слои
      for (int j = 0; j < input_size_of_hidden_layers + 1;
           ++j) {  //  резервируем первые нейроны каждого слоя под биасы с
        //  фиксированным значением 1
        Node *local_node = new Node(1, 0);
        next_local_layer.push_back(local_node);
      }
    } else {
      for (int j = 0; j < kOutputLayerSize; ++j) {
        Node *local_node = new Node(1, 0);
        next_local_layer.push_back(local_node);
      }
    }
    nodes.push_back(next_local_layer);
  }
  double lower_bound = -0.5;
  double upper_bound = 0.5;
  std::uniform_real_distribution<double> unif(lower_bound, upper_bound);
  std::default_random_engine re;
  for (int i = 1; i < input_amount_of_hidden_layers + 2;
       ++i) {  // Итерируемся с первого внутреннего слоя,
    // потому что у входного слоя нет весов
    for (size_t j = 0; j < nodes[i].size(); j++) {
      for (size_t k = 0; k < nodes[i - 1].size(); ++k)
        nodes[i][j]->weights.push_back(
            unif(re));  // Инитаем все нейроны текущего слоя весами,
      // связанными с каждым нейроном предыдущего слоя
    }
  }
}

ModelGraph::ModelGraph(const std::string &filename)
    : amount_of_hidden_layers_(0),
      learning_rate_(0.1),
      size_of_hidden_layers_(0) {
  confusion_matrix_.resize(26);
  for (int i = 0; i < 26; ++i) confusion_matrix_[i].resize(4);
  load_weights(filename);
}

std::vector<double> ModelGraph::train(int input_amount_of_epochs,
                                      std::string file_path_train,
                                      std::string file_path_test) {
  std::ifstream train_file(file_path_train);
  std::ifstream test_file(file_path_test);
  std::vector<double> success_rates;

  if (!train_file || !test_file) {
    throw std::ifstream::failure("Cannot open specified file");
  } else {
    // Переменная для подсчета строк
    int line_count = 0;

    std::string line;

    // Считываем файл построчно и увеличиваем счетчик строк
    while (std::getline(train_file, line)) {
      ++line_count;
    }

    for (int i = 0; i < input_amount_of_epochs; ++i) {
      int current_line = 0;

      for (int i2 = 0; i2 < kOutputLayerSize; ++i2) {
        for (int j = 0; j < 4; ++j) confusion_matrix_[i2][j] = 0;
      }
      train_file.clear();
      train_file.seekg(0);
      test_file.clear();
      test_file.seekg(0);

      int one_pecent_line = line_count / 100;
      while (std::getline(train_file, line)) {
        if (!(current_line % one_pecent_line)) {
#ifndef TESTS
          emit SignalProgress(current_line / one_pecent_line);
#endif
        }
        ++current_line;
        gradient_step(line);
      }

      while (std::getline(test_file, line)) test(line);

      int totalTP = 0;
      int totalFP = 0;
      for (int i2 = 0; i2 < kOutputLayerSize; ++i2) {
        totalTP += confusion_matrix_[i2][0];
        totalFP += confusion_matrix_[i2][2];
      }
      success_rates.push_back(100.0 * totalTP / (totalTP + totalFP));
// Отдаем в граф текущий рейт обучения в эпохе и тест этой эпохи
#ifndef TESTS
      emit SignalGraph(i + 1, success_rates.back());
#endif
    }
  }
  return success_rates;
}

std::vector<double> ModelGraph::train_k_fold(int k,
                                             std::string file_path_train) {
  std::ifstream train_file(file_path_train);
  std::vector<double> success_rates;

  if (!train_file) {
    throw std::ifstream::failure("Cannot open specified file");
  } else {
    // Переменная для подсчета строк
    int line_count = 0;

    std::string line;

    // Считываем файл построчно и увеличиваем счетчик строк
    while (std::getline(train_file, line)) {
      ++line_count;
    }

    int batch_size = line_count / k;
    for (int i = 0; i < k; ++i) {
      int current_line = 0;
      int one_pecent_line = (line_count / k * (k - 1)) / 100;

      for (int i2 = 0; i2 < kOutputLayerSize; ++i2) {
        for (int j = 0; j < 4; ++j) confusion_matrix_[i2][j] = 0;
      }
      train_file.clear();
      train_file.seekg(0);

      // int one_pecent_line = line_count / 100;
      for (int j = 0; j < k; ++j) {
        if (i == j) {
          for (int l = 0; l < batch_size; ++l) std::getline(train_file, line);
        } else {
          for (int l = 0; l < batch_size; ++l) {
            std::getline(train_file, line);
            if (!(current_line % one_pecent_line)) {
#ifndef TESTS
              emit SignalProgress(current_line / one_pecent_line);
#endif
            }
            ++current_line;
            gradient_step(line);
          }
        }
      }
      train_file.clear();
      train_file.seekg(0);
      for (int j = 0; j < k; ++j) {
        if (i == j) {
          for (int l = 0; l < batch_size; ++l) {
            std::getline(train_file, line);
            test(line);
          }
        } else {
          for (int l = 0; l < batch_size; ++l) std::getline(train_file, line);
        }
      }
      int totalTP = 0;
      int totalFP = 0;
      for (int i2 = 0; i2 < kOutputLayerSize; ++i2) {
        totalTP += confusion_matrix_[i2][0];
        totalFP += confusion_matrix_[i2][2];
      }
      success_rates.push_back(100.0 * totalTP / (totalTP + totalFP));
// Отдаем в граф текущий рейт обучения в эпохе и тест этой эпохи
#ifndef TESTS
      emit SignalGraph(i + 1, success_rates.back());
#endif
    }
  }
  return success_rates;
}

void ModelGraph::test(const std::string &line) {
  std::stringstream test(line);
  std::string segment;
  std::getline(test, segment, ',');
  int answer = std::stoi(segment) - 1;

  int input_index = 1;
  while (std::getline(test, segment,
                      ','))  // Заполняем входной слой числами из строки
    nodes[0][input_index++]->value = std::stoi(segment) / 255.0;

  for (int i = 1; i < amount_of_hidden_layers_ + 2;
       ++i) {  // Считаем нейроны, начиная с первого внутреннего слоя
    for (size_t j = i == amount_of_hidden_layers_ + 1 ? 0 : 1;
         j < nodes[i].size();
         ++j) {  // Во всех слоях, кроме последнего, скипаем первый нейрон,
      // потому что это резерв под биас, и его значение залочено
      // на 1. В последнем слое первому нейрону соответствует буква
      // 'A'
      double sum_function = 0;
      for (size_t k = 0; k < nodes[i][j]->weights.size(); ++k)
        sum_function += nodes[i - 1][k]->value * nodes[i][j]->weights[k];
      nodes[i][j]->value = apply_sigmoid(sum_function);
    }
  }
  int predicted_answer = 0;
  double probability = nodes[amount_of_hidden_layers_ + 1][0]->value;
  for (int i = 1; i < kOutputLayerSize; ++i) {
    if (nodes[amount_of_hidden_layers_ + 1][i]->value > probability) {
      probability = nodes[amount_of_hidden_layers_ + 1][i]->value;
      predicted_answer = i;
    }
  }

  for (int i = 0; i < 26; ++i) {
    if (answer == i) {
      if (predicted_answer == i)
        ++confusion_matrix_[i][0];
      else
        ++confusion_matrix_[i][3];
    } else {
      if (predicted_answer == i)
        ++confusion_matrix_[i][2];
      else
        ++confusion_matrix_[i][1];
    }
  }
}

int ModelGraph::test(const int input_layer[kInputLayerSize]) {
  for (int i = 0; i < kInputLayerSize; ++i)
    nodes[0][i + 1]->value = input_layer[i] / 255.0;
  for (int i = 1; i < amount_of_hidden_layers_ + 2;
       ++i) {  // Считаем нейроны, начиная с первого внутреннего слоя
    for (size_t j = i == amount_of_hidden_layers_ + 1 ? 0 : 1;
         j < nodes[i].size();
         ++j) {  // Во всех слоях, кроме последнего, скипаем первый нейрон,
      // потому что это резерв под биас, и его значение залочено
      // на 1. В последнем слое первому нейрону соответствует буква
      // 'A'
      double sum_function = 0;
      for (size_t k = 0; k < nodes[i][j]->weights.size(); ++k)
        sum_function += nodes[i - 1][k]->value * nodes[i][j]->weights[k];
      nodes[i][j]->value = apply_sigmoid(sum_function);
    }
  }
  int predicted_answer = 0;
  double probability = nodes[amount_of_hidden_layers_ + 1][0]->value;
  for (int i = 1; i < kOutputLayerSize; ++i) {
    if (nodes[amount_of_hidden_layers_ + 1][i]->value > probability) {
      probability = nodes[amount_of_hidden_layers_ + 1][i]->value;
      predicted_answer = i;
    }
  }
  return predicted_answer + 1;
}

std::vector<std::vector<double>> ModelGraph::partial_testing(
    double fraction, std::string file_path_test) {
  std::vector<std::vector<double>> output_data(27);
  std::ifstream test_file(file_path_test);

  if (!test_file) {
    throw std::ifstream::failure("Cannot open specified file");
  } else {
    for (int i = 0; i < 27; ++i) output_data[i].resize(4);
    for (int i = 0; i < kOutputLayerSize; ++i) {
      for (int j = 0; j < 4; ++j) confusion_matrix_[i][j] = 0;
    }
    // Переменная для подсчета строк
    int line_count = 0;

    std::string line;

    // Считываем файл построчно и увеличиваем счетчик строк
    while (std::getline(test_file, line)) {
      ++line_count;
    }
    test_file.clear();
    test_file.seekg(0);
    size_t iteraion_count = line_count * fraction;
    for (size_t i = 0; i < iteraion_count; ++i) {
      std::getline(test_file, line);
      test(line);
    }
    int totalTP = 0;
    int totalFP = 0;
    int totalFN = 0;
    for (int i = 0; i < kOutputLayerSize; ++i) {
      int TP = confusion_matrix_[i][0];
      int TN = confusion_matrix_[i][1];
      int FP = confusion_matrix_[i][2];
      int FN = confusion_matrix_[i][3];
      totalTP += TP;
      totalFP += FP;
      totalFN += FN;
      output_data[i][0] = (1.0 * TP + TN) / (TP + TN + FP + FN);
      output_data[i][1] = 1.0 * TP / (TP + FP);
      output_data[i][2] = 1.0 * TP / (TP + FN);
      output_data[i][3] = 2.0 * TP / (2 * TP + FP + FN);
    }
    output_data[26][1] = 1.0 * totalTP / (totalTP + totalFP);
    output_data[26][2] = 1.0 * totalTP / (totalTP + totalFN);
    output_data[26][3] = 2.0 * totalTP / (2 * totalTP + totalFP + totalFN);
    output_data[26][0] = output_data[26][1];
  }
  return output_data;
}

void ModelGraph::save_weights(std::string filename) {
  std::ofstream outfile(filename);

  outfile << nodes.size() - 2 << " " << nodes[1].size() - 1 << "\n";

  for (size_t k = 1; k < nodes.size(); ++k) {
    for (size_t i = 0; i < nodes[k].size(); ++i) {
      for (size_t j = 0; j < nodes[k - 1].size(); ++j)
        outfile << nodes[k][i]->weights[j] << " ";
      outfile << "\n";
    }
    outfile << "\n";
  }

  outfile.close();
}

void ModelGraph::load_weights(std::string filename) {
  std::ifstream loaded_file(filename);
  std::string line;
  std::getline(loaded_file, line);
  std::stringstream parameters(line);
  std::string segment;
  std::getline(parameters, segment, ' ');
  int amount_of_hidden_layers = std::stoi(segment);
  std::getline(parameters, segment, ' ');
  int size_of_hidden_layers = std::stoi(segment);
  amount_of_hidden_layers_ = amount_of_hidden_layers;
  size_of_hidden_layers_ = size_of_hidden_layers;
  for (int i = 0; i < amount_of_hidden_layers + 2;
       ++i) {  // Инициализируем входной, выходной и внутренние слои
    std::vector<Node *> next_local_layer;
    if (i == 0) {
      for (int j = 0; j < kInputLayerSize + 1;
           ++j) {  // резервируем первый нейрон под биас с фиксированным
        // значением 1
        Node *local_node = new Node(1, 0);
        next_local_layer.push_back(local_node);
      }
    } else if (i != amount_of_hidden_layers + 1) {  // Внутренние слои
      for (int j = 0; j < size_of_hidden_layers + 1;
           ++j) {  //  резервируем первые нейроны каждого слоя под биасы с
        //  фиксированным значением 1
        Node *local_node = new Node(1, 0);
        next_local_layer.push_back(local_node);
      }
    } else {
      for (int j = 0; j < kOutputLayerSize; ++j) {
        Node *local_node = new Node(1, 0);
        next_local_layer.push_back(local_node);
      }
    }
    nodes.push_back(next_local_layer);
  }
  // Здесь пушбэкаем веса
  std::setlocale(LC_ALL, "en_US.UTF-8");
  for (size_t k = 1; k < nodes.size(); ++k) {
    for (size_t i = 0; i < nodes[k].size(); ++i) {
      std::getline(loaded_file, line);
      std::stringstream local_weights(line);
      for (size_t j = 0; j < nodes[k - 1].size(); ++j) {
        std::getline(local_weights, segment, ' ');
        nodes[k][i]->weights.push_back(std::stod(segment));
      }
    }
    std::getline(loaded_file, line);
  }
}

void ModelGraph::gradient_step(const std::string &line) {
  std::stringstream test(line);
  std::string segment;
  std::getline(test, segment, ',');
  int answer = std::stoi(segment) - 1;

  int input_index = 1;
  while (std::getline(test, segment,
                      ','))  // Заполняем входной слой числами из строки
    nodes[0][input_index++]->value = std::stoi(segment) / 255.0;

  for (int i = 1; i < amount_of_hidden_layers_ + 2;
       ++i) {  // Считаем нейроны, начиная с первого внутреннего слоя
    for (size_t j = i == amount_of_hidden_layers_ + 1 ? 0 : 1;
         j < nodes[i].size();
         ++j) {  // Во всех слоях, кроме последнего, скипаем первый нейрон,
      // потому что это резерв под биас, и его значение залочено
      // на 1. В последнем слое первому нейрону соответствует буква
      // 'A'
      double sum_function = 0;
      for (size_t k = 0; k < nodes[i][j]->weights.size(); ++k)
        sum_function += nodes[i - 1][k]->value * nodes[i][j]->weights[k];
      nodes[i][j]->value = apply_sigmoid(sum_function);
    }
  }  // Теперь у нас есть заполненный вектор nodes, хранящий значения нейронов.
  // Приступаем к поиску error term всех нейронов
  calculate_and_apply_gradient(
      answer);  // Если делать многопоточность, то она будет здесь
}

void ModelGraph::calculate_and_apply_gradient(int answer) {
  // 1-ый шаг, находим error term внешнего слоя
  for (int i = 0; i < kOutputLayerSize; ++i) {
    int local_answer = answer == i ? 1 : 0;
    double value = nodes[amount_of_hidden_layers_ + 1][i]->value;
    nodes[amount_of_hidden_layers_ + 1][i]->error =
        value * (1 - value) * 2 * (value - local_answer);
  }

  // 2-ой шаг, находим error term остальных слоев, справа налево
  for (int k = amount_of_hidden_layers_; k > -1; --k) {
    for (size_t i = 0; i < nodes[k].size(); ++i) {
      double error = 0;
      for (size_t l = k == amount_of_hidden_layers_ ? 0 : 1;
           l < nodes[k + 1].size(); ++l) {
        error += nodes[k + 1][l]->error * nodes[k + 1][l]->weights[i];
      }
      error *= nodes[k][i]->value * (1 - nodes[k][i]->value);
      nodes[k][i]->error = error;
    }
  }

  // 3-ий шаг, меняем веса
  for (int k = 1; k < amount_of_hidden_layers_ + 2; ++k) {
    for (size_t i = 0; i < nodes[k].size(); ++i) {
      for (size_t j = 0; j < nodes[k][0]->weights.size(); ++j) {
        nodes[k][i]->weights[j] -=
            learning_rate_ * nodes[k - 1][j]->value * nodes[k][i]->error;
      }
    }
  }
}

inline double ModelGraph::apply_sigmoid(const double &input) noexcept {
  return 1 / (1 + exp(-input));
}

ModelGraph::~ModelGraph() {
  for (size_t i = 0; i < nodes.size(); ++i) {
    for (size_t j = 0; j < nodes[i].size(); ++j) {
      delete nodes[i][j];
    }
  }
}

}  // namespace s21

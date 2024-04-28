/*******************************************************************************
 * @copyright Copyright [2023] <Morris Roggo, Phylis Cristobal>
 * @brief Реализация многослойного перцептрона CPP7_MLP-1.
 * @file model_matrix.cc
 * @date 2023-09-20
 * @author morrisro, phyliscr
 ********************************************************************************/

#include "model_matrix.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

namespace s21 {

ModelMatrix::ModelMatrix(int input_amount_of_hidden_layers,
                         double input_learning_rate,
                         int input_size_of_hidden_layers)
    : global_counter_(0),
      amount_of_hidden_layers_(input_amount_of_hidden_layers),
      learning_rate_(input_learning_rate),
      size_of_hidden_layers_(input_size_of_hidden_layers) {
  confusion_matrix_.resize(26);
  for (int i = 0; i < 26; ++i) confusion_matrix_[i].resize(4);
  layers_.push_back(S21Matrix(kInputLayerSizeMatrix + 1, 1));
  error_terms_.push_back(S21Matrix(kInputLayerSizeMatrix + 1, 1));
  for (int i = 0; i < amount_of_hidden_layers_; ++i) {
    layers_.push_back(S21Matrix(size_of_hidden_layers_ + 1, 1));
    error_terms_.push_back(S21Matrix(size_of_hidden_layers_ + 1, 1));
    if (i == 0) {
      weights_.push_back(
          S21Matrix(size_of_hidden_layers_ + 1, kInputLayerSizeMatrix + 1));
      gradient_descent_.push_back(
          S21Matrix(size_of_hidden_layers_ + 1, kInputLayerSizeMatrix + 1));
    } else {
      weights_.push_back(
          S21Matrix(size_of_hidden_layers_ + 1, size_of_hidden_layers_ + 1));
      gradient_descent_.push_back(
          S21Matrix(size_of_hidden_layers_ + 1, size_of_hidden_layers_ + 1));
    }
  }
  for (size_t i = 0; i < layers_.size(); ++i) layers_.at(i)(0, 0) = 1;
  layers_.push_back(S21Matrix(kOutputLayerSizeMatrix, 1));
  error_terms_.push_back(S21Matrix(kOutputLayerSizeMatrix, 1));
  weights_.push_back(
      S21Matrix(kOutputLayerSizeMatrix, size_of_hidden_layers_ + 1));
  gradient_descent_.push_back(
      S21Matrix(kOutputLayerSizeMatrix, size_of_hidden_layers_ + 1));

  init_random_weights_and_biases();
}

ModelMatrix::ModelMatrix(const std::string &filename)
    : global_counter_(0),
      amount_of_hidden_layers_(0),
      learning_rate_(0.1),
      size_of_hidden_layers_(0) {
  confusion_matrix_.resize(26);
  for (int i = 0; i < 26; ++i) confusion_matrix_[i].resize(4);
  load_weights(filename);
}

std::vector<double> ModelMatrix::train(int amount_of_epochs,
                                       std::string file_path_train,
                                       std::string file_path_test) {
  std::ifstream train_file(file_path_train);
  std::ifstream test_file(file_path_test);
  std::vector<double> success_rates_by_epochs;
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

    for (int i = 0; i < amount_of_epochs; ++i) {
      int current_line = 0;

      for (int j = 0; j < kOutputLayerSizeMatrix; ++j) {
        for (int k = 0; k < 4; ++k) confusion_matrix_[j][k] = 0;
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
        ++global_counter_;
      }

      while (std::getline(test_file, line)) test(line);

      int totalTP = 0;
      int totalFP = 0;
      for (int j = 0; j < kOutputLayerSizeMatrix; ++j) {
        totalTP += confusion_matrix_[j][0];
        totalFP += confusion_matrix_[j][2];
      }
      success_rates_by_epochs.push_back(100.0 * totalTP / (totalTP + totalFP));
    }
  }
  return success_rates_by_epochs;
}

std::vector<double> ModelMatrix::train_k_fold(int k,
                                              std::string file_path_train) {
  std::ifstream train_file(file_path_train);
  std::vector<double> success_rates_by_epochs;
  if (!train_file) {
    throw std::ifstream::failure("Cannot open specified file");
  } else {
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

      for (int j = 0; j < kOutputLayerSizeMatrix; ++j) {
        for (int k2 = 0; k2 < 4; ++k2) confusion_matrix_[j][k2] = 0;
      }
      train_file.clear();
      train_file.seekg(0);
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
      for (int j = 0; j < kOutputLayerSizeMatrix; ++j) {
        totalTP += confusion_matrix_[j][0];
        totalFP += confusion_matrix_[j][2];
      }
      success_rates_by_epochs.push_back(100.0 * totalTP / (totalTP + totalFP));
// Отдаем в граф текущий рейт обучения в эпохе и тест этой эпохи
#ifndef TESTS
      emit SignalGraph(i + 1, success_rates_by_epochs.back());
#endif
    }
  }
  return success_rates_by_epochs;
}

std::vector<std::vector<double>> ModelMatrix::partial_testing(
    double fraction, std::string file_path_test) {
  std::vector<std::vector<double>> output_data(27);
  std::ifstream test_file(file_path_test);

  if (!test_file) {
    throw std::ifstream::failure("Cannot open specified file");
  } else {
    for (int i = 0; i < 27; ++i) output_data[i].resize(4);
    for (int i = 0; i < kOutputLayerSizeMatrix; ++i) {
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
    for (int i = 0; i < kOutputLayerSizeMatrix; ++i) {
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

int ModelMatrix::test(const int input_layer[kInputLayerSizeMatrix]) {
  for (int i = 0; i < kInputLayerSizeMatrix; ++i)
    layers_.at(0)(i, 0) = input_layer[i] / 255.0;
  for (size_t i = 1; i < layers_.size(); ++i) {
    layers_.at(i) = weights_.at(i - 1) * layers_.at(i - 1);
    apply_sigmoid(layers_.at(i));
  }
  int predicted_answer = 0;
  double probability = layers_.at(layers_.size() - 1)(0, 0);
  for (int i = 1; i < kOutputLayerSizeMatrix; ++i) {
    if (layers_.at(layers_.size() - 1)(i, 0) > probability) {
      probability = layers_.at(layers_.size() - 1)(i, 0);
      predicted_answer = i;
    }
  }
  return predicted_answer + 1;
}

void ModelMatrix::test(const std::string &line) {
  std::stringstream test(line);
  std::string segment;
  std::getline(test, segment, ',');
  int answer = std::stoi(segment) - 1;
  int counter = 0;
  while (std::getline(test, segment, ',')) {
    layers_.at(0)(counter++, 0) = std::stoi(segment) / 255.0;
  }
  for (size_t i = 1; i < layers_.size(); ++i) {
    layers_.at(i) = weights_.at(i - 1) * layers_.at(i - 1);
    apply_sigmoid(layers_.at(i));
  }
  int predicted_answer = 0;
  double probability = layers_.at(layers_.size() - 1)(0, 0);
  for (int i = 1; i < kOutputLayerSizeMatrix; ++i) {
    if (layers_.at(layers_.size() - 1)(i, 0) > probability) {
      probability = layers_.at(layers_.size() - 1)(i, 0);
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

void ModelMatrix::save_weights(std::string filename) {
  std::ofstream outfile(filename);

  outfile << layers_.size() - 2 << " " << layers_[1].GetRows() - 1 << "\n";

  for (size_t k = 0; k < weights_.size(); ++k) {
    for (int i = 0; i < weights_[k].GetRows(); ++i) {
      for (int j = 0; j < weights_[k].GetColumns(); ++j)
        outfile << weights_[k](i, j) << " ";
      outfile << "\n";
    }
    outfile << "\n";
  }

  outfile.close();
}

void ModelMatrix::load_weights(std::string filename) {
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

  layers_ = std::vector<S21Matrix>(0);
  error_terms_ = std::vector<S21Matrix>(0);
  weights_ = std::vector<S21Matrix>(0);
  gradient_descent_ = std::vector<S21Matrix>(0);

  layers_.push_back(S21Matrix(kInputLayerSizeMatrix + 1, 1));
  error_terms_.push_back(S21Matrix(kInputLayerSizeMatrix + 1, 1));
  for (int i = 0; i < amount_of_hidden_layers; ++i) {
    layers_.push_back(S21Matrix(size_of_hidden_layers + 1, 1));
    error_terms_.push_back(S21Matrix(size_of_hidden_layers + 1, 1));
    if (i == 0) {
      weights_.push_back(
          S21Matrix(size_of_hidden_layers + 1, kInputLayerSizeMatrix + 1));
      gradient_descent_.push_back(
          S21Matrix(size_of_hidden_layers + 1, kInputLayerSizeMatrix + 1));
    } else {
      weights_.push_back(
          S21Matrix(size_of_hidden_layers + 1, size_of_hidden_layers + 1));
      gradient_descent_.push_back(
          S21Matrix(size_of_hidden_layers + 1, size_of_hidden_layers + 1));
    }
  }
  for (size_t i = 0; i < layers_.size(); ++i) layers_.at(i)(0, 0) = 1;
  layers_.push_back(S21Matrix(kOutputLayerSizeMatrix, 1));
  error_terms_.push_back(S21Matrix(kOutputLayerSizeMatrix, 1));
  weights_.push_back(
      S21Matrix(kOutputLayerSizeMatrix, size_of_hidden_layers + 1));
  gradient_descent_.push_back(
      S21Matrix(kOutputLayerSizeMatrix, size_of_hidden_layers + 1));

  // Здесь пушбэкаем веса
  std::setlocale(LC_ALL, "en_US.UTF-8");
  for (size_t k = 0; k < weights_.size(); ++k) {
    for (int i = 0; i < weights_[k].GetRows(); ++i) {
      std::getline(loaded_file, line);
      std::stringstream local_weights(line);
      for (int j = 0; j < weights_[k].GetColumns(); ++j) {
        std::getline(local_weights, segment, ' ');
        weights_[k](i, j) = std::stod(segment);
      }
    }
    std::getline(loaded_file, line);
  }
}

void ModelMatrix::gradient_step(const std::string &line) {
  std::stringstream test(line);
  std::string segment;
  std::getline(test, segment, ',');
  int answer = std::stoi(segment) - 1;
  int counter = 0;
  while (std::getline(test, segment, ',')) {
    layers_.at(0)(counter++, 0) = std::stoi(segment) / 255.0;
  }
  for (size_t i = 1; i < layers_.size(); ++i) {
    layers_.at(i) = weights_.at(i - 1) * layers_.at(i - 1);
    apply_sigmoid(layers_.at(i));
  }
  apply_gradient_descent(answer);
}

void ModelMatrix::apply_gradient_descent(int answer) {
  // 1-ый шаг, находим error term внешнего слоя
  for (int i = 0; i < kOutputLayerSizeMatrix; ++i) {
    int local_answer = answer == i ? 1 : 0;
    double o_i_k = layers_.at(layers_.size() - 1)(i, 0);
    error_terms_.at(error_terms_.size() - 1)(i, 0) =
        2 * (o_i_k - local_answer) * o_i_k * (1 - o_i_k);
  }
  // 2-ой шаг, считаем error term остальных слоев
  for (int k = error_terms_.size() - 2; k != -1; --k) {
    for (int i = 0; i < error_terms_.at(k).GetRows(); ++i) {
      double local_sum = 0;
      for (int l = 1; l < error_terms_.at(k + 1).GetRows(); ++l)
        local_sum += error_terms_.at(k + 1)(l, 0) * weights_.at(k)(l, i);
      error_terms_.at(k)(i, 0) =
          local_sum * layers_.at(k)(i, 0) * (1 - layers_.at(k)(i, 0));
    }
  }
  // 3-ий шаг, находим градиенты для всех весов
  for (size_t k = 0; k < gradient_descent_.size(); ++k) {
    for (int i = 0; i < gradient_descent_.at(k).GetRows(); ++i) {
      for (int j = 0; j < gradient_descent_.at(k).GetColumns(); ++j) {
        gradient_descent_.at(k)(i, j) =
            learning_rate_ * layers_.at(k)(j, 0) * error_terms_.at(k + 1)(i, 0);
      }
    }
  }
  // 4-ый шаг, применяем градиентный спуск к весам
  for (size_t k = 0; k < weights_.size(); ++k)
    weights_.at(k) -= gradient_descent_.at(k);
}

void ModelMatrix::apply_sigmoid(S21Matrix &input_matrix) {
  for (int i = 0; i < input_matrix.GetRows(); ++i) {
    for (int j = 0; j < input_matrix.GetColumns(); ++j) {
      input_matrix(i, j) = 1 / (1 + exp(-input_matrix(i, j)));
    }
  }
}

void ModelMatrix::init_random_weights_and_biases() {
  double lower_bound = -0.5;
  double upper_bound = 0.5;
  std::uniform_real_distribution<double> unif(lower_bound, upper_bound);
  std::default_random_engine re;
  for (size_t i = 0; i < weights_.size(); ++i) {
    for (int j = 0; j < weights_.at(i).GetRows(); ++j) {
      for (int k = 0; k < weights_.at(i).GetColumns(); ++k) {
        weights_.at(i)(j, k) = unif(re);
      }
    }
  }
}

}  // namespace s21

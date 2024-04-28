// Copyright [2023] <Phylisis Crystobal>

#include "s21_matrix_oop.h"

#include <math.h>

#include <iostream>

namespace s21 {

// ********************************************************
// ******************* CONSTRUCTORS ***********************
// ********************************************************
S21Matrix::S21Matrix(int rows, int columns) : rows_(rows), columns_(columns) {
  matrix_ = new double *[rows_];
  for (int m = 0; m < rows_; ++m) {
    matrix_[m] = new double[columns_]();
    for (int n = 0; n < columns_; ++n) {
      matrix_[m][n] = 0;
    }
  }
}

S21Matrix::S21Matrix(const S21Matrix &other)
    : rows_(other.rows_), columns_(other.columns_) {
  if (other.matrix_) {
    matrix_ = new double *[rows_];
    for (int m = 0; m < rows_; ++m) {
      matrix_[m] = new double[columns_]();
      for (int n = 0; n < columns_; ++n) {
        matrix_[m][n] = other.matrix_[m][n];
      }
    }
  } else {
    matrix_ = nullptr;
    rows_ = 0;
    columns_ = 0;
  }
}

// ********************************************************
// ********************* OVERLOAD *************************
// ********************************************************

S21Matrix &S21Matrix::operator=(const S21Matrix &other) {
  if (&other != this) {
    this->~S21Matrix();
    S21Matrix result(other);
    rows_ = other.rows_;
    columns_ = other.columns_;
    matrix_ = result.matrix_;
    result.matrix_ = nullptr;
  }
  return *this;
}

double &S21Matrix::operator()(int row, int column) {
  if (row >= rows_ || column >= columns_ || row < 0 || column < 0)
    throw std::out_of_range(ErrorIndexOutOfRange(std::string(__FUNCTION__)));

  return matrix_[row][column];
}

double &S21Matrix::operator()(int row, int column) const {
  if (row >= rows_ || column >= columns_ || row < 0 || column < 0)
    throw std::out_of_range(ErrorIndexOutOfRange(std::string(__FUNCTION__)));

  return matrix_[row][column];
}

S21Matrix S21Matrix::operator+(const S21Matrix &other) const {
  if (rows_ != other.rows_ || columns_ != other.columns_)
    throw std::out_of_range(ErrorSameSize(std::string(__FUNCTION__)));

  S21Matrix result(other.rows_, other.columns_);
  for (int m = 0; m < rows_; m++) {
    for (int n = 0; n < columns_; n++) {
      result.matrix_[m][n] = matrix_[m][n] + other.matrix_[m][n];
    }
  }

  return result;
}

S21Matrix S21Matrix::operator-(const S21Matrix &other) const {
  if (rows_ != other.rows_ || columns_ != other.columns_)
    throw std::out_of_range(ErrorSameSize(std::string(__FUNCTION__)));

  S21Matrix result(other.rows_, other.columns_);
  for (int m = 0; m < rows_; m++) {
    for (int n = 0; n < columns_; n++) {
      result.matrix_[m][n] = matrix_[m][n] - other.matrix_[m][n];
    }
  }

  return result;
}

S21Matrix S21Matrix::operator*(const S21Matrix &other) const {
  if (columns_ != other.rows_)
    throw std::out_of_range(ErrorMultiply(std::string(__FUNCTION__)));

  S21Matrix result(rows_, other.columns_);
  for (int i = 0; i < rows_; i++) {
    for (int j = 0; j < other.columns_; j++) {
      for (int k = 0; k < columns_; k++)
        result.matrix_[i][j] += matrix_[i][k] * other.matrix_[k][j];
    }
  }

  return result;
}

// ********************************************************
// ******************** DESTRUCTOR ************************
// ********************************************************
S21Matrix::~S21Matrix() {
  if (matrix_ != nullptr) {
    for (int m = 0; m < rows_; ++m) {
      delete[] matrix_[m];
    }
    delete[] matrix_;
  }
}

// ********************************************************
// **************** GETTERS / SETTERS *********************
// ********************************************************
void S21Matrix::SetRows(int rows) {
  if (rows < 0)
    throw std::out_of_range(ErrorIndexOutOfRange(std::string(__FUNCTION__)));

  double **matrix_tmp = new double *[rows];
  for (int m = 0; m < rows; ++m) {
    matrix_tmp[m] = new double[columns_]();
    for (int n = 0; n < columns_; ++n) {
      if (m <= rows_ - 1)
        matrix_tmp[m][n] = matrix_[m][n];
      else
        matrix_tmp[m][n] = 0;
    }
  }
  this->~S21Matrix();
  matrix_ = matrix_tmp;
  rows_ = rows;
}

void S21Matrix::SetColumns(int columns) {
  if (columns < 0)
    throw std::out_of_range(ErrorIndexOutOfRange(std::string(__FUNCTION__)));

  double **matrix_tmp = new double *[rows_];
  for (int m = 0; m < rows_; ++m) {
    matrix_tmp[m] = new double[columns]();
    for (int n = 0; n < columns; ++n) {
      if (n <= columns_ - 1)
        matrix_tmp[m][n] = matrix_[m][n];
      else
        matrix_tmp[m][n] = 0;
    }
  }
  this->~S21Matrix();
  matrix_ = matrix_tmp;
  columns_ = columns;
}

// ********************************************************
// ******************** CALCULATION ***********************
// ********************************************************

// Сравнение двух матриц
bool S21Matrix::EqMatrix(const S21Matrix &other) const noexcept {
  int return_result = true;
  if (rows_ == other.rows_ && columns_ == other.columns_) {
    for (int m = 0; m < rows_ && return_result; m++) {
      for (int n = 0; n < columns_ && return_result; n++) {
        if (round(matrix_[m][n] * pow(10, 7)) !=
            round(other.matrix_[m][n] * pow(10, 7))) {
          return_result = false;
        }
      }
    }
  } else {
    return_result = false;
  }
  return return_result;
}

// Суммой двух матриц A = m × n и B = m × n одинаковых размеров называется
// матрица C = m × n = A + B тех же размеров, элементы которой определяются
// равенствами C(i,j) = A(i,j) + B(i,j).
void S21Matrix::SumMatrix(const S21Matrix &other) {
  if (rows_ != other.rows_ || columns_ != other.columns_)
    throw std::out_of_range(ErrorSameSize(std::string(__FUNCTION__)));

  for (int m = 0; m < rows_; m++) {
    for (int n = 0; n < columns_; n++) {
      matrix_[m][n] += other.matrix_[m][n];
    }
  }
}

// Разностью двух матриц A = m × n и B = m × n одинаковых размеров называется
// матрица C = m × n = A - B тех же размеров, элементы которой определяются
// равенствами C(i,j) = A(i,j) - B(i,j).
void S21Matrix::SubMatrix(const S21Matrix &other) {
  if (rows_ != other.rows_ || columns_ != other.columns_)
    throw std::out_of_range(ErrorSameSize(std::string(__FUNCTION__)));

  for (int m = 0; m < rows_; m++) {
    for (int n = 0; n < columns_; n++) {
      matrix_[m][n] -= other.matrix_[m][n];
    }
  }
}

// Произведением матрицы A = m × n на число λ называется матрица B = m × n = λ ×
// A, элементы которой определяются равенствами B = λ × A(i,j).
void S21Matrix::MulNumber(const double number) noexcept {
  for (int m = 0; m < rows_; m++) {
    for (int n = 0; n < columns_; n++) {
      matrix_[m][n] = number * matrix_[m][n];
    }
  }
}

// Произведением матрицы A = m × k на матрицу B = k × n называется матрица C = m
// × n = A × B размера m × n, элементы которой определяются равенством C(i,j) =
// A(i,1) × B(1,j) + A(i,2) × B(2,j) + … + A(i,k) × B(k,j).
void S21Matrix::MulMatrix(const S21Matrix &other) {
  if (columns_ != other.rows_)
    throw std::out_of_range(ErrorMultiply(std::string(__FUNCTION__)));

  S21Matrix result(rows_, other.columns_);
  for (int m = 0; m < rows_; m++) {
    for (int n = 0; n < other.columns_; n++) {
      result.matrix_[m][n] = 0.0;
      for (int k = 0; k < columns_; k++) {
        result.matrix_[m][n] += matrix_[m][k] * other.matrix_[k][n];
      }
    }
  }
  this->~S21Matrix();
  matrix_ = result.matrix_;
  result.matrix_ = nullptr;
  rows_ = result.rows_;
  columns_ = result.columns_;
}

// Транспонирование матрицы А заключается в замене строк этой матрицы ее
// столбцами с сохранением их номеров.
S21Matrix S21Matrix::Transpose() {
  S21Matrix result(columns_, rows_);
  for (int i = 0; i < rows_; i++) {
    for (int j = 0; j < columns_; j++) {
      result.matrix_[j][i] = matrix_[i][j];
    }
  }
  return result;
}

// Минором M(i,j) называется определитель (n-1)-го порядка, полученный
// вычёркиванием из матрицы A i-й строки и j-го столбца.
S21Matrix S21Matrix::Minor(int m, int n) const {
  int skip_i = 0;
  int skip_j = 0;
  S21Matrix result(rows_ - 1, columns_ - 1);
  for (int i = 0; i < result.rows_; i++) {
    if (i == m) skip_i = 1;
    for (int j = 0; j < result.columns_; j++) {
      if (j == n) skip_j = 1;
      result(i, j) = matrix_[i + skip_i][j + skip_j];
    }
    skip_j = 0;
  }
  return result;
}

// Определитель (детерминант) - это число, которое ставят в соответствие каждой
// квадратной матрице и вычисляют из элементов по специальным формулам. Tip:
// определитель может быть вычислен только для квадратной матрицы. Определитель
// матрицы равен сумме произведений элементов строки (столбца) на
// соответствующие алгебраические дополнения.
double S21Matrix::Determinant() const {
  if (rows_ != columns_)
    throw std::out_of_range(ErrorSquareOnly(std::string(__FUNCTION__)));

  double result = 0;
  if (rows_ == 1) {
    result = matrix_[0][0];
  } else {
    for (int i = 0; i < rows_; i++) {
      S21Matrix buff;
      result += pow(-1, i) * matrix_[0][i] * Minor(0, i).Determinant();
    }
  }
  return result;
}

// Алгебраическим дополнением элемента матрицы является значение минора
// умноженное на -1^(i+j).
S21Matrix S21Matrix::CalcComplements() const {
  S21Matrix result(rows_, columns_);
  if (rows_ != columns_)
    throw std::out_of_range(ErrorSquareOnly(std::string(__FUNCTION__)));
  for (int i = 0; i < rows_; i++) {
    for (int j = 0; j < columns_; j++) {
      result(i, j) = pow(-1, i + j) * Minor(i, j).Determinant();
    }
  }
  return result;
}

// Матрицу A в степени -1 называют обратной к квадратной матрице А, если
// произведение этих матриц равняется единичной матрице.
S21Matrix S21Matrix::InverseMatrix() const {
  double determinant = Determinant();
  if (abs(determinant) < 1e-06)
    throw std::out_of_range(
        "If the determinant of a matrix is equal to 0, the inverse matrix does "
        "not exist.");
  S21Matrix result(*this);
  result = result.Transpose().CalcComplements();
  if (rows_ == 1 && columns_ == 1) {
    result.matrix_[0][0] = 1 / matrix_[0][0];
  } else {
    result.MulNumber(1 / Determinant());
  }
  return result;
}
}  // namespace s21

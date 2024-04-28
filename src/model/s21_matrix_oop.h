// Copyright [2023] <Phylis Crystobal>
#ifndef CPP7_MLP_MODEL_S21_MATRIX_OOP_H
#define CPP7_MLP_MODEL_S21_MATRIX_OOP_H

#include <string>

namespace s21 {

class S21Matrix {
 public:
  // Constructors
  inline S21Matrix() : rows_(0), columns_(0), matrix_(nullptr) {}

  S21Matrix(int rows, int columns);

  S21Matrix(const S21Matrix &other);

  inline S21Matrix(S21Matrix &&other) noexcept
      : rows_(other.rows_), columns_(other.columns_), matrix_(other.matrix_) {
    other.matrix_ = nullptr;
    other.rows_ = 0;
    other.columns_ = 0;
  }

  // Overloads
  S21Matrix &operator=(const S21Matrix &other);
  inline bool operator==(const S21Matrix &other) const noexcept {
    return S21Matrix::EqMatrix(other);
  }

  inline bool operator!=(const S21Matrix &other) const noexcept {
    return !(EqMatrix(other));
  }

  double &operator()(int row, int column);

  double &operator()(int row, int column) const;

  S21Matrix operator+(const S21Matrix &other) const;

  inline S21Matrix &operator+=(const S21Matrix &other) {
    this->SumMatrix(other);
    return *this;
  }

  S21Matrix operator-(const S21Matrix &other) const;
  inline S21Matrix &operator-=(const S21Matrix &other) {
    this->SubMatrix(other);
    return *this;
  }

  S21Matrix operator*(const S21Matrix &other) const;
  inline S21Matrix operator*(const double number) const noexcept {
    S21Matrix result(*this);
    result.MulNumber(number);
    return result;
  }

  inline S21Matrix &operator*=(const S21Matrix &other) {
    MulMatrix(other);
    return *this;
  }

  inline S21Matrix operator*=(const double number) {
    MulNumber(static_cast<double>(number));
    return *this;
  }

  inline friend S21Matrix operator*(const double number,
                                    const S21Matrix &matrix) {
    return matrix * number;
  }

  // Destructor
  ~S21Matrix() noexcept;

  // Getters and setters (Accessors and mutators)
  inline int GetRows() const noexcept { return rows_; }
  inline int GetColumns() const noexcept { return columns_; }
  void SetRows(int rows);
  void SetColumns(int columns);

  // Calculation
  bool EqMatrix(const S21Matrix &other) const noexcept;
  void SumMatrix(const S21Matrix &other);
  void SubMatrix(const S21Matrix &other);
  void MulNumber(const double number) noexcept;
  void MulMatrix(const S21Matrix &other);
  S21Matrix Transpose();
  S21Matrix Minor(int m, int n) const;
  S21Matrix CalcComplements() const;
  double Determinant() const;
  S21Matrix InverseMatrix() const;

  // Helpers
  inline std::string ErrorSameSize(const std::string &foo_name) const {
    return "The matrices must be of the same size. [" + foo_name + "]";
  }

  inline std::string ErrorIndexOutOfRange(const std::string &foo_name) const {
    return "Incorrect input. Index is out of range. [" + foo_name + "]";
  }

  inline std::string ErrorMultiply(const std::string &foo_name) const {
    return "The number of columns of the first matrix is not equal to the "
           "number "
           "of rows of the second matrix. [" +
           foo_name + "]";
  }

  inline std::string ErrorSquareOnly(const std::string &foo_name) const {
    return "The determinant can be calculated only for a square matrix. [" +
           foo_name + "]";
  }

 private:
  int rows_;
  int columns_;
  double **matrix_;
};
}  // namespace s21

#endif  // CPP7_MLP_MODEL_S21_MATRIX_OOP_H

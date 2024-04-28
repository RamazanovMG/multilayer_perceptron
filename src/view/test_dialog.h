#ifndef CPP7_MLP_1_VIEW_TEST_DIALOG_H_
#define CPP7_MLP_1_VIEW_TEST_DIALOG_H_

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>

namespace s21 {

class TestDialog : public QDialog {
  Q_OBJECT

public:
  explicit TestDialog(QWidget *parent = nullptr);
  ~TestDialog();
  int getSliderValue() const;

private:
  QSlider *slider;
  QLabel *label;
  QPushButton *okButton;
  QVBoxLayout *layout;

private slots:
  void updateLabel(int value);
};

} // namespace s21

#endif // CPP7_MLP_1_VIEW_TEST_DIALOG_H_

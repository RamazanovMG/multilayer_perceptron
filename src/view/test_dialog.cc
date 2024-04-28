#include "test_dialog.h"

namespace s21 {

TestDialog::TestDialog(QWidget *parent) : QDialog(parent) {
  setWindowTitle("Test settings");

  label = new QLabel();
  label->setText("");

  // Создаем слайдер
  slider = new QSlider(Qt::Horizontal);
  slider->setMinimum(0);
  slider->setMaximum(100);

  // Создаем кнопку "OK"
  okButton = new QPushButton("OK");

  // Создаем компоновщик для размещения слайдера и кнопки
  layout = new QVBoxLayout;
  layout->addWidget(label);
  layout->addWidget(slider);
  layout->addWidget(okButton);

  // Устанавливаем компоновщик в диалог
  setLayout(layout);

  // Подключаем слот для обработки события нажатия на кнопку "OK"
  connect(okButton, &QPushButton::clicked, this, &TestDialog::accept);

  // Подключаем слот к сигналу изменения значения слайдера
  connect(slider, &QSlider::valueChanged, this, &TestDialog::updateLabel);

  slider->setValue(100);
  label->setText(QString::number(slider->value()) + "%");
}

TestDialog::~TestDialog() {
  delete label;
  delete slider;
  delete okButton;
  delete layout;
}

int TestDialog::getSliderValue() const { return slider->value(); }

void TestDialog::updateLabel(int value) {
  label->setText(QString::number(slider->value()) + "%");
}

} // namespace s21

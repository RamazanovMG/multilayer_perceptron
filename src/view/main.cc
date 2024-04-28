/*******************************************************************************
 * @copyright Copyright [2023] <Phylis Cristobal, Morris Roggo>
 * @brief Реализация многослойного перцептрона CPP7_MLP-1.
 * @file main.cc
 * @date 2023-09-19
 * @author phyliscr, morrisro
 ********************************************************************************/
#include "mainwindow.h"

#include <QApplication>
#include <cstdlib>
#include <ctime>

int main(int argc, char *argv[]) {
  unsigned int seed = static_cast<unsigned int>(std::time(nullptr));
  std::srand(seed);

  QApplication a(argc, argv);

  // Устанавливаем тему
  a.setStyle(QStyleFactory::create("Fusion"));

  // Устанавливаем палитру для темного режима
  QPalette darkPalette;
  darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
  darkPalette.setColor(QPalette::WindowText, Qt::white);
  darkPalette.setColor(QPalette::Base, QColor(45, 45, 45));
  darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
  darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
  darkPalette.setColor(QPalette::ToolTipText, Qt::white);
  darkPalette.setColor(QPalette::Text, Qt::white);
  darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
  darkPalette.setColor(QPalette::ButtonText, Qt::white);
  darkPalette.setColor(QPalette::BrightText, Qt::red);
  darkPalette.setColor(QPalette::Link, QColor(238, 32, 77));
  darkPalette.setColor(QPalette::Highlight, QColor(238, 32, 77));
  darkPalette.setColor(QPalette::HighlightedText, Qt::black);

  a.setPalette(darkPalette);

  s21::MainWindow w;

  if (std::rand() % 2)
    w.setWindowTitle("MLP (written by PhylisCr, MorrisRo)");
  else
    w.setWindowTitle("MLP (written by MorrisRo, PhylisCr)");

  w.setWindowIcon(QIcon(":/icons/icon.png"));

  w.show();

  return a.exec();
}

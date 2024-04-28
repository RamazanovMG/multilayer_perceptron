/*******************************************************************************
 * @copyright Copyright [2023] <Phylis Cristobal, Morris Roggo>
 * @brief Реализация многослойного перцептрона CPP7_MLP-1.
 * @file mainwindow.h
 * @date 2023-09-19
 * @author phyliscr, morrisro
 ********************************************************************************/

#ifndef CPP7_MLP_1_VIEW_MAINWINDOW_H_
#define CPP7_MLP_1_VIEW_MAINWINDOW_H_

#include "../controller/controller.h"
#include "../model/model_graph.h"
#include "../model/model_matrix.h"
#include "../model/s21_matrix_oop.h"
#include "painter.h"
#include "qcustomplot_pch.h"
#include "test_dialog.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QMainWindow>

namespace s21 {

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:
  void MainWindowStyle();
  void MakeNewScene();
  void ProgressBarStyle();
  void ProgressBarInitialising();
  void QCustomPlotStyle();
  void QCustomPlotInitialising();
  //  void QCustomPlotDraw(const QVector<double> x, const QVector<double> y);
  void QCustomPlotDraw(const double, const double);
  void ImageToArray();
  QImage CropImage(const QImage &originalImage);
  QImage ResizeImage(const QImage &original_image, int target_width,
                     int target_height);
  void ClearArray();
  void DebugPrint(QImage &image);

private slots:
  void on_actionOpenImage_triggered();
  void on_actionSaveImage_triggered();
  void on_actionClearImage_triggered();
  void on_actionOpenTTFiles_triggered();
  void on_actionExit_triggered();
  void on_actionAntialiasing_triggered(bool checked);
  void on_slideLearningRate_actionTriggered(int action);
  void on_sbCVE_valueChanged(int arg1);
  void on_cbCVE_clicked(bool checked);
  void on_pbTrain_clicked();
  void on_pbTest_clicked();
  void on_actionOpen_Weights_triggered();
  void on_actionSave_Weights_triggered();

private:
  Ui::MainWindow *ui_;
  Painter *scene_;
  QCustomPlot *custom_plot_;
  int *grayValues_ = new int[784];
  QFile file_train_;
  QFile file_test_;
  Controller *controller_;
  double learning_rate_;
  const char alphabet_[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
                              'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
                              'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
  double graph_min_;
  double graph_max_;

public slots:
  void SlotDraw();
  void SlotProgress(int);
  void SlotGraph(int, double);
};

} // namespace s21

#endif // CPP7_MLP_1_VIEW_MAINWINDOW_H_

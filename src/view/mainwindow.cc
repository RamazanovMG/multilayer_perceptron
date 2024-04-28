/*******************************************************************************
 * @copyright Copyright [2023] <Phylis Cristobal, Morris Roggo>
 * @brief Реализация многослойного перцептрона CPP7_MLP-1.
 * @file mainwindow.cc
 * @date 2023-09-19
 * @author phyliscr, morrisro
 ********************************************************************************/

#include "mainwindow.h"

namespace s21 {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui_(new Ui::MainWindow), learning_rate_(0.1),
      controller_(nullptr), graph_min_(0.0), graph_max_(100.0) {
  ui_->setupUi(this);
  custom_plot_ = new QCustomPlot;
  QVBoxLayout *layout = new QVBoxLayout(ui_->widgetPlot);

  MakeNewScene();

  this->setFixedSize(800, 563);
  MainWindowStyle();

  ui_->graphicsView->setSceneRect(0, 0, 510, 510);
  ui_->graphicsView->setFixedSize(512, 512);
  ui_->graphicsView->setRenderHint(QPainter::Antialiasing, true);

  layout->addWidget(custom_plot_);
  QCustomPlotStyle();
  QCustomPlotInitialising();

  ProgressBarStyle();
  ProgressBarInitialising();

  ui_->lblLearninfRate->setText("Learning rate: " +
                                QString::number(learning_rate_));

  file_train_.setFileName("emnist-letters-train.csv");
  file_test_.setFileName("emnist-letters-test.csv");
}

MainWindow::~MainWindow() {
  delete[] grayValues_;
  delete custom_plot_;
  delete scene_;
  delete ui_;
  delete controller_;
}

void MainWindow::MainWindowStyle() {
  QString style_button =
      "QPushButton {"
      "   background-color: " +
      QColor(palette().color(QPalette::Highlight)).name() +
      ";"
      "   border: none;"
      "   padding: 5px 0px;"
      "   border-radius: 5px;"
      "}"
      "QPushButton:!enabled {"
      "   background-color: " +
      QColor(palette().color(QPalette::Highlight)).darker().name() +
      ";"
      "}"
      "QPushButton:hover {"
      "   background-color: " +
      QColor(palette().color(QPalette::Highlight)).darker().name() +
      ";"
      "}"
      "QPushButton:pressed {"
      "   background-color: " +
      QColor(palette().color(QPalette::Highlight)).lighter().name() +
      ";"
      "}";
  ui_->pbTrain->setStyleSheet(style_button);
  ui_->pbTest->setStyleSheet(style_button);
  ui_->lblSymbol->setStyleSheet(
      "QLabel { border: 3px solid " +
      QColor(palette().color(QPalette::Highlight)).name() +
      ";"
      "   background-color: " +
      QColor(palette().color(QPalette::Base)).name() +
      ";"
      "}");
}

void MainWindow::MakeNewScene() {
  scene_ = new Painter;
  connect(scene_, &Painter::SignalDraw, this, &MainWindow::SlotDraw);
  ui_->graphicsView->setScene(scene_);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//              Progress bar              //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void MainWindow::ProgressBarStyle() {}

void MainWindow::ProgressBarInitialising() {
  ui_->progressBar->setVisible(true);
  ui_->progressBar->setValue(0);
  ui_->progressBarEpoch->setVisible(true);
  ui_->progressBarEpoch->setValue(0);
  ui_->progressBarEpoch->setFixedHeight(7);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//              QCustomPlot               //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void MainWindow::QCustomPlotStyle() {
  custom_plot_->xAxis->setSubTicks(false);
  custom_plot_->yAxis->setSubTicks(false);
  // Проверяем, включен ли темный режим
  if (palette().color(QPalette::Window).lightness() < 128) {
    QColor current_color = palette().color(QPalette::Mid);
    custom_plot_->setBackground(QBrush(palette().color(QPalette::Window)));
    custom_plot_->xAxis->setBasePen(QPen(current_color, 1));
    custom_plot_->yAxis->setBasePen(QPen(current_color, 1));
    custom_plot_->xAxis->setTickPen(QPen(current_color, 1));
    custom_plot_->yAxis->setTickPen(QPen(current_color, 1));
    custom_plot_->xAxis->setTickLabelColor(current_color);
    custom_plot_->yAxis->setTickLabelColor(current_color);
  }
}

void MainWindow::QCustomPlotInitialising() {
  custom_plot_->clearGraphs(); // Очистка графика
  custom_plot_->xAxis->setRange(0, ui_->sbCVE->value() + 1);
  custom_plot_->yAxis->setRange(0, 100);
  custom_plot_->addGraph();
  custom_plot_->graph(0)->setPen(QPen(palette().color(QPalette::Highlight), 2));
  custom_plot_->graph(0)->setLineStyle(QCPGraph::lsLine);
  custom_plot_->graph(0)->setScatterStyle(QCPScatterStyle(
      QCPScatterStyle::ssCircle, QPen(palette().color(QPalette::Highlight)),
      QBrush(palette().color(QPalette::Highlight)), 5));
  custom_plot_->replot();
}

void MainWindow::QCustomPlotDraw(const double x, const double y) {
  if (graph_max_ > y)
    graph_max_ = y;
  if (graph_min_ < y)
    graph_min_ = y;
  double average = (graph_max_ - graph_min_) / 2;
  custom_plot_->yAxis->setRange(graph_min_ - average, graph_max_ + average);
  custom_plot_->graph(0)->addData(x, y);
  custom_plot_->replot();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                Helpers                 //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void MainWindow::ImageToArray() {
  int width = 28;
  int height = 28;
  scene_->setMousePointerVisible(false);
  QImage image(ui_->graphicsView->sceneRect().size().toSize(),
               QImage::Format_Grayscale8);
  // Создаем объект в котором будем хранить растрированную scene_
  QPainter painter(&image);
  // Растрируем в image
  ui_->graphicsView->render(&painter);

  QImage normalized_image;
  normalized_image = CropImage(image);
  int max_side = qMax(normalized_image.width(), normalized_image.height());
  normalized_image = normalized_image.scaled(
      max_side, max_side, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  normalized_image = ResizeImage(normalized_image, width, height);
  normalized_image = normalized_image.scaled(width, height);
  normalized_image.invertPixels();

  // Поворачиваем изображение против часовой стрелки на 90 градусов и
  // отзеркаливаем
  normalized_image =
      normalized_image.transformed(QTransform().rotate(-90).scale(-1, 1));

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      grayValues_[y * width + x] =
          normalized_image.pixelColor(x, y).lightness();
    }
  }

  // DebugPrint(normalized_image);

  int result = -1;
  if (controller_ != nullptr)
    // result = model_graph_->test(grayValues_);
    result = controller_->test(grayValues_);

  if (result != -1)
    ui_->lblSymbol->setText(QString(alphabet_[result - 1]));
}

QImage MainWindow::CropImage(const QImage &original_image) {
  // Находим прямоугольник, ограничивающий непустую область изображения
  QRect boundingRect = QRect();
  boundingRect.setTop(513);
  boundingRect.setBottom(0);
  boundingRect.setLeft(513);
  boundingRect.setRight(0);

  for (int y = 0; y < original_image.height(); ++y) {
    for (int x = 0; x < original_image.width(); ++x) {
      if (!original_image.pixelColor(x, y).lightness()) {
        boundingRect.setTop(qMin(boundingRect.top(), y));
        boundingRect.setBottom(qMax(boundingRect.bottom(), y));
        boundingRect.setLeft(qMin(boundingRect.left(), x));
        boundingRect.setRight(qMax(boundingRect.right(), x));
      }
    }
  }
  // Обрезаем изображение с использованием найденного прямоугольника
  return original_image.copy(boundingRect);
}

// Функция для изменения размера изображения и добавления пикселей
QImage MainWindow::ResizeImage(const QImage &original_image, int target_width,
                               int target_height) {
  QImage resized_image(target_width, target_height, QImage::Format_Grayscale8);
  resized_image.fill(Qt::white);
  QPainter painter(&resized_image);

  QImage scale_image = original_image.scaled(20, 20, Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation);
  painter.drawImage((target_width - scale_image.width()) / 2,
                    (target_height - scale_image.height()) / 2, scale_image);

  return resized_image;
}

void MainWindow::ClearArray() {
  for (int i = 0; i <= 783; ++i) {
    grayValues_[i] = 0;
  }
  //  ImageToArray();
}

void MainWindow::DebugPrint(QImage &image) {
  int width = 28;
  int height = 28;
  QDebug deb = qDebug().nospace();
  for (int y = 0; y < height; ++y) {
    deb << "\n";
    for (int x = 0; x < width; ++x) {
      QColor pixelColor = image.pixelColor(x, y);
      grayValues_[y * width + x] = pixelColor.lightness();
      deb << grayValues_[y * width + x] << " "
          << (grayValues_[y * width + x] <= 99 ? " " : "")
          << (grayValues_[y * width + x] <= 9 ? " " : "");
    }
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                 Slots                  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void MainWindow::SlotDraw() { ImageToArray(); }

void MainWindow::SlotProgress(int progress) {
  ui_->progressBar->setValue(progress);
  ui_->progressBarEpoch->setValue(ui_->progressBarEpoch->value() + 1);
}

void MainWindow::SlotGraph(int epoch, double test_percent) {
  QCustomPlotDraw(epoch, test_percent);
}

void MainWindow::on_cbCVE_clicked(bool checked) {
  if (checked) {
    ui_->lblCVE->setText("K-fold (k)");
    ui_->sbCVE->setMinimum(2);
  } else {
    ui_->lblCVE->setText("Epochs");
    ui_->sbCVE->setMinimum(1);
  }
}

void MainWindow::on_sbCVE_valueChanged(int arg1) {
  custom_plot_->xAxis->setRange(0, arg1 + 1);
  custom_plot_->replot();
}

void MainWindow::on_pbTrain_clicked() {
  QString report = "";
  QElapsedTimer timer;
  timer.start();
  std::vector<double> result;
  if (file_train_.exists() && file_test_.exists()) {
    QCustomPlotInitialising();
    // переключать реализацию перцпетрона (матричная или графовая)
    ui_->progressBarEpoch->setMaximum(100 * ui_->sbCVE->value());
    ui_->progressBar->setValue(0);
    ui_->progressBarEpoch->setValue(0);

    delete controller_;
    controller_ =
        new Controller(!ui_->rbGraph->isChecked(), ui_->sbHiddenLayers->value(),
                       learning_rate_, ui_->sbLayerSize->value());
    connect(controller_, &Controller::SignalProgress, this,
            &MainWindow::SlotProgress);
    connect(controller_, &Controller::SignalGraph, this,
            &MainWindow::SlotGraph);
    if (!ui_->cbCVE->isChecked())
      result = controller_->Train(ui_->sbCVE->value(),
                                  file_train_.fileName().toStdString(),
                                  file_test_.fileName().toStdString());
    else
      result = controller_->Train(ui_->sbCVE->value(),
                                  file_train_.fileName().toStdString());

    qint64 elapsed = timer.elapsed();

    for (int i = 0; i < result.size(); ++i)
      report += "\nEpoch " + QString::number(i) +
                ": Test success rate: " + QString::number(result.at(i)) + "%";
    report +=
        "\n\nElapsed time: " + QString::number(elapsed / 1000.0) + "sec.\n";

    QMessageBox::information(this, "MLP Train",
                             "Training completed.\t\n" + report);
  } else {
    QMessageBox::critical(this, "Error",
                          "For training and testing you need to select the "
                          "EMNIST files in the File menu");
  }
}

void MainWindow::on_pbTest_clicked() {
  QString report = "";
  QElapsedTimer timer;
  timer.start();
  if (controller_ == nullptr) {
    QMessageBox::critical(this, "Error", "Train the model before test.");
  } else {
    if (file_test_.exists()) {
      // Создаем и показываем диалог со слайдером и кнопкой "OK"
      TestDialog *dialog = new TestDialog(this);
      if (dialog->exec() == QDialog::Accepted) {
        // Получение значения слайдера после закрытия диалогового окна
        int sliderValue = dialog->getSliderValue();
        delete dialog;
        std::vector<std::vector<double>> tmp_vector;
        if (controller_ != nullptr) {
          tmp_vector = controller_->partial_testing(
              sliderValue / 100.0, file_test_.fileName().toStdString());
        }

        qint64 elapsed = timer.elapsed();

        for (int i = 0; i < 27; ++i) {
          i < 26 ? report.append(alphabet_[i]) : report.append("Micro F1");
          report += "\t";
          for (int j = 0; j < 4; ++j) {
            if (std::isnan(tmp_vector[i][j]))
              report += "-\t";
            else
              report +=
                  QString::number(round(tmp_vector[i][j] * 1000) / 1000) + "\t";
          }
          report += "\n";
        }

        report +=
            "\nElapsed time: " + QString::number(elapsed / 1000.0) + "sec.\n";
        // Сгенерировать репорт
        QMessageBox::information(
            this, "MLP Test",
            "Тест "
            "завершен.\n\nClass\tAccuracy\tPrecision\tRecall\tf-measure\n" +
                report);
      }
    } else {
      QMessageBox::critical(this, "Error",
                            "For training and testing you need to select the "
                            "EMNIST files in the File menu");
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                 Menu                   //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void MainWindow::on_actionOpenImage_triggered() {
  QString file_path =
      QFileDialog::getOpenFileName(this, "Open files", QDir::currentPath(),
                                   "Images (*.bmp *.png);;Все файлы (*.*)");

  if (!file_path.isEmpty()) {
    statusBar()->showMessage(file_path, 5000);
    QImage loaded_image(file_path);
    int maxSize = 512;
    QImage scaledImage =
        loaded_image.scaledToWidth(maxSize, Qt::SmoothTransformation);
    QImage gray_scale_loaded_image =
        scaledImage.convertToFormat(QImage::Format_Grayscale8);
    QGraphicsPixmapItem *pixel_loaded_image =
        new QGraphicsPixmapItem(QPixmap::fromImage(gray_scale_loaded_image));
    scene_->addItem(pixel_loaded_image);
    ImageToArray();
  }
}

void MainWindow::on_actionSaveImage_triggered() {
  QDateTime currentDateTime = QDateTime::currentDateTime();
  QString defaultDirectory = QDir::currentPath();
  QString defaultFileName =
      "/image_" + currentDateTime.toString("yyyy_MM_dd_hh_mm_ss");
  QString file_path_train = QFileDialog::getExistingDirectory(
      this, "Save image", defaultDirectory, QFileDialog::ShowDirsOnly);

  scene_->setMousePointerVisible(false);
  QImage image(ui_->graphicsView->sceneRect().size().toSize(),
               QImage::Format_Grayscale8);
  QPainter painter(&image);
  ui_->graphicsView->render(&painter);
  QImage resizedImage =
      image.scaled(28, 28, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  // Сохраняем QPixmap в формате BMP
  image.save(file_path_train + "/" + defaultFileName + "_512_512.bmp");
}

void MainWindow::on_actionOpenTTFiles_triggered() {
  QString file_path_train = QFileDialog::getOpenFileName(
      this, "Open EMNIST file for TRAIN", QDir::currentPath());
  file_train_.setFileName(file_path_train);

  QString file_path_test = QFileDialog::getOpenFileName(
      this, "Open EMNIST file for TEST", QDir::currentPath());
  file_test_.setFileName(file_path_test);
}

void MainWindow::on_actionExit_triggered() { QCoreApplication::quit(); }

void MainWindow::on_slideLearningRate_actionTriggered(int action) {
  int value = ui_->slideLearningRate->value();
  learning_rate_ = round(pow(10.0, (value / 50.0 - 3)) * 10000.0) / 10000.0;
  ui_->lblLearninfRate->setText("Learning rate: " +
                                QString::number(learning_rate_));
}

void MainWindow::on_actionOpen_Weights_triggered() {
  QFile file;
  QString file_path = QFileDialog::getOpenFileName(
      this, "Open weights", QDir::currentPath(), "MLP (*.mlp);;Все файлы (*)");
  file.setFileName(file_path);
  if (!file_path.isEmpty()) {
    delete controller_;
    controller_ = new Controller(file.fileName().toStdString());
  }
}

void MainWindow::on_actionSave_Weights_triggered() {
  QFile file;
  QString file_path = QFileDialog::getSaveFileName(
      this, "Save weights. Please specify a file name without an extension.",
      QDir::currentPath(), "MLP (*.mlp);;Все файлы (*)");
  file.setFileName(file_path + ".mlp");
  if (controller_ != nullptr)
    controller_->save_weights(file.fileName().toStdString());
  else
    QMessageBox::warning(this, "Save", "Train the model before save!");
}

void MainWindow::on_actionClearImage_triggered() {
  delete scene_;
  MakeNewScene();
  ClearArray();
  ui_->lblSymbol->setText("");
}

void MainWindow::on_actionAntialiasing_triggered(bool checked) {
  ui_->graphicsView->setRenderHint(QPainter::Antialiasing, checked);
}

} // namespace s21

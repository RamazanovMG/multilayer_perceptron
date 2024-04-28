/*******************************************************************************
 * @copyright Copyright [2023] <Phylis Cristobal, Morris Roggo>
 * @brief Реализация многослойного перцептрона CPP7_MLP-1.
 * @file Painter.h
 * @date 2023-09-19
 * @author phyliscr, morrisro
 ********************************************************************************/

#ifndef CPP7_MLP_1_VIEW_PAINTER_H_
#define CPP7_MLP_1_VIEW_PAINTER_H_

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

namespace s21 {

class Painter : public QGraphicsScene {
  Q_OBJECT
public:
  Painter();
  ~Painter();

  void setMousePointerVisible(bool value);

private:
  QGraphicsEllipseItem *mouse_pointer;
  QPointF previous_point_;
  int thickness_;

  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

signals:
  void SignalDraw();
};

} // namespace s21

#endif // CPP7_MLP_1_VIEW_PAINTER_H_

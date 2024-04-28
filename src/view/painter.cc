/*******************************************************************************
 * @copyright Copyright [2023] <Phylis Cristobal, Morris Roggo>
 * @brief Реализация многослойного перцептрона CPP7_MLP-1.
 * @file Painter.cc
 * @date 2023-09-19
 * @author phyliscr, morrisro
 ********************************************************************************/

#include "painter.h"

namespace s21 {

Painter::Painter() {
  thickness_ = 52;

  setBackgroundBrush(QBrush(Qt::white));

  QPen mouse_pointer_pen;
  mouse_pointer_pen.setColor(Qt::black);
  mouse_pointer_pen.setStyle(Qt::DashLine);
  mouse_pointer_pen.setWidth(1);

  mouse_pointer = new QGraphicsEllipseItem(-thickness_ / 2, -thickness_ / 2,
                                           thickness_, thickness_);
  mouse_pointer->setBrush(Qt::transparent);
  mouse_pointer->setPen(mouse_pointer_pen);
  mouse_pointer->setVisible(false);

  addItem(mouse_pointer);
}

Painter::~Painter() { delete mouse_pointer; }

void Painter::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (event->button() == Qt::MouseButton::LeftButton) {
    addEllipse(event->scenePos().x() - thickness_ / 2,
               event->scenePos().y() - thickness_ / 2, thickness_, thickness_,
               QPen(Qt::NoPen), QBrush(Qt::black));
    previous_point_ = event->scenePos();
  }
}

void Painter::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  if (event->buttons() & Qt::LeftButton) {
    addLine(previous_point_.x(), previous_point_.y(), event->scenePos().x(),
            event->scenePos().y(),
            QPen(Qt::black, thickness_, Qt::SolidLine, Qt::RoundCap));
    previous_point_ = event->scenePos();
  } else {
    mouse_pointer->setVisible(true);
    mouse_pointer->setRect(event->scenePos().x() - thickness_ / 2,
                           event->scenePos().y() - thickness_ / 2, thickness_,
                           thickness_);
  }
}

void Painter::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  if (event->button() == Qt::LeftButton)
    emit SignalDraw();
}

void Painter::setMousePointerVisible(bool value) {
  mouse_pointer->setVisible(value);
}

} // namespace s21

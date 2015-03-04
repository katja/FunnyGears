#include "preferences.h"

QColor Preferences::SelectionColor(QColor(220, 220, 255)); // light blue

QColor Preferences::HoverColor(QColor(240, 210, 80)); // dark yellow

QColor Preferences::AttentionColor(QColor(220, 0, 30)); // red

QColor Preferences::BackgroundGridColor(Qt::lightGray);

QColor Preferences::CoordinateAxisColor(QColor(10, 10, 10, 50)); // dark grey, half invisible

qreal Preferences::SimpleLineWidth(1);

qreal Preferences::HighlightedLineWidth(3);

qreal Preferences::PointRadius(2.0);

qreal Preferences::SmallPointRadius(0.1);

qreal Preferences::TangentLength(80.0);

qreal Preferences::AngularPitchStrokesLength(300.0);

qreal Preferences::ZoomDefaultX(750.0);

qreal Preferences::ZoomDefaultY(750.0);

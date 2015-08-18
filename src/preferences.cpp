#include "preferences.h"

QColor Preferences::SelectionColor(QColor(220, 220, 255)); // light blue    #DCDCFF

QColor Preferences::HoverColor(QColor(240, 210, 80)); // dark yellow        #F0D250

QColor Preferences::AttentionColor(QColor(190, 0, 30)); // red              #BE001E

QColor Preferences::InformationColorA(QColor(0, 128, 0)); //dark green      #008000

QColor Preferences::InformationColorB(QColor(130, 167, 0)); //yellow green  #82A700

QColor Preferences::InformationColorC(QColor(210, 180, 0)); //orange        #D2B800

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

qreal Preferences::DefaultViewSceneXStart(-300.0);
qreal Preferences::DefaultViewSceneYStart(-300.0);
qreal Preferences::DefaultViewSceneXStop(600.0);
qreal Preferences::DefaultViewSceneYStop(600.0);

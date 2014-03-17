#ifndef SPLINE_MODEL
#define SPLINE_MODEL

#include "stable.h"
#include "List.h"
#include "Spline.h"
#include "SceneTreeModel.h"
#include "GeometryModel.h"

class SplineModel : public GeometryModel {

Q_OBJECT

public:
    SplineModel(SceneTreeModel *model, QObject *parent = 0);
    ~SplineModel();

    enum Properties {
        DEGREE, TORN_TO_EDGES
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool addSpline(Spline *spline);
    bool removeSpline(Spline *spline);

    bool operatesOn(QGraphicsItem *graphicsItem);
    int  indexOf(QGraphicsItem *graphicsItem);

public slots:
    void geometryAdded(QGraphicsItem *graphicsItem);

private:
    List<Spline*> m_splines;

    bool isSpline(QGraphicsItem *graphicsItem);
};

#endif // SPLINE_MODEL

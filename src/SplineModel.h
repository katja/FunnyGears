#ifndef SPLINE_MODEL
#define SPLINE_MODEL

#include "stable.h"
#include "List.h"
#include "Spline.h"
#include "SceneTreeModel.h"
#include "GraphicsItemModel.h"

class SplineModel : public GraphicsItemModel {

Q_OBJECT

public:
    SplineModel(SceneTreeModel *model, QObject *parent = 0);
    ~SplineModel();

    enum Properties {
        DEGREE, TORN_TO_EDGES, PROPERTIES //PROPERTIES has to stay last "property", as this gives the number of available properties!
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool addSpline(GraphicsSpline *graphicsSpline);
    bool removeSpline(GraphicsSpline *graphicsSpline);

    bool operatesOn(QGraphicsItem *graphicsItem);
    int  indexOf(QGraphicsItem *graphicsItem);

public slots:
    void graphicsItemAdded(QGraphicsItem *graphicsItem);

private:
    List<GraphicsSpline*> m_splines;

    bool isSpline(QGraphicsItem *graphicsItem);
};

#endif // SPLINE_MODEL

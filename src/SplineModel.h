#ifndef SPLINE_MODEL
#define SPLINE_MODEL

#include "stable.h"
#include "List.h"
#include "Spline.h"

class SplineModel : public QAbstractTableModel {

public:
    SplineModel(QObject *parent = 0);
    ~SplineModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool addSpline(Spline *spline);
    bool removeSpline(Spline *spline);

private:
    enum Properties {
        DEGREE, TORN_TO_EDGES
    };
    int m_numberOfProperties;
    List<Spline*> m_splines;
};

#endif // SPLINE_MODEL

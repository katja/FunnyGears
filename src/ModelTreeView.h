#ifndef MODEL_TREE_VIEW
#define MODEL_TREE_VIEW

#include "stable.h"
#include "Model.h"

class ModelTreeView : public QTreeView {

public:
    ModelTreeView(Model *model, QWidget *parent = 0);
    virtual ~ModelTreeView();

    QModelIndexList selectedIndexes() const override; //does nothing but making this method public

protected:
    Model *m_model;
    // QPixmap m_rightArrow, m_downArrow, m_spline;

    virtual void drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const;

};

#endif // MODEL_TREE_VIEW

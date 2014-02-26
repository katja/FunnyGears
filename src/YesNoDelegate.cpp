#include "YesNoDelegate.h"
#include "SceneTreeModel.h"

YesNoDelegate::YesNoDelegate(QObject *parent) : QStyledItemDelegate(parent) {
}

YesNoDelegate::~YesNoDelegate() {
}

bool YesNoDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    if(event->type() == QEvent::MouseButtonDblClick && index.data(Qt::DecorationRole).canConvert<QIcon>()) {
        if(model->inherits("SceneTreeModel")) {
            SceneTreeModel *sceneModel = static_cast<SceneTreeModel*>(model);
            sceneModel->toggleValue(index);
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

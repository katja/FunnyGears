#include "YesNoDelegate.h"
#include "Model.h"

YesNoDelegate::YesNoDelegate(QObject *parent) : QStyledItemDelegate(parent) {
    std::cout << "YesNoDelegate is created" << std::endl;
}

YesNoDelegate::~YesNoDelegate() {
    std::cout << "YesNoDelegate is deleted" << std::endl;
}

bool YesNoDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    if(event->type() == QEvent::MouseButtonDblClick && index.data(Qt::DecorationRole).canConvert<QIcon>()) {
        if(model->inherits("Model")) {
            Model *m = static_cast<Model*>(model);
            m->toggleValue(index);
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

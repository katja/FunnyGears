#ifndef YES_NO_DELEGATE
#define YES_NO_DELEGATE

#include "stable.h"

/** Special QStyledItemDelegate which allows toggling boolean values,
 *  represented by QIcons in Model by calling toggleValue(QModelIndex)
 *  in the Model.
 */
class YesNoDelegate : public QStyledItemDelegate {

public:
    YesNoDelegate(QObject *parent = 0);
    ~YesNoDelegate();

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

};

#endif // YES_NO_DELEGATE

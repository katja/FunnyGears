#ifndef LIST
#define LIST

// #include <initializer_list>
#include "stable.h"

template <typename T>
class List : public QList<T> {

public:
    List() : QList<T>() {}
    List(const QList<T> &other) : QList<T>(other) {}
    ~List() {}

    bool containsIndex(int i) const;
    int lastIndex() const;

};

template <typename T>
bool List<T>::containsIndex(int i) const {
    if(i < 0 || i >= QList<T>::size())
        return false;
    return true;
}

template <typename T>
int List<T>::lastIndex() const {
    return QList<T>::size() - 1;
}

#endif // LIST

#ifndef QVARIANTPTR_H
#define QVARIANTPTR_H

#include <QVariant>

template <class T> class QVariantPtr
{
public:
    static T* asPtr(QVariant v)
    {
        return  (T *) v.value<void *>();
    }

    static QVariant asQVariant(T* ptr)
    {
        return QVariant::fromValue((void *) ptr);
    }
};

#endif // QVARIANTPTR_H

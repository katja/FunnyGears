#ifndef HELPERS
#define HELPERS

#include "stable.h"
#include "definitions.h"
#include "basic_objects/Spline.h"

////////////////////////////////////
////////// STREAMING ///////////////

std::ostream& operator <<(std::ostream &os, const QPointF &point);

std::ostream& operator <<(std::ostream &os, const QRectF &rect);

std::ostream& operator <<(std::ostream &os, const vec2 vec);

std::ostream& operator <<(std::ostream &os, const QVector<vec2> v);

std::ostream& operator <<(std::ostream &os, const vector<vec2> v);

std::ostream& operator <<(std::ostream &os, const QVector<real> v);

std::ostream& operator <<(std::ostream &os, const vector<real> v);

std::ostream& operator <<(std::ostream &os, const Spline &spline);

////////////////////////////////////
////////// OTHER ///////////////////

QColor darkenColor(const QColor &color, int degree = 60);

real angleBetween(const vec2 &a, const vec2 &b);

real absolute(const real &number);

real toRad(const real &degree);

real toDegree(const real &rad);

QVector<QPointF> convertToQVectorWithQPointFs(const vector<vec2> &v);

////////////////////////////////////
////////// TEMPLATE HELPERS ////////
// They are implemented in header file, because this is necessary for templates
// TODO: Put implementation in a separate file and include it here with "#include '...'"
template <typename T>
vector<T> convertToVector(const QVector<T> &qV) {
    vector<T> v(qV.size());
    for(int i = 0; i < qV.size(); ++i) {
        v[i] = qV.at(i);
    }
    return v;
};

template <typename T>
QVector<T> convertToQVector(const vector<T> &v) {
    QVector<T> qV(v.size());
    for(uint i = 0; i < v.size(); ++i) {
        qV[i] = v.at(i);
    }
    return qV;
};

template <typename T>
vector<T> copyVectorPart(const vector<T> &v, uint start, uint length) {
    //if length would exceed size of given vector v, it is shortened to copy whole rest of v but without trying to copy more
    if(v.size() < start + length)
        length = v.size() - start;
    vector<T> copiedVector(length);
    for(uint i = 0; i < length; ++i) {
        copiedVector[i] = v.at(start + i);
    }
    return copiedVector;
}



#endif // HELPERS

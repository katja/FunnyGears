#ifndef HELPERS
#define HELPERS

#include "stable.h"
#include "definitions.h"
#include "preferences.h"
class Spline;
enum class ErrorCode;
struct ContactPoint;
struct NoneContactPoint;
struct WrongContactPoint;
struct CPcutting;
struct NCPcutting;
class ContactPointIterator;


////////////////////////////////////
////////// STREAMING ///////////////

std::ostream& operator <<(std::ostream &os, const QPointF &point);

std::ostream& operator <<(std::ostream &os, const QRectF &rect);

std::ostream& operator <<(std::ostream &os, const vec2 vec);

std::ostream& operator <<(std::ostream &os, const QVector<vec2> v);

std::ostream& operator <<(std::ostream &os, const vector<vec2> v);

std::ostream& operator <<(std::ostream &os, const vector<int> v);

std::ostream& operator <<(std::ostream &os, const QVector<real> v);

std::ostream& operator <<(std::ostream &os, const vector<real> v);

std::ostream& operator <<(std::ostream &os, const Spline &spline);

std::ostream& operator <<(std::ostream &os, const ErrorCode &error);

std::ostream& operator <<(std::ostream &os, const ContactPoint &contactPoint);

std::ostream& operator <<(std::ostream &os, const NoneContactPoint &noneContactPoint);

std::ostream& operator <<(std::ostream &os, const WrongContactPoint &WrongContactPoint);

std::ostream& operator <<(std::ostream &os, const list< list<ContactPoint*> *> &listsOfContactPoints);

std::ostream& operator <<(std::ostream &os, const CPcutting &cutting);

std::ostream& operator <<(std::ostream &os, const NCPcutting &cutting);

std::ostream& operator <<(std::ostream &os, const ContactPointIterator &iterator);

////////////////////////////////////
////////// OTHER ///////////////////

QColor darkenColor(const QColor &color, int degree = 60);

QColor lightUpColor(const QColor &color, real degree = 0.6);

int max(int a, int b);

real absolute(const real &number);

real toRad(const real &degree);

real toDegree(const real &rad);

real dot(const vec2 &a, const vec2& b);

real cross(const vec2 &a, const vec2 &b);

vec2 normalize(const vec2 &a);

real angleBetween(const vec2 &a, const vec2 &b);

real angleBetweenN(const vec2 &normalizedA, const vec2 &normalizedB);

real square(const real &number);

QVector<QPointF> convertToQVectorWithQPointFs(const vector<vec2> &v);


QPainterPath circlePath(real radius);

QPainterPath pitchesPath(uint numberOfSections, vec2 startPoint, real sectionLength);

void drawCircle(QPainter *painter, vec2 center, real radius = Preferences::PointRadius);

void drawLine(QPainter *painter, vec2 start, vec2 end);

void drawArc(QPainter *painter, real startAngle, real stopAngle, real radius, vec2 center = vec2(0, 0));


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

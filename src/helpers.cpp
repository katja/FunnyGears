#include "helpers.h"
#include "definitions.h"
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "basic_objects/Spline.h"
#include "basic_objects/ContactPoint.h"
#include "basic_objects/ContactPointHelpers.h"
#include "basic_objects/ContactPointIterator.h"


////////////////////////////////////
////////// STREAMING ///////////////

std::ostream& operator<<(std::ostream &os, const QPointF &point) {
    return os << "(" << point.x() << ", " << point.y() << ")";
}

std::ostream& operator<<(std::ostream &os, const QRectF &rect) {
    return os << "" << rect.topLeft() << ", " << rect.bottomRight() << "";
}

std::ostream& operator <<(std::ostream &os, const vec2 vec) {
    return os << "[" << vec.x << "| " << vec.y << "]";
}

std::ostream& operator <<(std::ostream &os, const QVector<vec2> vector) {
    os << "[";
    int i = 0;
    for(; i < vector.size() - 1; ++i) {
        os << vector.at(i) << "|\t";
    }
    os << vector.at(i) << "]";
    return os;
}

std::ostream& operator <<(std::ostream &os, const vector<vec2> v) {
    os << "[";
    uint i;
    for(i = 0; i < v.size() - 1; ++i) {
        os << v[i] << "|\t";
    }
    os << v[i] << "]";
    return os;
}

std::ostream& operator <<(std::ostream &os, const vector<int> v) {
    os << "[";
    uint i;
    for(i = 0; i < v.size() - 1; ++i) {
        os << v[i] << ",\t";
    }
    os << v[i] << "]";
    return os;
}

std::ostream& operator <<(std::ostream &os, const QVector<real> vector) {
    for(int i = 0; i < vector.size(); ++i) {
        os << vector.at(i);
        if(i != vector.size() - 1)
            os << ", ";
    }
    return os;
}

std::ostream& operator <<(std::ostream &os, const vector<real> v) {
    for(uint i = 0; i < v.size(); ++i) {
        os << v[i];
        if(i != v.size() - 1)
            os << ", ";
    }
    return os;
}

std::ostream& operator <<(std::ostream &os, const Spline &spline) {
    os << "SPLINE with properties...\n";
    os << "Degree: " << spline.degree();
    os << "\nControl Points: (" << spline.numberOfControlPoints() << " x)\n";
    const vector<vec2> controlPoints = spline.controlPoints();
    foreach(vec2 point, controlPoints) {
        os << point << "    ";
    }
    os << "\n";
    os << "Knots: (" << spline.numberOfKnots() << " x)\n";
    const vector<real> knots = spline.knots();
    for(uint i = 0; i < knots.size(); ++i) {
        os << knots.at(i);
        if(i != knots.size() - 1)
            os << ", ";
    }
    os << "\n";
    os << "Evaluable from " << spline.lowerDomainLimit() << " until " << spline.upperDomainLimit(0.0) << std::endl;
    os << "TornToEdges? ";
    if(spline.isTornToEdges())
        os << "yes";
    else
        os << "no";
    os << "\n";
    return os;
}

std::ostream& operator <<(std::ostream &os, const ErrorCode &error) {
    switch(error) {
        case ErrorCode::NO_ERROR:
            return os << "no error";
        case ErrorCode::NO_CUT_WITH_REFERENCE_RADIUS:
            return os << "no cut with reference radius found";
        default:
            return os << "error case unknown";
    }
}

    real evaluationValue; //Spline curve of driving gear was evaluated at this value to get this ContactPoint
    uint evaluationStep; //Evaluation number of ContactPoint
    vec2 point; //Point on driven gear
    vec2 normal; //Must be normalized! normal on driven gear
    vec2 originPoint; //Point on driving gear
    vec2 originNormal; //Must be normalized! Normal on driving gear
    vec2 contactPosition; //Position where contact took place
    vec2 normalInContact; //Must be normalized! Normal of driving gear in contact point
    real forbiddenAreaLength; //Thickness of the driving gear
    vec2 forbiddenAreaEndPoint; //Opposite point of driving gear transferred to driven gear
    bool usedLargerValue; // In most cases the normal of the driving gear has two cuts with reference radius => originPoint + t * originNormal => if maximum possible value is taken, this attribute is set to true
    bool isCovered; //is set to true, when this point can't become a contact point
    bool isRotated;
    ErrorCode error;


std::ostream& operator <<(std::ostream &os, const ContactPoint &cp) {
    os << "CONTACT POINT with properties...\n";
    os << "Evaluation value:        " << cp.evaluationValue << "\n";
    os << "Evaluation step:         " << cp.evaluationStep << "\n";
    os << "Driven gear point:      " << cp.point << "\n";
    os << "Driven gear normal:     " << cp.normal << "\n";
    os << "Driving gear point:      " << cp.originPoint << "\n";
    os << "Driving gear normal:     " << cp.originNormal << "\n";
    os << "Contact position point:  " << cp.contactPosition << "\n";
    os << "Contact position normal: " << cp.normalInContact << "\n";
    os << "Forbidden area length:   " << cp.forbiddenAreaLength << "\n";
    os << "Forbidden area end point:" << cp.forbiddenAreaEndPoint << "\n";
    os << "Used larger t value: "     << (cp.usedLargerValue ? "yes" : "no") << "\n";
    os << "This is a contact point: " << (cp.isCovered ? "no" : "yes") << "\n";
    os << "Error: " << cp.error << "\n";
    return os;
}

std::ostream& operator <<(std::ostream &os, const NoneContactPoint &ncp) {
    os << "NONE CONTACT POINT with properties...\n";
    os << "Evaluation value:        " << ncp.evaluationValue << "\n";
    os << "Evaluation step:         " << ncp.evaluationStep << "\n";
    os << "Driving gear point:      " << ncp.originPoint << "\n";
    os << "Driving gear normal:     " << ncp.originNormal << "\n";
    os << "Forbidden area length:   " << ncp.forbiddenAreaLength << "\n";
    os << "Forbidden area end point:" << ncp.forbiddenAreaEndPoint << "\n";
    os << "Used larger t value: "     << (ncp.usedLargerValue ? "yes" : "no") << "\n";
    os << "This is a contact point: " << (ncp.isCovered ? "no" : "yes") << "\n";
    os << "Error: " << ncp.error << "\n";
    os << "Driven gear points:\n" << ncp.points << "\n";
    os << "Driven gear normal:\n" << ncp.normals << "\n";
    os << "Contact position points:\n" << ncp.contactPositions << "\n";
    os << "Contact position normals:\n" << ncp.normalsInContact << "\n";
    return os;
}

std::ostream& operator <<(std::ostream &os, const WrongContactPoint &wcp) {
    os << "WRONG CONTACT POINT with properties...\n";
    os << "Evaluation value:        " << wcp.evaluationValue << "\n";
    os << "Evaluation step:         " << wcp.evaluationStep << "\n";
    os << "Driven gear point:      " << wcp.point << "\n";
    os << "Driven gear normal:     " << wcp.normal << "\n";
    os << "Driving gear point:      " << wcp.originPoint << "\n";
    os << "Driving gear normal:     " << wcp.originNormal << "\n";
    os << "Contact position point:  " << wcp.contactPosition << "\n";
    os << "Contact position normal: " << wcp.normalInContact << "\n";
    os << "Forbidden area length:   " << wcp.forbiddenAreaLength << "\n";
    os << "Forbidden area end point:" << wcp.forbiddenAreaEndPoint << "\n";
    os << "Used larger t value: "     << (wcp.usedLargerValue ? "yes" : "no") << "\n";
    os << "This is a contact point: " << (wcp.isCovered ? "no" : "yes") << "\n";
    os << "Error: " << wcp.error << "\n";
    return os;
}

std::ostream& operator <<(std::ostream &os, const list< list<ContactPoint*> *> &listsOfContactPoints) {
    std::cout << "list of lists of ContactPoints – " << listsOfContactPoints.size() << " lists included." << std::endl;
    for(list<ContactPoint*> *l : listsOfContactPoints) {
        std::cout << "-------------------------------\nLIST with " << l->size() << " ContactPoints:" << std::endl;
        for(ContactPoint *cp : *l) {
            std::cout << *cp << std::endl;
        }
        std::cout << "-------------------------------" << std::endl;
    }
    return os;
}

std::ostream& operator <<(std::ostream &os, const CPcutting &cutting) {
    os << "CPcutting with properties...\n";
    os << "value t:         " << cutting.t << std::endl;
    os << "cuttingPoint:    " << cutting.cuttingPoint << std::endl;
    os << "iterator before: " << *cutting.placeBeforeCutting << std::endl;
    os << "pointer to list: " << cutting.list << std::endl;
    os << "location:        ";
    if(cutting.location == IterationLocation::Ground)
        os << "Ground";
    else if(cutting.location == IterationLocation::Top)
        os << "Top";
    else
        os << "Undefined";
    return os << std::endl;
}

std::ostream& operator <<(std::ostream &os, const NCPcutting &cutting) {
    os << "NCPcutting with properties...\n";
    os << "value t:            " << cutting.t << std::endl;
    os << "cuttingPoint:       " << cutting.cuttingPoint << std::endl;
    os << "pointer to NCP:     " << cutting.ncp << std::endl;
    os << "placeBeforeCutting: " << cutting.placeBeforeCutting << std::endl;
    os << "location:           ";
    if(cutting.location == IterationLocation::Ground)
        os << "Ground";
    else if(cutting.location == IterationLocation::Top)
        os << "Top";
    else
        os << "Undefined";
    return os << std::endl;
}

std::ostream& operator <<(std::ostream &os, const ContactPointIterator &iterator) {
    return iterator.currentStateStream(os);
}

////////////////////////////////////
////////// OTHER ///////////////////

QColor darkenColor(const QColor &color, int degree) {
    QColor darkenedColor;
    darkenedColor.setRed(color.red() < degree ? 0 : color.red() - degree);
    darkenedColor.setGreen(color.green() < degree ? 0 : color.green() - degree);
    darkenedColor.setBlue(color.blue() < degree ? 0 : color.blue() - degree);
    return darkenedColor;
}

QColor lightUpColor(const QColor &color, real degree) {
    return QColor(  color.red() + (255 - color.red()) * degree,
                    color.green() + (255 - color.green()) * degree,
                    color.blue() + (255 - color.blue()) * degree);
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

real dot(const vec2 &a, const vec2 &b) {
    return a.x * b.x + a.y * b.y;
}

real cross(const vec2 &a, const vec2 &b) {
    return a.x * b.y - a.y * b.x;
}

vec2 normalize(const vec2 &a) {
    return a * (1.0 / sqrt(a.x * a.x + a.y * a.y));
}

real angleBetween(const vec2 &a, const vec2 &b) {
    vec2 nA = a * (1.0 / sqrt(a.x * a.x + a.y * a.y));
    vec2 nB = b * (1.0 / sqrt(b.x * b.x + b.y * b.y));
    return angleBetweenN(nA, nB);
}

real angleBetweenN(const vec2 &normalizedA, const vec2 &normalizedB) {
    return acos(normalizedA.x * normalizedB.x + normalizedA.y * normalizedB.y);
}

real square(const real &number) {
    return number * number;
}

real absolute(const real &number) {
    if(number < 0.0)
        return -number;
    else
        return number;
}

real toRad(const real &degree) {
    return degree * M_PI / 180.0;
}

real toDegree(const real &rad) {
    return rad * 180.0 / M_PI;
}

QVector<QPointF> convertToQVectorWithQPointFs(const vector<vec2> &v) {
    QVector<QPointF> qV(v.size());
    for(uint i = 0; i < v.size(); ++i) {
        qV[i] = QPointF(v.at(i).x, v.at(i).y);
    }
    return qV;
};

QPainterPath circlePath(real radius) {
    QPainterPath path;
    uint samples = 96;
    real sampleSize = 2.0 * M_PI / (real)samples;
    path.moveTo(radius, 0);
    for(uint i = 1; i < 96; ++i) {
        path.lineTo(cos(sampleSize * i) * radius, -sin(sampleSize * i) * radius);
    }
    path.lineTo(radius, 0);
    return path;
}

QPainterPath pitchesPath(uint numberOfSections, vec2 startPoint, real sectionLength) {
    QPainterPath path;
    real angularPitch = 2.0 * M_PI / (real)numberOfSections;
    vec2 p = normalize(startPoint) * sectionLength;
    for(uint i = 0; i < numberOfSections; ++i) {
        real rotationInRad = angularPitch * i;
        vec2 turnedPoint = glm::rotate(p, rotationInRad);
        path.lineTo(QPointF(turnedPoint.x, turnedPoint.y));
        path.moveTo(0,0);
    }
    return path;
}

void drawCircle(QPainter *painter, vec2 center, real radius) {
    painter->drawEllipse(QPointF(center.x, center.y), radius, radius);
}

void drawLine(QPainter *painter, vec2 start, vec2 end) {
    painter->drawLine(QPointF(start.x, start.y), QPointF(end.x, end.y));
}

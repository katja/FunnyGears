#ifndef SPLINE
#define SPLINE

#include "definitions.h"

class Spline {

public:
    Spline();
    ~Spline();

    vec2  evaluate(qreal value) const;
    void  curve(QVector<QPointF> &curve) const;
    int   degree() const;
    void  setDegree(int degree);

    /** Checks if there exists a spline in the current state.
     *  This depends on the degree and the number of control points.
     */
    bool  enoughControlPoints() const;

    /** Returns the lowest value, which can be evaluated.
     *  If not enough control points are available at the moment,
     *  it returns -1.0
     */
    qreal lowerDomainBorder() const;

    /** Returns a border for the highest value that can be evalueated.
     *  Attention: this value MUST NOT BE REACHED! It has to be a little
     *  bit smaller!
     *  If not enough control points are available at the moment,
     *  it returns -1.0
     */
    qreal upperDomainBorder() const;

    void  addControlPoint(vec2 point);
    void  addControlPoint(QPointF point);

    void  moveControlPoint(int index, QPointF newPosition);

    void  setTornToEdges(bool tearToEdges);

    bool isTornToEdges();

    /** Returns the index of the knot in the knot array,
     *  whose value is smaller or equal to the given value.
     *  (If a value is given, which is lower than all current knots
     *  index 0 is returned.)
     */
    int   lowerNextKnot(int value) const;

private:
    QVector<real> m_knots;
    QVector<vec2> m_controlPoints;
    int           m_degree;
    bool          m_tornToEdges;

    void deBoor(QVector<vec2> &controlPoints, qreal value, qreal n) const;
    void deBoor(QVector<vec2> &controlPoints, qreal value, qreal n, int degree, int stop = 0) const;
    void equalizeFirstKnots();
    void equalizeLastKnots();
    void makeDifferentFirstKnots();
    void makeDifferentLastKnots();

};

#endif // SPLINE

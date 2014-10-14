#ifndef SPLINE
#define SPLINE

#include <iostream>
#include "definitions.h"
#include "basic_objects/Ray.h"

class Spline {

public:
    Spline(uint degree = 2);
    Spline(const Spline &other);
    Spline(vector<vec2> controlPoints, vector<real> knots);
    Spline(vector<vec2> interpolationPoints); //TODO!
    ~Spline();

    Spline& operator=(const Spline &other);

    /** Evalutes the spline at the given value
     *  Attention: make sure that the spline is valid and that value is one of the interval
     *  [lowerDomainLimit(), upperDomainLimit())
     *  Method does not check this!
     *  @param value between lowerDomainLimit() and upperDomainLimit()
     *  @return point of spline at given value
     */
    vec2  evaluate(real value) const;

    /** Uses de Boor algorithm to get the derivative vector at the given value
     *  If Spline is not valid or value is not valid, it returns a 0-vector.
     *  @param value specifies the position of the spline for the derivative
     *  @return derivation vector of the spline evaluated at value
     */
    vec2  derivative(real value) const;

    /** Uses derivative to form the normal
     *  If Spline is not valid or value is not valid, it returns a 0-vector.
     *  @param value specifies the position of the spline for the normal
     *  @return normal vector of the spline evaluated at value
     */
    vec2  normal(real value) const;

    /** Evaluates spline curve.size() times in equal intervals and writes
     *  the result in the curve vector. Evaluation starts with lowest possible
     *  value and continues to upper limit.
     *  If spline is not valid, nothing is done!
     *  @param curve used for the result, size specifies number of samples, is totally filled
     */
    void  curve(vector<QPointF> &curve) const;

    /** @return degree of spline curve */
    uint  degree() const;

    /** Sets the degree of the spline to degree
     *  Updates knot vector respectively.
     *  @param degree new degree for spline, must be greater than 0
     */
    void  setDegree(uint degree);

    /** @return number of control points of the spline
     */
    uint  numberOfControlPoints() const;

    /** @return control points of the spline */
    const vector<vec2>& controlPoints() const;

    /** Appends a control point at point.
     *  The knot vector is continued with a suitable value
     *  @param point position of the new control point
     */
    void  addControlPoint(vec2 point);

    /** Appends a control point at point.
     *  @see addControlPoint(vec2 point)
     */
    void  addControlPoint(QPointF point);

    /** Removes control point at given index.
     *  As a control point is normally influenced by more than one knot
     *  simply tha last knot is removed. This also has the advantage,
     *  that the removal does not result in a bigger and probably unwanted
     *  gap in the knot vector. If the property tornToEdges is set, this property
     *  is restored (so remains active).
     *  @param index index of the control point to be removed (index >= 0)
     */
    void  removeControlPoint(uint index);

    /** Sets the position of the control points at index index to new Position.
     *  If given index is out of range, it does nothing.
     *  @param index index of the control point which is set to newPosition (index >= 0)
     *  @param newPosition new position for control point with given index
     */
    void  moveControlPoint(uint index, QPointF newPosition);

    /** @return number of knots of the spline
     *  Remeber that first and last knot are not needed for the rendering of the spline curve.
     *  They are only included on definition purpose.
     */
    uint  numberOfKnots() const;

    /** @return knots of the spline */
    const vector<real>& knots() const;

    /** Searches the knot vector for given knotValue and return the occurences.
     *  Note that at the moment this is a comparisons of real values and therefore
     *  not really a useful implementation!
     *  @param knotValue value to be searched for occurences in the knot vector
     *  @return occurences of knotValue in the knot vector
     */
    uint  multiplicityOfKnotValue(real knotValue) const;

    /** Multiplicity of knot at index knotValue
     *  Searches the knot vector for knots with same value as the one at given knotIndex.
     *  Searching is started from knotIndex in both directions.
     *  @param knotIndex index of knot whichs multiplicity is asked (knotIndex >= 0)
     *  @return multiplicity of knot at knotIndex
     */
    uint  multiplicity(uint knotIndex) const;

    /** Inserts knots and control points without changing the curve
     *  until between every two knots is a maximum of minDist gap.
     *  With this knot refinement the control point polygon converges to the curve.
     *  Attention: This method really changes the spline curve!
     *  So make a copy if you want the old representation is worthwhile!
     *  @param minDist the maximal distance in the evaluable knot vector between two knots
     *                 after the knot refinement
     */
    void knotRefinement(real minDist);

    /** Inserts a knot at the given knotValue and a suitable point without changing the curve.
     *  If the knot should have a multiplicity bigger than 1, give a suitable value
     *  (smallest multiplicity value is 1, largest is m_degree)
     *  Attention: This method does NOT check the validity of the curve, nor does it check
     *  if the given knotValue is a possible value. So make sure you check this first!
     *  @param knotValue a suitable new knotValue to insert
     *  @param multiplicity how often this new knotValue should be inserted (0 < multiplicity <= m_degree)
     */
    void refineAt(real knotValue, uint multiplicity = 1);

    /** The spline curve can be expanded to form a closed curve.
     *   @param isClosed if true, curve will be expanded to a closed curve,
     *                   if false, periodicity will be removed //TODO
     */
    void  setClosedCurve(bool isClosed);

    /** Uses saved value to decide, if spline is or is not a closed curve.
     *  Note: to update this state, call @see updateClosedCurve()
     *  @return true, if saved value says that curve is closed
     */
    bool  isClosed() const;

    /** TODO!!!
     */
    void updateClosedCurve();

    /** Clamps start and end to the first and last control points if tearToEdges.
     *  This is possible with a specific choice of the knot vector.
     *  If the spline is valid, it sets the first and last n = m_degree + 1
     *  knot values each to the same value.
     *  If tearToEdges is false, it replaces the first and last knots with evenly
     *  distributed values.
     *  Note that this method changes the knot values and therefore earlier
     *  chosen values will be lost!
     */
    void  setTornToEdges(bool tearToEdges);


    /** Uses saved value to decide, if spline is or is not torn to edges.
     *  Note: to update this state, call @see updateTornToEdges()
     *  @return True if value saved in spline says, that it is torn to edges.
     */
    bool  isTornToEdges() const;

    /** Tests the knot vector to decide if the spline is torn to first and last
     *  control point and not the saved value.
     *  @return True if knots is valid and belongs to a spline with clamped ends
     */
    bool  checkTornToEdges() const;

    /** Uses @see checkTornToEdges() to decide, if the spline is torn to the edges
     *  and saves this information in the spline.
     */
    void  updateTornToEdges();

    /** Checks the relation between degree, number of knots and number of control points.
     *  When there are too less control points for the current degree a spline curve
     *  can not be created.
     *  @return True if curve can be drawn.
     */
    bool  isValid() const;

    /** Examines the spline curve and writes any error messages to the given ostream.
     *  This method is other than @param isValid() in that it not only investigates the
     *  relation of degree and number of knots and control points, but also the values of
     *  the knot vector.
     *  @param os The stream the error messages are written to
     *  @return True if no error was found
     */
    bool  isValidMessages(std::ostream &os) const;

    /** Returns the lowest value, which can be evaluated.
     *  If spline is not valid in the current state it returns -1.0
     *  @return lowest possible value, which can be evaluated, or -1.0f if spline is not valid
     */
    real  lowerDomainLimit() const;

    /** Gives a limit for the highest value that can be evaluated.
     *  Attention: this value MUST NOT BE REACHED! It has to be a little
     *  bit smaller!
     *  It first checks the spline with @see isValid() and if it is not, it return -1.0f
     *  @return limit for the highest value, which must not be reached when evaluating
     *  the spline or -1.0f if spline is not valid
     */
    real  upperDomainLimit() const;

    /** Returns the index of the knot in the knot array,
     *  whose value is smaller or equal to the given value.
     *  If there are more knots with this same small value,
     *  it returns the highest index.
     *  (If a value is given, which is lower than all current knots
     *  index 0 is returned.)
     */
    uint  lowerNextKnot(real value) const;

    /** Cuts given ray with the spline curve and expands the vector intersectionPoints with
     *  found (approximated) intersection points. Already present values in the
     *  intersectionPoints vector will not be deleted!
     *  @param ray Ray which is searched for intersections with the spline curve
     *  @param intersectionPoints found intersection points are inserted in this vector.
     */
    void  getIntersectionPointsWithRay(const Ray &ray, vector<vec2> &intersectionPoints) const;

protected:
    vector<real>    m_knots;
    vector<vec2>    m_controlPoints;
    uint            m_degree;
    bool            m_isClosed;
    bool            m_tornToEdges;

    void deBoor(vector<vec2> &controlPoints, real value, real n) const;
    void deBoor(vector<vec2> &controlPoints, real value, real n, uint degree, uint stop = 0) const;
    void adjustKnots();

    /** Sets the first m_degree + 1 knots to the same value.
     *  More precisely: they are set to the value one less than at knot index (m_degree + 2)
     *  ATTENTION: precondition for this method is that the spline is valid!
     *  So check this first (@see isValid())
     */
    void equalizeFirstKnots();

    /** Sets the last m_degree + 1 knots to the same value.
     *  More precisely: they are set to the value one more than at the previous knot
     *  ATTENTION: precondition for this method is that the spline is valid!
     *  So check this first (@see isValid())
     */
    void equalizeLastKnots();

    /** Sets the first m_degree + 1 knots to different values,
     *  each one less than the one that follows.
     *  ATTENTION: precondition for this method is that the spline is valid!
     *  So check this first (@see isValid())
     */
    void makeDifferentFirstKnots();

    /** Sets the last m_degree + 1 knots to different values,
     *  each one to one more than the previous one.
     *  ATTENTION: precondition for this method is that the spline is valid!
     *  So check this first (@see isValid())
     */
    void makeDifferentLastKnots();

};

#endif // SPLINE

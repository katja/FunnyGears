#ifndef SPLINE
#define SPLINE

#include <iostream>
#include "definitions.h"
#include "basic_objects/Ray.h"

/** Spline class, which uses a B-Spline basis function for the representation.
 *  For given m+1 control points are m+1+degree knot values required for a correct
 *  B-Spline calculation. The first and last knot don't influence the curve, nevertheless
 *  as they are necessary for the correct definition of B-Splines, they are implemented
 *  here, too.
 *  Therefore the following equation has to be satisfied:
 *  #(control points) = #(knots) - degree - 1
 *  This equation is also tested in isValid()
 */
class Spline {

public:
    Spline(uint degree = 2);
    Spline(const Spline &other);
    Spline(vector<vec2> controlPoints, vector<real> knots);
    Spline(vector<vec2> interpolationPoints); //TODO!
    virtual ~Spline();

    Spline& operator=(const Spline &other);

    /** Evalutes the spline at the given value
     *  Attention: make sure that the spline is valid and that value is one of the interval
     *  [lowerDomainLimit(), upperDomainLimit())
     *  Method does not check this!
     *  @param value between lowerDomainLimit() and upperDomainLimit()
     *  @return point of spline at given value
     */
    virtual vec2 evaluate(real value) const;

    /** @brief Returns the first point of the spline curve
     *
     *  Evaluates at the first value, where evaluation can take place
     *  @warning This method requires a valid Spline. So check validness (#isValid()) before!
     *  @return first point of curve
    */
    virtual vec2 firstPoint() const;

    /** @brief Returns the last point of the spline curve
     *
     *  Evaluates at one of the highest possible values, where evaluation
     *  can take place. As the highest possible value must remain a bit smaller
     *  than upperDomainLimit(), @p epsilon is passed to specify the difference
     *  to this highest value.
     *  @warning: This method requires a valid Spline. So check the validness before
     *  (#isValid())
     *  @param epsilon Specifies the distance to the upperDomainLimit() for the evaluation
     *  @return last point of curve (depends on epsilon)
     */
    virtual vec2 lastPoint(real epsilon = 0.00001) const;

    /** Uses de Boor algorithm to get the derivative vector at the given value
     *  If Spline is not valid or value is not valid, it returns a 0-vector.
     *  @param value specifies the position of the spline for the derivative
     *  @return derivation vector of the spline evaluated at value
     */
    virtual vec2 derivative(real value) const;

    /** Uses derivative to form the normal
     *  If Spline is not valid or value is not valid, it returns a 0-vector.
     *  @param value specifies the position of the spline for the normal
     *  @return normal vector of the spline evaluated at value
     */
    virtual vec2 normal(real value) const;

    /** Evaluates spline curve.size() times in equal intervals and writes
     *  the result in the curve vector. Evaluation starts with lowest possible
     *  value and continues to upper limit.
     *  If spline is not valid, nothing is done!
     *  @param curve used for the result, size specifies number of samples, is totally filled
     */
    virtual void curve(vector<QPointF> &curve) const;

    /** @return degree of spline curve */
    virtual uint degree() const;

    /** Sets the degree of the spline to degree
     *  Updates knot vector respectively.
     *  @param degree new degree for spline, must be greater than 0
     */
    virtual void setDegree(uint degree);

    /** @brief Returns the number of control points of the spline
     *
     *  @return number of control points of the spline
     */
    virtual uint numberOfControlPoints() const;

    /** @return control points of the spline */
    virtual const vector<vec2>& controlPoints() const;

    /** @brief Returns the control point with index @param i
     *
     *  @return control point with index @param i
     *  @note no test is done, if the given index exists in the control points.
     *        So check this before.
     */
    virtual vec2 controlPoint(uint i) const;

    /** @brief Appends a control point at point @p point.
     *
     *  The knot vector is continued with a suitable value.
     *
     *  @param point position of the new control point
     */
    virtual void addControlPoint(vec2 point);

    /** @brief Appends a control point at the point with coordinates (x, y).
     *
     *  @see addControlPoint(vec2 point)
     */
    virtual void addControlPoint(real x, real y);

    /** @brief Removes control point at given index.
     *
     *  The knot vector then has to be reduced by one value, too.
     *  As a control point is normally influenced by more than one knot
     *  simply the last knot is removed. This also has the advantage,
     *  that the removal does not result in a bigger and probably unwanted
     *  gap in the knot vector. If the property @p m_tornToEdges is set, this property
     *  is restored (so remains active).
     *  @param index index of the control point to be removed
     */
    virtual void removeControlPoint(uint index);

    /** @brief Sets the position of the control point at index @p index to @newPosition.
     *
     *  If given index is out of range, it does nothing.
     *  @param index index of the control point which is set to newPosition (index >= 0)
     *  @param newPosition new position for control point with given index
     */
    virtual void moveControlPoint(uint index, vec2 newPosition);

    /** @brief Sets the position of the control point at index @p index to @newPosition.
     *
     *  overloaded method.
     *  @see moveControlPoint(uint index, vec2 new Position)
     */
    virtual void moveControlPoint(uint index, QPointF newPosition);


    /** @return number of knots of the spline
     *  Remeber that first and last knot are not needed for the rendering of the spline curve.
     *  They are only included on definition purpose.
     */
    virtual uint numberOfKnots() const;

    /** @return knots of the spline */
    virtual const vector<real>& knots() const;

    /** Searches the knot vector for given knotValue and return the occurences.
     *  Note that at the moment this is a comparisons of real values and therefore
     *  not really a useful implementation!
     *  @param knotValue value to be searched for occurences in the knot vector
     *  @return occurences of knotValue in the knot vector
     */
    virtual uint multiplicityOfKnotValue(real knotValue) const;

    /** Multiplicity of knot at index knotValue
     *  Searches the knot vector for knots with same value as the one at given knotIndex.
     *  Searching is started from knotIndex in both directions.
     *  @param knotIndex index of knot whichs multiplicity is asked (knotIndex >= 0)
     *  @return multiplicity of knot at knotIndex
     */
    virtual uint multiplicity(uint knotIndex) const;

    /** Inserts knots and control points without changing the curve
     *  until between every two knots is a maximum of maxDist gap.
     *  With this knot refinement the control point polygon converges to the curve.
     *  Attention: This method really changes the spline curve!
     *  So make a copy if you think the old representation is worthwhile!
     *  @param maxDist the maximal distance in the evaluable knot vector between two knots
     *                 after the knot refinement
     */
    virtual void knotRefinement(real maxDist);

    /** Inserts a knot at the given knotValue and a suitable point without changing the curve.
     *  If the knot should have a multiplicity bigger than 1, give a suitable value
     *  (smallest multiplicity value is 1, largest is m_degree)
     *  Attention: This method does NOT check the validity of the curve, nor does it check
     *  if the given knotValue is a possible value. So make sure you check this first!
     *  @param knotValue a suitable new knotValue to insert
     *  @param multiplicity how often this new knotValue should be inserted (0 < multiplicity <= m_degree)
     */
    virtual void refineAt(real knotValue, uint multiplicity = 1);

    /** The spline curve can be expanded to form a closed curve.
     *   @param isClosed if true, curve will be expanded to a closed curve,
     *                   if false, periodicity will be removed //TODO
     */
    virtual void setClosedCurve(bool isClosed);

    /** Uses saved value to decide, if spline is or is not a closed curve.
     *  @note To update this state, call updateClosedCurve()
     *  @return true, if saved value (#m_isClosed) says that curve is closed
     */
    virtual bool isClosed() const;

    /** TODO!!!
     */
    virtual void updateClosedCurve();

    /** @brief Changes the knot vector at start and end.
     *
     *  If @p tearToEdges is @b true, clamps start and end to the first and last
     *  control points. This is possible with a specific choice of the knot vector.
     *  If the spline is @b valid, it sets the first and last n = m_degree + 1
     *  knot values each to the same value.
     *
     *  If @p tearToEdges is @b false, it replaces the first and last knots with evenly
     *  distributed values.
     *
     *  @warning Note that this method changes the knot values and therefore earlier
     *  chosen values will be lost!
     */
    virtual void setTornToEdges(bool tearToEdges);


    /** @brief Checks if the given #m_toothProfile is torn to edges or not.
     *
     *  Uses saved value (#m_tornToEdges) to decide, if spline is or is not
     *  torn to edges.
     *  @note to update this state, call updateTornToEdges()
     *  @return True if value saved value #m_tornToEdges is @c true.
     *  @see checkTornToEdges()
     *  @see updateTornToEdges()
     */
    virtual bool isTornToEdges() const;

    /** @brief Checks if the given #m_toothProfile is torn to edges or not.
     *
     *  Tests the knot vector to decide if the spline is torn to first and last
     *  control point. The decision is done on really examining the knot values and not
     *  the saved value.
     *  @return True if spline state is valid and belongs to a spline with clamped ends
     *  @see isTornToEdges()
     *  @see updateTornToEdges()
     */
    virtual bool checkTornToEdges() const;

    /** @brief Updates the torn to edges state of the spline
     *
     *  Uses checkTornToEdges() to decide, if the spline is torn to the edges
     *  and saves this information in the spline (in variable #m_tornToEdges)
     */
    virtual void updateTornToEdges();

    /** Checks the relation between degree, number of knots and number of control points.
     *  When there are too less control points for the current degree a spline curve
     *  can not be created.
     *  @return True if curve can be drawn.
     */
    virtual bool isValid() const;

    /** Examines the spline curve and writes any error messages to the given ostream.
     *  This method is other than @param isValid() in that it not only investigates the
     *  relation of degree and number of knots and control points, but also the values of
     *  the knot vector.
     *  @param os The stream the error messages are written to
     *  @return True if no error was found
     */
    virtual bool isValidMessages(std::ostream &os) const;

    /** Returns the lowest value, which can be evaluated.
     *  If spline is not valid in the current state it returns -1.0
     *  @return lowest possible value, which can be evaluated, or -1.0 if spline is not valid
     */
    virtual real lowerDomainLimit() const;

    /** Gives a limit for the highest value that can be evaluated.
     *  Attention: this value MUST NOT BE REACHED! It has to be a little
     *  bit smaller!
     *  It first checks the spline with isValid() and if it is not, it return -1.0
     *  @return limit for the highest value, which must not be reached when evaluating
     *  the spline or -1.0 if spline is not valid
     */
    virtual real upperDomainLimit() const;

    /** Returns the index of the knot in the knot array,
     *  whose value is smaller or equal to the given value.
     *  If there are more knots with this same small value,
     *  it returns the highest index.
     *  (If a value is given, which is lower than all current knots
     *  index 0 is returned.)
     */
    virtual uint lowerNextKnot(real value) const;

    /** @brief Returns the smallest distance of a control point to the origin
     *
     *  Searches the control points for the nearest one to the origin
     *  and returns the distance of this point to the origin
     *  @return smallest distance of a control point to the origin
     */
    virtual real minimumDistanceOfControlPointToOrigin() const;

    /** @brief Returns the largest distance of a control point to the origin
     *
     *  Searches the control points for the furthest one from to the origin
     *  and returns the distance of this point to the origin
     *  @return largest distance of a control point to the origin
     */
    virtual real maximumDistanceOfControlPointToOrigin() const;

    /** @brief Searches the spline for the nearest point to the origin
     *
     *  @return the distance to the nearest found point of the spline to the origin
     */
    virtual real minimumDistanceToOrigin() const;

    /** @brief Searches the spline for the furthest point to the origin
     *
     *  @return the distance to the furthest found point of the spline to the origin
     */
    virtual real maximumDistanceToOrigin() const;

    /** Cuts given ray with the spline curve and expands the vector intersectionPoints with
     *  found (approximated) intersection points. Already present values in the
     *  intersectionPoints vector will not be deleted!
     *  @param ray Ray which is searched for intersections with the spline curve
     *  @param intersectionPoints found intersection points are inserted in this vector.
     */
    virtual void getIntersectionPointsWithRay(const Ray &ray, vector<vec2> &intersectionPoints) const;

    /*///////////////TTTTTTTTTTTTTTTTTTTOOOOOOOOOOOOOOOOOOOOOODDDDDDDDDDDDDDDDDDDDDDOOOOOOOOOOOOOOOOO!!!!!!!!!!!!!!!!!!!
     */
    virtual void scale(real scaling, vec2 origin = vec2(0, 0));

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
     *  So check this first
     *  @see isValid()
     */
    void equalizeFirstKnots();

    /** Sets the last m_degree + 1 knots to the same value.
     *  More precisely: they are set to the value one more than at the previous knot
     *  ATTENTION: precondition for this method is that the spline is valid!
     *  So check this first
     *  @see isValid()
     */
    void equalizeLastKnots();

    /** Sets the first m_degree + 1 knots to different values,
     *  each one less than the one that follows.
     *  ATTENTION: precondition for this method is that the spline is valid!
     *  So check this first
     *  @see isValid()
     */
    void makeDifferentFirstKnots();

    /** Sets the last m_degree + 1 knots to different values,
     *  each one to one more than the previous one.
     *  ATTENTION: precondition for this method is that the spline is valid!
     *  So check this first
     *  @see isValid()
     */
    void makeDifferentLastKnots();

    /** @brief Searches the spline for points whose normales pass the origin.
     *
     *  Found points are necessary f.ex. for the calculation of the minimum and maximum
     *  distances of the curve to the origin.
     *  As resizing of the given @param samples vector should be prohibited, the samples
     *  vector must at least have the size of:
     *  2 x number of control points
     *  When a smaller samples vector is given, "-1" is returned.
     *  @param samples the points whose normals pass the origin are inserted in this given vector
     *  @return number of inserted points or '-1' when given samples vector is too small
     */
    int pointsWithNormalsThroughOrigin(vector<vec2> &samples, real epsilon = 0.00001) const;
    /** @brief Examines the normal of the spline at value @p evaluationValue
     *
     *  The sign passed in @p lastSign should contain the sign of the cross product of the
     *  normal with the vector to the point of the curve last evaluated.
     *
     *  If the normal in the given @p evaluationValue passes the origin (or nearly the origin
     *  with only a divergence smaller than given @p epsilon) the return value is '1' and
     *  the spline point at @p evaluationValue is entered in @p samples at position @p foundPoints.
     *  Also the @p foundPoints is increased by one and @p lastSign is set to '0'.
     *
     *  If the normal does not pass the origin but the sign saved and given in @p lastSign
     *  differs from the cross product (of normal and curve point) in the given @p evaluationValue
     *  '-1' is returned. This means, that the direction changed, but no point is found and
     *  therefore one should check a value between the last tested ones again.
     *
     *  If the normal does not pass the origin and the sign is the same as the one saved in
     *  @p lastSign, '0' is returned
     *
     *  @return  1 ==> point found and inserted in samples with foundPoints increased by 1
     *           0 ==> no point found, direction same as in last test
     *          -1 ==> no point found, but direction changed => Please check values between!
     */
    int testThroughOrigin(real evaluationValue, int &lastSign, int &foundPoints, vector<vec2> &samples, real epsilon) const;

};

#endif // SPLINE

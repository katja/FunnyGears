#ifndef SPLINE_GEAR
#define SPLINE_GEAR

#include "basic_objects/SplineToothProfile.h"
#include "definitions.h"

class SplineGear : public Spline {
public:
    // Given toothOfGear is used to build a gear "around" this tooth. If tooth is changed
    // call updateKnotsAndControlPoints() to update the gear curve.
    SplineGear(SplineToothProfile *toothOfGear);

    //Does NOT delete the Spline it is linked to!!!
    virtual ~SplineGear();

    /** @brief Updates important values for the gear.
     *
     *  @p m_degree is set to the degree of the current @p m_toothProfile.
     *  @p m_radius is only changed, if @p m_radius <= 0 and if isValid().
     *                It is then set to the middle value of toothOfGear.
     *  @p m_numberOfTeeth is only changed, if @p m_numberOfTeeth == 0 and if isValid().
     *                It is then set to maximumPossibleToothCount().
     *  If isValid() results in false, knots and control points are set back.
     *  Otherwise they are updated.
     */
    void update();

    /** @brief Gives information about the direction @p m_toothProfile is described in.
     *
     *  Here a coordinate system like that of screen coordinates is assumed.
     *  So the x-axis is pointing to the right, the @b y-axis points @b downward.
     *  Therefore "in clock direction" is a mathematical "counter clock direction"!
     *
     *  @return Returns true, if the current used @p m_toothProfile is described in clock direction,
     *  and false otherwise.
     */
    bool toothDescribedInClockDirection() const;

    /** @brief Returns the number of teeth
     *
     *  @return number of teeth
     */
    uint numberOfTeeth() const;

    /** @brief sets the number of teeth to @p numberOfTeeth
     *
     *  If given @p numberOfTeeth is @c 0, the number of teeth is set to maximumPossibleToothCount()
     *  @param numberOfTeeth new number of teeth
     */
    void setNumberOfTeeth(uint numberOfTeeth);

    /** @brief Returns the angular pitch of the gear (German: Teilungswinkel)
     *
     *  The angular pitch is quasi the angle needed for one tooth.
     *  Or more precisely: It is given by the angle in a transverse gear, formed by
     *  dividing a whole perifery of a circle in z equal parts, whereas z = <number of teeth>
     *  @return the angular pitch of a transverse gear
     */
    real angularPitch() const;

    /** @brief Returns the maximum distance to the gear center
     *
     *  @return maximum distance to gear center
     */
    real maximumDistanceToCenter() const;

    /** @brief Returns the minimum distance to the gear center
     *
     *  @return minimum distance to gear center
     */
    real minimumDistanceToCenter() const;

    /** @brief Returns the maximal possible tooth count
     *
     *  This maximal tooth count depends on the current used @p m_toothProfile.
     */
    uint maximumPossibleToothCount() const;

    /** @brief Returns a starting point of the first tooth
     *
     *  As the gear can have every shape, it is not possible to use a special gear point.
     *  So the first visible curve point of the @p m_toothProfile is used.
     *  @return starting point of the (first) tooth
     *  @warning test the spline for validness before!
     */
    vec2 startPointForTooth() const;

    /** @brief Recalculcates the knots and control points.
     *
     *  Calculates the knots and control points necessary to form a gear out of the used
     *  @p m_toothOfGear.
     *  @warning A precondition for this update is the current given validness!
     *  So only execute this method, if isValid() returns true!
     */
    void updateKnotsAndControlPoints();

    /** @brief Removes all knots and control points and deletes them */
    void setBackKnotsAndControlPoints();

    /** @brief Sets the degree of the @p m_toothProfile to @param degree
     *
     *  and updates the gear afterwards (update() is called). Therefor @p m_degree is
     *  changed accordingly.
     *  @param degree new degree for spline, must be greater than 0
     */
    void setDegree(uint degree) override; // from Spline

    /** @brief Appends a control point to the @p m_toothProfile
     *
     *  Then the gears knots and control points are updated accordingly.
     *
     *  @param point position of the new control point of the @p m_toothProfile
     */
    void addControlPoint(vec2 point) override; // from Spline

    /** @brief Appends a control point to the @p m_toothProfile
     *
     *  @see addControlPoint(vec2 point)
     */
    void addControlPoint(real x, real y) override; // from Spline

    /** @brief Removes control point at given index and all related control points.
     *
     *  The corresponding control point of @p m_toothProfile is removed and then
     *  updateKnotsAndControlPoints() is called. Therefore all related control points
     *  will be removed, too.
     *  @param index index of the control point to be removed
     */
    void removeControlPoint(uint index) override; // from Spline

    /** @brief Sets the position of the control point at index @p index to @newPosition
     *
     *  and updates the corresponding control points in the gear accordingly.
     *  If given index is out of range of the gear, it does nothing.
     *  @param index index of the control point which is set to newPosition (index >= 0)
     *  @param newPosition new position for control point with given index
     */
    void moveControlPoint(uint index, vec2 newPosition) override; // from Spline

    /** @brief Sets the position of the control point at index @p index to @newPosition.
     *
     *  overloaded method.
     *  @see moveControlPoint(uint index, vec2 new Position)
     */
    void moveControlPoint(uint index, QPointF newPosition) override; // from Spline

    /** @brief Gear is closed!
     *
     *  The gear curve is always a closed curve. Therefore this method does
     *  nothing!
     *   @param isClosed is ignored
     */
    void setClosedCurve(bool isClosed) override; // from Spline

    /** @brief Returns always true.
     *
     *  Gears are always closed. This can't be changed.
     *  @return true
     */
    bool isClosed() const override; // from Spline

    /** @brief Prevents updating behaviour from normal Spline
     */
    void updateClosedCurve() override; // from Spline

    /** @brief Changes the start and end of @p m_toothProfile.
     *
     *  If @p tearToEdges is true, @p m_toothProfile spline is set to clamp
     *  to first and last control point.
     *  If tearToEdges is false, it replaces the first and last knots of @p m_toothProfile
     *  with evenly distributed values.
     *  The gear is updated afterwards accordingly.
     *  @warning Note that this method changes the knot values and therefore earlier
     *  chosen values will be lost!
     */
    void setTornToEdges(bool tearToEdges) override; // from Spline


    /** @brief Checks if the given #m_toothProfile is torn to edges or not.
     *
     *  Uses saved value in #m_toothProfile to decide, if spline is or is not
     *  torn to edges.
     *  @note to update this state, call updateTornToEdges()
     *  @return True if value saved in @p m_toothProfile says, that it is torn to edges.
     *  @see checkTornToEdges()
     *  @see updateTornToEdges()
     */
    bool isTornToEdges() const override; // from Spline

    /** @brief Checks if the given #m_toothProfile is torn to edges or not.
     *
     *  Tests the knot vector of @p m_toothProfile to decide if the spline is torn to
     *  first and last control point. The decision is done on really examining the knot
     *  values and not the saved value.
     *  @return True if spline state is valid and belongs to a spline with clamped ends
     *  @see isTornToEdges()
     *  @see updateTornToEdges()
     */
    bool checkTornToEdges() const override; // from Spline

    /** @brief Updates the torn to edges state of given #m_toothProfile
     *
     *  Uses checkTornToEdges() to decide, if the spline #m_toothProfile is torn to the
     *  edges and saves this information (in #m_toothProfile).
     */
    void updateTornToEdges() override; // from Spline

    /** @brief Checks the validness of given #m_toothProfile
     *
     *  Checks the relation between degree, number of knots and number of control points.
     *  When there are too less control points for the current degree a spline curve
     *  can not be created.
     *  @return True if curve can be drawn.
     */
    bool isValid() const override; // from Spline

    /** @brief Turns the @p positionInFirstTooth around toothIndex teeth
     *
     *  Assumes that @p positionInFirstTooth lies in first tooth and turns it around
     *  the gear center, until it reaches the given @p toothIndex
     *  @param toothIndex target tooth index
     *  @param positionInFirstTooth an arbitrary point
     *  @return turned position point @p positionInFirstTooth
     */
    vec2 relatedPositionInTooth(uint toothIndex, vec2 positionInFirstTooth) const;

private:
    SplineToothProfile *m_toothProfile;
    real m_radius;
    uint m_numberOfTeeth;

    uint relatedIndexInFirstTooth(uint controlPointIndex) const;
    uint toothIndex(uint controlPointIndex) const;
    vec2 relatedPositionInFirstTooth(uint toothIndex, vec2 position) const;

};

#endif // SPLINE_GEAR

#ifndef SIMPLE_GEAR
#define SIMPLE_GEAR

#include "CylindricalGear.h"
#include "ToothProfile.h"

class SimpleGear : public CylindricalGear {

public:
    /** Simple Gear constructor which takes a copy of any class of a tooth profile
     *  The intern copy of the given ToothProfile must not be changed or intern values
     *  have to be set again!
     */
    SimpleGear(const ToothProfile &toothProfile);

    /** Simple Gear copy constructor */
    SimpleGear(const SimpleGear &other);

    virtual ~SimpleGear();

    /** Returns the momentarily set radius for this gear */
    real getRadius() const;

    /** Sets the radius of the gear to given @param radius, if this is possible
     *  If given @param radius is smaller than minRadius() or bigger than maxRadius()
     *  nothing is done!
     */
    void setRadius(real radius);

    /** Gives a bottom limit for a radius value */
    bool minRadius() const;

    /** Gives an upper limit for a radius value */
    bool maxRadius() const;

    real getRootRadius() const;

    real getTipRadius() const;

    uint getNumberOfTeeth() const;

    void setNumberOfTeeth(uint numberOfTeeth);

    real getAngularPitch() const; //purely virtual method of CylindricalGear

    //necessary:
    void getSampledProfileOfATooth(vector<vec2> &toothProfile) const; //purely virtual method of CylindricalGear

private:
    ToothProfile *m_toothProfile;
    real m_radius;
    uint m_numberOfTeeth;
};


#endif // SIMPLE_GEAR

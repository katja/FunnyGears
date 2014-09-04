#ifndef CYLINDRICAL_GEAR
#define CYLINDRICAL_GEAR

#include "definitions.h"
#include "helpers.h"
#include "stable.h"

/**
 * @class CylindricalGear
 * @brief Simplest Gear class
 *
 * This class is only for gears with finite radius. This simplifies
 * calculations and ensures f. ex. a well-defined rotational axis,
 * which lays on the center (0,0).
 *
 * As there are different possibilities to define a gear, like with
 * parameters (module, radius, number of teeth,...) or with point clouds,
 * this class holds only the few methods which all representations have.
 */
class CylindricalGear {

public:

    virtual real getAngularPitch() const = 0;
    virtual uint getNumberOfTeeth() const = 0;

    /**
     * Writes the points of a whole tooth profile of the gear
     * to the given toothProfile vector. Even the last point,
     * which must be the same one as the first one of the next tooth,
     * is included.
     */
    virtual void getSampledProfileOfATooth(vector<vec2> &toothProfile) const = 0;

    /**
     * Writes the points of the whole gear in the given gearProfile vector.
     * One has to bear in mind, that no two points may on each other - so
     * if one uses the getToothProfile() here, special care is needed for
     * the first and last points of the tooth profile.
     * If the gearProfile is not a multiple of the number of teeth of the
     * gear, this method won't change anything!
     * @param gearProfile given vec2 QVector whose length is a multiple
     *        of the number of teeth
     */
    virtual void getGearProfile(vector<vec2> &gearProfile) const;

};

#endif // CYLINDRICAL_GEAR

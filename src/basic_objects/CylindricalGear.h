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
     * Writes the points of one tooth profile of the gear
     * to the given toothProfile vector.
     */
    virtual void toothProfile(vector<vec2> &toothProfile) const = 0;

    /**
     * Writes the points of the whole gear in the given gearProfile vector.
     * If no special method is used in subclasses, this works on the toothProfile()
     * method to construct a whole gear profile.
     * Often it would be better to write an own implementation.
     *
     * If the gearProfile's size is not a multiple of the number of teeth of the
     * gear, this method won't do anything!
     * @param gearProfile given vec2 vector whose length is a multiple
     *        of the number of teeth
     */
    virtual void gearProfile(vector<vec2> &gearProfile) const;

};

#endif // CYLINDRICAL_GEAR

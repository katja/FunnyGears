#ifndef TOOTH_PROFILE
#define TOOTH_PROFILE

class ToothProfile {

    public:
        virtual ~ToothProfile() {}
        virtual ToothProfile* create() const = 0;
        virtual ToothProfile* clone() const = 0;

        virtual vec2 start() const = 0;
        virtual vec2 stop() const = 0;
        virtual real minimumDistanceToOrigin() const = 0;
        virtual real maximumDistanceToOrigin() const = 0;
        virtual void sampledCurve(vector<vec2> &samples) const = 0;

};

#endif // TOOTH_PROFILE

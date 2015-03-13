#include "basic_objects/GearPairInformation.h"
#include "basic_objects/GearPair.h"

GearPairInformation::GearPairInformation(GearPair *gearPair) : m_gearPair(gearPair) {
    setBackAttributes();
}

GearPairInformation::~GearPairInformation() {
}

void GearPairInformation::setBackAttributes() {
    m_gearShapeValid = true;
    changed();
}

void GearPairInformation::notValidGearShapeFound() {
    m_gearShapeValid = false;
    changed();
}

// void GearPairInformation::validGearShapeInfoText() const {
// Dieses Programm betrachtet nur außenverzahnte Stirnräder. Wird eine Kurve als Zahn gewählt,
// die Schnittpunkte enthält oder eine zu hohe Zähnezahl gewählt, so dass das Treibrad Schnittpunkte
// aufweist, kann keine korrekte Verzahnung berechnet werden.
// Verbesserung: Schnittpunkte entfernen!
// }

void GearPairInformation::matingGearOutlineConstructionFailed() {
    m_gearConstructionValid = false;
    changed();
}

real GearPairInformation::approximatePercentageOfInContactPoints() const {
    //TODO !!!
    //maybe: give a relationship between m_gearCPs size and m_gearWCPs size of m_gearPair foundPoints() ????
    // uint notCorrect = m_gearPair->foundPoints().numberOfNotCorrectContactPoints();
    // uint correct = m_gearPair->foundPoints().gearContactPoints().size();
    // return 100.0 * static_cast<real>(correct) / static_cast<real>(notCorrect + correct);
    return 0.0;
}

bool GearPairInformation::validGearShapeOfDrivingGear() const {
    return m_gearShapeValid;
}

void GearPairInformation::objectChanged(ChangingObject *object) {
    //TODO
}

void GearPairInformation::informAboutChange(ChangingObjectListener *listener) {
    m_listeners.push_back(listener);
}

void GearPairInformation::noMoreInformAboutChange(ChangingObjectListener *listener) {
    m_listeners.remove(listener);
}

void GearPairInformation::changed() {
    for(ChangingObjectListener *listener : m_listeners) {
        listener->objectChanged(this);
    }
}

#include "Parser.h"
#include <string>

std::ostream& operator<<(std::ostream &os, const QChar &c) {
    return os << (unsigned char)c.unicode();
}

void print(std::string s) {
    std::cout << s << std::endl;
}

bool Parser::applyModelWith(const QString &savedFGProjectText, Model *model) const {
    if(model == nullptr)
        return false;
    QString::const_iterator startScope = savedFGProjectText.cbegin();
    QString::const_iterator endScope = savedFGProjectText.cend();
    QString::const_iterator nextScope = QString::const_iterator(endScope);

    QString symbol = findNextSymbol(startScope, endScope, nextScope);
    if(symbol != "funny_gears_project")
        return false;
    return parseFunnyGearsProject(model, startScope, endScope);
}

//Handles text INSIDE <funny_gears_project> scope
//funny_gears_project := *graphics_schedule_item
bool Parser::parseFunnyGearsProject(
    Model *model,
    const QString::const_iterator &startScope,
    const QString::const_iterator &endScope) const
{
    QString::const_iterator startItemScope = startScope;
    QString::const_iterator nextItemScope = startScope;
    QString::const_iterator endItemScope = endScope;
    while(!nothingLeftBetween(nextItemScope, endScope)) {
        GraphicsScheduleItem *item = nullptr;
        QString symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
        if(symbol != "graphics_schedule_item")
            return false;
        try {
            item = parseGraphicsScheduleItem(startItemScope, endItemScope);
        } catch(QString e) {
            std::cerr << "Error when parsing scope of graphics_schedule_item::" << e.toStdString() << std::endl;
            return false;
        }
        model->addItem(item);
        print("Item added to model: " + item->name().toStdString());
        startItemScope = nextItemScope;
        endItemScope = endScope;
    }
    return true;
}

// Handles text INSIDE <graphics_schedule_item> scope
// graphics_schedule_item:
//     1. graphics_spline | graphics_spline_gear | graphics_gear_pair
//     2. name
//     3. visibility
GraphicsScheduleItem* Parser::parseGraphicsScheduleItem(
    const QString::const_iterator &startScope,
    const QString::const_iterator &endScope) const
{
    QString::const_iterator startItemScope = startScope;
    QString::const_iterator nextItemScope = startScope;
    QString::const_iterator endItemScope = endScope;

    //test for graphics_spline | graphics_spline_gear | graphics_gear_pair
    GraphicsScheduleItem *graphicsScheduleItem = nullptr;

    QString symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
    if(symbol == "graphics_spline") {
        try {
            graphicsScheduleItem = parseGraphicsSpline(startItemScope, endItemScope);
        } catch(QString e) {
            throw e.prepend("graphics_spline::"); return graphicsScheduleItem;
        }
    } else
    if(symbol == "graphics_spline_gear") {
        try {
            graphicsScheduleItem = parseGraphicsSplineGear(startItemScope, endItemScope);
        } catch(QString e) {
            throw e.prepend("graphics_spline_gear::"); return graphicsScheduleItem;
        }
    } else
    if(symbol == "graphics_gear_pair") {
        try {
            graphicsScheduleItem = parseGraphicsGearPair(startItemScope, endItemScope);
        } catch(QString e) {
            throw e.prepend("graphics_gear_pair::"); return graphicsScheduleItem;
        }
    } else {
        throw QString(" kind of graphic schedule item not recognized"); return nullptr;
    }

    //test for name
    startItemScope = nextItemScope;
    endItemScope = endScope;
    symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
    if(symbol != "name") {
        throw QString(" name is missing"); return nullptr;
    }
    graphicsScheduleItem->setName(innerText(startItemScope, endItemScope));

    //test for visibility
    startItemScope = nextItemScope;
    endItemScope = endScope;
    symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
    if(symbol != "visibility") {
        throw QString(" visibility is missing"); return nullptr;
    }
    if(!parseBool(startItemScope, endItemScope))
        graphicsScheduleItem->toggleVisibility();

    return graphicsScheduleItem;
}

// Handles text INSIDE <graphics_spline> scope
// graphics_spline:
//     1. spline
//     |- (color)
//     |  (is_control_polygon_visible)
//     |  (is_tangent_drawn)
//     |_ (tangent_value)
GraphicsSpline* Parser::parseGraphicsSpline(
    const QString::const_iterator &startScope,
    const QString::const_iterator &endScope) const
{
    QString::const_iterator startItemScope = startScope;
    QString::const_iterator nextItemScope = startScope;
    QString::const_iterator endItemScope = endScope;

    //test for spline
    QString symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
    if(symbol != "spline") {
        throw QString(" spline missing"); return nullptr;
    }
    Spline *spline = nullptr;
    try {
        spline = parseSpline(startItemScope, endItemScope);
    } catch(QString e) {
        throw e.prepend("spline::"); return nullptr;
    }
    GraphicsSpline *graphicsSpline = new GraphicsSpline(spline);

    //test for graphics spline properties
    while(!nothingLeftBetween(nextItemScope, endScope)) {
        startItemScope = nextItemScope;
        endItemScope = endScope;
        symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
        QString value = innerText(startItemScope, endItemScope);
        if(symbol == "color") {
            QColor color;
            try {
                color = parseColor(startItemScope, endItemScope);
            } catch(QString e) {
                throw e.prepend("color::"); return nullptr;
            }
            graphicsSpline->setColor(color);
        } else
        if(symbol == "is_control_polygon_visible") {
            graphicsSpline->setVisibleControlPolygon(parseBool(value));
        } else
        if(symbol == "is_tangent_drawn") {
            graphicsSpline->setTangentDrawn(parseBool(value));
        } else
        if(symbol == "tangent_value") {
            try {
                graphicsSpline->setTangentValue(parseReal(value));
            } catch(QString e) {
                throw e.prepend("tangent_value::"); return nullptr;
            }
        } else {
            throw QString(" property not recognized: ").append(symbol); return nullptr;
        }
    }
    return graphicsSpline;
}

// Handles text INSIDE <graphics_spline_gear> scope
// graphics_spline_gear:
//     1. spline_gear
//     |- (is_base_pitch_visible)
//     |  (is_reference_circle_visible)
//     |  (is_rotating)
//     |_ (rotation_velocity)
GraphicsSplineGear* Parser::parseGraphicsSplineGear(
    const QString::const_iterator &startScope,
    const QString::const_iterator &endScope) const
{
    QString::const_iterator startItemScope = startScope;
    QString::const_iterator nextItemScope = startScope;
    QString::const_iterator endItemScope = endScope;

    //test for spline gear
    QString symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
    if(symbol != "spline_gear") {
        throw QString(" spline gear missing"); return nullptr;
    }
    SplineGear *splineGear = nullptr;
    try {
        splineGear = parseSplineGear(startItemScope, endItemScope);
    } catch (QString e) {
        throw e.prepend("spline_gear::"); return nullptr;
    }
    GraphicsSplineGear *graphicsSplineGear = new GraphicsSplineGear(splineGear);

    //test for spline gear properties
    while(!nothingLeftBetween(nextItemScope, endScope)) {
        QString::const_iterator startItemScope = nextItemScope;
        QString::const_iterator endItemScope = endScope;
        symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
        QString value = innerText(startItemScope, endItemScope);
        if(symbol == "is_base_pitch_visible") {
            graphicsSplineGear->setBasePitchVisibility(parseBool(value));
        } else
        if(symbol == "is_reference_circle_visible") {
            graphicsSplineGear->setReferenceCircleVisibility(parseBool(value));
        } else
        if(symbol == "is_rotating") {
            graphicsSplineGear->setRotating(parseBool(value));
        } else
        if(symbol == "rotation_velocity") {
            try {
                graphicsSplineGear->setRotationVelocity(parseReal(value));
            } catch (QString e) {
                throw e.prepend("rotation_velocity::");
            }
        } else {
            throw QString(" property not recognized: ").append(symbol); return nullptr;
        }
    }
    return graphicsSplineGear;
}

// Handles text INSIDE <graphics_spline> scope
// graphics_gear_pair:
//     - gear_pair
//     - (is_pitch_point_visible)
//     - (is_driving_gear_sampling_visible)
//     - (is_driven_gear_sampling_visible)
//     - (is_forbidden_area_in_driving_gear_visible)
//     - (is_forbidden_are_in_driven_gear_visible)
//     - (are_none_contact_points_visible)
//     - (is_selection_of_gear_points_visible)
//     - (is_path_of_possible_contact_visible)
//     - (is_path_of_real_contact_visible)
//     - (is_path_of_best_contact_visible)
//     - (are_pitches_visible)
//     - (are_pitch_circles_visible)
//     - (is_rotating)
//     - (rotation_velocity)
//     - (fine_pencil_used)
GraphicsGearPair* Parser::parseGraphicsGearPair(
    const QString::const_iterator &startScope,
    const QString::const_iterator &endScope) const
{
    QString::const_iterator startItemScope = startScope;
    QString::const_iterator nextItemScope = startScope;
    QString::const_iterator endItemScope = endScope;

    //test for gear pair
    QString symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
    if(symbol != "gear_pair") {
        throw QString(" gear pair missing"); return nullptr;
    }
    GearPair *gearPair = nullptr;
    try {
        gearPair = parseGearPair(startItemScope, endItemScope);
    } catch(QString e) {
        throw e.prepend("gear_pair::"); return nullptr;
    }

    GraphicsGearPair *graphicsGearPair = new GraphicsGearPair(gearPair);

    //test for graphics gear pair properties
    while(!nothingLeftBetween(nextItemScope, endScope)) {
        startItemScope = nextItemScope;
        endItemScope = endScope;
        symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
        QString value = innerText(startItemScope, endItemScope);
        if(symbol == "is_pitch_point_visible") {
            graphicsGearPair->setVisibilityOfPitchPoint(parseBool(value));
        } else
        if(symbol == "is_driving_gear_sampling_visible") {
            graphicsGearPair->setVisibilityOfDrivingGearSampling(parseBool(value));
        } else
        if(symbol == "is_driven_gear_sampling_visible") {
            graphicsGearPair->setVisibilityOfDrivenGearSampling(parseBool(value));
        } else
        if(symbol == "is_forbidden_area_in_driving_gear_visible") {
            graphicsGearPair->setVisibilityOfForbiddenAreaInDrivingGear(parseBool(value));
        } else
        if(symbol == "is_forbidden_are_in_driven_gear_visible") {
            graphicsGearPair->setVisibilityOfForbiddenAreaInDrivenGear(parseBool(value));
        } else
        if(symbol == "are_none_contact_points_visible") {
            graphicsGearPair->setVisibilityOfNoneContactPoints(parseBool(value));
        } else
        if(symbol == "is_selection_of_gear_points_visible") {
            graphicsGearPair->setVisibilityOfSelectedGearPoints(parseBool(value));
        } else
        if(symbol == "is_path_of_possible_contact_visible") {
            graphicsGearPair->setVisibilityOfPathOfPossibleContact(parseBool(value));
        } else
        if(symbol == "is_path_of_real_contact_visible") {
            graphicsGearPair->setVisibilityOfPathOfRealContact(parseBool(value));
        } else
        if(symbol == "is_path_of_best_contact_visible") {
            graphicsGearPair->setVisibilityOfPathOfBestContact(parseBool(value));
        } else
        if(symbol == "are_pitches_visible") {
            graphicsGearPair->setVisibilityOfPitches(parseBool(value));
        } else
        if(symbol == "are_pitch_circles_visible") {
            graphicsGearPair->setVisibilityOfPitchCircles(parseBool(value));
        } else
        if(symbol == "is_rotating") {
            graphicsGearPair->setRotating(parseBool(value));
        } else
        if(symbol == "rotation_velocity") {
            try {
                graphicsGearPair->setRotationVelocity(parseReal(value));
            } catch(QString e) {
                throw e.prepend("rotation_velocity::"); return nullptr;
            }
        } else
        if(symbol == "fine_pencil_used") {
            graphicsGearPair->setFinePencil(parseBool(value));
        } else {
            throw QString(" property not recognized: ").append(symbol);
        }
    }
    return graphicsGearPair;
}

// Handles text INSIDE <spline> scope
// spline:
//     1. knots
//     2. control_points
Spline* Parser::parseSpline(
    const QString::const_iterator &startScope,
    const QString::const_iterator &endScope) const
{
    QString::const_iterator startItemScope = startScope;
    QString::const_iterator nextItemScope = startScope;
    QString::const_iterator endItemScope = endScope;

    //test for knots
    QString symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
    if(symbol != "knots") {
        throw QString(" knots missing"); return nullptr;
    }
    vector<real> knots;
    while(!nothingLeftBetween(startItemScope, endItemScope)) {
        try {
            real value = parseReal(stepToNextSpace(startItemScope, endItemScope));
            knots.push_back(value);
            std::cout << "knot inserted: " << value << std::endl;
        } catch(QString e) {
            throw e.prepend("knots::"); return nullptr;
        }
    }

    //test for control_points
    startItemScope = nextItemScope;
    endItemScope = endScope;
    symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
    if(symbol != "control_points") {
        throw QString(" control points missing"); return nullptr;
    }
    vector<vec2> controlPoints;
    try {
        controlPoints = parseControlPoints(startItemScope, endItemScope);
    } catch(QString e) {
        throw e.prepend("controlPoints::"); return nullptr;
    }

    if(!nothingLeftBetween(nextItemScope, endScope))
        std::cerr << "ATTENTION: spline allows only knots and control points!" << std::endl;

    Spline *spline = new Spline(controlPoints, knots);
    std::cout << "Spline created:" << *spline << std::endl;
    return spline;

    // return new Spline(controlPoints, knots);
}

// Handles text INSIDE <spline_gear> scope
// spline_gear:
//     1. spline/tooth_profile
//     2. number_of_teeth
SplineGear* Parser::parseSplineGear(
    const QString::const_iterator &startScope,
    const QString::const_iterator &endScope) const
{
    QString::const_iterator startItemScope = startScope;
    QString::const_iterator nextItemScope = startScope;
    QString::const_iterator endItemScope = endScope;

    //test for tooth_profile (=spline)
    QString symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
    if(symbol != "spline" && symbol != "tooth_profile") {
        throw QString(" tooth profile (spline) missing"); return nullptr;
    }
    Spline *toothProfile = nullptr;

    std::cerr << "parse spline now for tooth_profile" << std::endl;
    try {
        toothProfile = parseSpline(startItemScope, endItemScope);
    } catch(QString e) {
        throw e.prepend("tooth_profile::"); return nullptr;
    }
    std::cerr << "                     tooth_profile created" << std::endl;
    std::cerr << "create SplineGear with tooth_profile" << std::endl;
    SplineGear *splineGear = new SplineGear(*toothProfile);
    std::cerr << "                     SplineGear created" << std::endl;
    delete toothProfile;
    toothProfile = nullptr;

    std::cerr << "set properties of splineGear:" << std::endl;
    //test for number_of_teeth
    startItemScope = nextItemScope;
    endItemScope = endScope;
    symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
    if(symbol != "number_of_teeth") {
        throw QString(" number of teeth is missing"); return nullptr;
    }
    try {
        splineGear->setNumberOfTeeth(parseUInt(innerText(startItemScope, endItemScope)));
    } catch (QString e) {
        throw e.prepend("number_of_teeth::");
    }

    if(!nothingLeftBetween(nextItemScope, endScope))
        std::cerr << "ATTENTION: spline_gear is based only on a spline (tooth_profile) and the number_of_teeth!" << std::endl;

    return splineGear;
}

// Handles text INSIDE <gear_pair> scope
// gear_pair:
//     - spline_gear (driving_gear)
//     - (driven_gear_number_of_teeth)
//     - (use_bottom_clearance)
//     - (bottom_clearance)
//     - (bottom_clearance_start_angle)
//     - (max_drift_angle)
//     - (sampling_rate)
GearPair* Parser::parseGearPair(
    const QString::const_iterator &startScope,
    const QString::const_iterator &endScope) const
{
    QString::const_iterator startItemScope = startScope;
    QString::const_iterator nextItemScope = startScope;
    QString::const_iterator endItemScope = endScope;

    //test for driving_gear (=spline_gear)
    QString symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
    if(symbol != "driving_gear" && symbol != "spline_gear") {
        throw QString(" driving gear (spline gear) missing"); return nullptr;
    }
    SplineGear *drivingGear = nullptr;
    try {
        drivingGear = parseSplineGear(startItemScope, endItemScope);
    } catch(QString e) {
        throw e.prepend("driving_gear::"); return nullptr;
    }
    GearPair *gearPair = new GearPair(*drivingGear);
    delete drivingGear;
    drivingGear = nullptr;

    //test for gear pair properties
    while(!nothingLeftBetween(nextItemScope, endScope)) {
        startItemScope = nextItemScope;
        endItemScope = endScope;
        symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
        QString value = innerText(startItemScope, endItemScope);
        if(symbol == "driven_gear_number_of_teeth") {
            try {
                gearPair->setNumberOfTeethOfDrivenGear(parseUInt(value));
            } catch(QString e) {
                throw e.prepend("driven_gear_number_of_teeth::"); return nullptr;
            }
        } else
        if(symbol == "use_bottom_clearance") {
            gearPair->useBottomClearance(parseBool(value));
        } else
        if(symbol == "bottom_clearance") {
            real bottomClearance;
            try {
                bottomClearance = parseReal(value);
            } catch(QString e) {
                throw e.prepend("bottom_clearance::"); return nullptr;
            }
            gearPair->setBottomClearance(bottomClearance, gearPair->bottomClearanceStartAngle());
        } else
        if(symbol == "bottom_clearance_start_angle") {
            real bottomClearanceStartAngle;
            try {
                bottomClearanceStartAngle = parseReal(value);
            } catch(QString e) {
                throw e.prepend("bottom_clearance_start_angle::"); return nullptr;
            }
            gearPair->setBottomClearance(gearPair->bottomClearance(), bottomClearanceStartAngle);
        } else
        if(symbol == "max_drift_angle") {
            try {
                gearPair->setMaxDriftAngle(parseReal(value));
            } catch(QString e) {
                throw e.prepend("max_drift_angle::"); return nullptr;
            }
        } else
        if(symbol == "sampling_rate") {
            try {
                gearPair->setSamplingRate(parseUInt(value));
            } catch(QString e) {
                throw e.prepend("sampling_rate::"); return nullptr;
            }
        } else {
            throw QString(" property not recognized: ").append(symbol); return nullptr;
        }
    }
    return gearPair;
}

// Handles text INSIDE <control_points> scope
// control_points:
//     - (point)
//     - (point)
//     - ...
vector<vec2> Parser::parseControlPoints(
    const QString::const_iterator &startScope,
    const QString::const_iterator &endScope) const
{
    QString::const_iterator startItemScope = startScope;
    QString::const_iterator nextItemScope = startScope;
    QString::const_iterator endItemScope = endScope;

    vector<vec2> controlPoints;
    while(!nothingLeftBetween(nextItemScope, endScope)) {
        QString symbol = findNextSymbol(startItemScope, endItemScope, nextItemScope);
        if(symbol != "point") {
            throw QString(" only points allowed"); return controlPoints;
        }
        vec2 point;
        for(int i = 0; i < 2; ++i) {
            if(nothingLeftBetween(startItemScope, endItemScope)) {
                throw QString(" points need 2 entries"); return controlPoints;
            }
            real value;
            try {
                value = parseReal(stepToNextSpace(startItemScope, endItemScope));
            } catch(QString e) {
                throw e.prepend("point::"); return controlPoints;
            }
            if(i == 0)
                point.x = value;
            else
                point.y = value;
        }
        controlPoints.push_back(point);

        startItemScope = nextItemScope;
        endItemScope = endScope;
    }
    return controlPoints;
}

//Only a "y" or a "y" with spaces around results in true
//Anything else is treated as false
bool Parser::parseBool(
    const QString::const_iterator &startScope,
    const QString::const_iterator &endScope) const
{
    return (innerText(startScope, endScope).trimmed() == "y");
}
bool Parser::parseBool(const QString &text) const {
    return text.trimmed() == "y";
}

//Uses Qts parsing from QString to uint
uint Parser::parseUInt(const QString &text) const {
    bool ok;
    uint value = text.toUInt(&ok);
    if(!ok)
        throw QString(" uint not recognized");
    return value;
}

//Uses Qts parsing from QString to double
//Allows "+" and "-", numerical characters, a decimal point and "e"/"g"
real Parser::parseReal(const QString &text) const
{
    bool ok;
    std::cout << "text: " << text.toStdString() << ", ---> " << text.toDouble();
    real value = text.toDouble(&ok);
    std::cout << " =? " << value << std::endl;
    if(!ok)
        throw QString(" real not recognized");
    return value;
}

QColor Parser::parseColor(
    const QString::const_iterator &startScope,
    const QString::const_iterator &endScope) const
{
    QString::const_iterator it = startScope;
    QColor color;
    for(int i = 0; i < 3; ++i) {
        if(nothingLeftBetween(it, endScope)){
            throw QString(" color has wrong format"); return color;
        }
        uint value;
        try {
            value = parseUInt(stepToNextSpace(it, endScope));
            if(value >=256)
                throw QString(" color value must be in [0, 255]");
        } catch(QString e) {
            throw e.prepend("color::"); return color;
        }
        if(i == 0)
            color.setRed(value);
        else if(i == 1)
            color.setGreen(value);
        else
            color.setBlue(value);
    }
    if(!nothingLeftBetween(it, endScope))
        std::cerr << "ATTENTION: only first 3 entries of color are passed!" << std::endl;
    return color;
}

// Steps with fromIterator to the next space behind the next text
// in [fromIterator, endScope).
// Returns the text, the iterator had to step over to reach this space sign,
// without leading whitespaces
// If no text is between fromIterator and endScope, an empty QString() is returned
QString Parser::stepToNextSpace(
    QString::const_iterator &fromIterator,
    const QString::const_iterator &endScope) const
{
    while(fromIterator < endScope && fromIterator->isSpace())
        ++fromIterator;
    QString steppedOverText;
    while(fromIterator < endScope && !fromIterator->isSpace()) {
        steppedOverText.append(*fromIterator);
        ++fromIterator;
    }
    return steppedOverText;
}

/** Searches for the next occurence of a symbol – like "<visibility>" or "</visibility>" – and returns
 *  the symbol name. So it would f. ex. return "visibility" when found <visibility>.
 *  If endScope is reached while scanning for the next symbol, an empty QString is returned.
 */
QString Parser::findNextSymbol(
    QString::const_iterator &startScope,
    QString::const_iterator &endScope,
    QString::const_iterator &nextScope) const
{
    bool output = false;
    if(output) print("findNextSymbol called...");

    //use yet another const_iterator for the iteration from startScope to endScope
    QString::const_iterator iter = startScope;

    //find start of next symbol
    if(output) print("find start of next symbol...");
    while(iter != endScope && *iter != QString("<")) { //QString::compare((*iter), tr("<"), Qt::CaseInsensitive) != 0) {
        ++iter;
    }
    if(output) print("...found start of next symbol: " + ((iter == endScope) ? "_end_" : iter->decomposition().toStdString()));
    if(iter == endScope)
        return QString();

    //read symbol name
    if(output) print("read symbol name...");
    ++iter;
    QString symbol;
    while(iter != endScope && *iter != QString(">")) {
        symbol += *iter;
        ++iter;
    }
    if(output) print("...read symbol name: " + ((iter == endScope) ? "_reached_end_" : symbol.toStdString()));
    if(iter == endScope)
        return QString();

    //set new startScope (points to first place after closing the symbol)
    startScope = iter + 1;

    //construct closing symbol
    if(output) print("construct closing symbol...");
    QString closingSymbol = QString("</" + symbol + ">");
    if(output) print("...constructed closing symbol:" + closingSymbol.toStdString());

    //find closing symbol
    if(output) print("find closing symbol...");
    int distToNextOccurence = nextOccurenceOf(closingSymbol, iter, endScope);
    iter = iter + distToNextOccurence + closingSymbol.length();
    if(distToNextOccurence < 0) {
        //set a symbol for parsing error:
        std::cerr << "PARSING ERROR – found only one occurence of " << symbol.toStdString() << std::endl;
        return QString();
    } else if(iter > endScope) {
        //set a symbol for parsing error:
        std::cerr << "PARSING ERROR – found only one occurence of '" << symbol.toStdString() << "' in inspected range" << std::endl;
        return QString();
    }
    if(output) print("... found closing symbol");

    //set new endScope (points to first place of closing the stop symbol ("<"))
    //and new nextScope (points to first place after the current symbols sphere of influence)
    //ATTENTION: nextScope may point anywhere behind the savedFGProjectText!!!
    endScope = iter - closingSymbol.length();
    nextScope = iter;

    if(output) print("found inner text:");
    if(output) print((innerText(startScope, endScope)).toStdString());
    if(output) print("...findNextSymbol finished\n");

    return symbol;
}

//Searches for occurences of 'text' in the range of [startIterator ... endIterator)
int Parser::nextOccurenceOf(const QString &text, const QString::const_iterator &startIterator, const QString::const_iterator &endIterator) const {
    if(text.isEmpty())
        return -1;
    QString::const_iterator it = startIterator;
    QString::const_iterator stopIt = endIterator - text.length();
    int i = 0;
    while(it <= stopIt) {
        if(startsWith(text, it))
            return i;
        ++i; ++it;
    }
    return -1;
}

//Requires the possibility that const_iterator 'it' can step forward at least text.length()
//times without reaching the end of its QString
bool Parser::startsWith(const QString &text, const QString::const_iterator &it) const {
    for(int i = 0; i < text.length(); ++i) {
        if(text.at(i) != *(it + i))
            return false;
    }
    return true;
}

bool Parser::nothingLeftBetween(const QString::const_iterator &a, const QString::const_iterator &b) const {
    if(a >= b)
        return true;
    QString::const_iterator aCopy = a;
    while(aCopy < b) {
        if(!aCopy->isSpace())
            return false;
        ++aCopy;
    }
    return true;
}

QString Parser::innerText(const QString::const_iterator &a, const QString::const_iterator &b) const {
    QString inner;
    for(QString::const_iterator iter = a; iter < b; ++iter) {
        inner.append(*iter);
    }
    return inner;
}

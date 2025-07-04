#include "customgpswaypointrecoder.h"
#include "MultiVehicleManager.h"
#include "Vehicle.h"
#include "JsonHelper.h"
#include "QGroundControlQmlGlobal.h"

CustomGPSWaypointRecoder::CustomGPSWaypointRecoder(QGCToolbox* toolbox, QObject *parent) :
    QObject(parent),
    _toolbox(toolbox),
    _isRecording(false),
    _minimiumDistance(1) // default distance is 1 meters
{

}

// stop recording, disconnect Vehicle::coordinateChanged to CustomGPSWaypointRecoder::onPointAdded
// add current coordinate to waypoint list as last waypoint
void CustomGPSWaypointRecoder::stopRecording()
{
    auto activeVehicle = _toolbox->multiVehicleManager()->activeVehicle();
    _waypoints.append(activeVehicle->coordinate());
    disconnect(_recordConnection);
    _isRecording = false;
    emit isRecordingChanged();
}

// Save waypoints as mission file
void CustomGPSWaypointRecoder::saveMission(QUrl filePath)
{
    constexpr int kPlanFileVersion = 1;
    const char* kPlanFileType = "Plan";
    const char* kJsonMissionObjectKey = "mission";
    const char* kJsonGeoFenceObjectKey = "geoFence";
    const char* kJsonRallyPointsObjectKey = "rallyPoints";

    QJsonObject planJson;
    QJsonObject missionJson;
    QJsonObject fenceJson;
    QJsonObject rallyJson;
    JsonHelper::saveQGCJsonFileHeader(planJson, kPlanFileType, kPlanFileVersion);
    fenceJson = QJsonObject();
    fenceJson["circles"] = QJsonArray();
    fenceJson["polygons"] = QJsonArray();
    fenceJson["version"] = 2;
    rallyJson = QJsonObject();
    rallyJson["points"] = QJsonArray();
    rallyJson["version"] = 2;

    auto activeVehicle = _toolbox->multiVehicleManager()->activeVehicle();
    QJsonValue coordinateValue;
    JsonHelper::saveGeoCoordinate(activeVehicle->homePosition(), true /* writeAltitude */, coordinateValue);
    missionJson["plannedHomePosition"] = coordinateValue;
    missionJson["firmwareType"] = activeVehicle->firmwareType();
    missionJson["vehicleType"] = activeVehicle->vehicleType();
    missionJson["cruiseSpeed"] = activeVehicle->defaultCruiseSpeed();
    missionJson["hoverSpeed"] = activeVehicle->defaultHoverSpeed();
    missionJson["globalPlanAltitudeMode"] = QGroundControlQmlGlobal::AltitudeModeRelative;

    QJsonArray items;
    for (int i = 0; i < _waypoints.size(); ++i) {
        QJsonObject o;
        o["AMSLAltAboveTerrain"] = QJsonValue::Null;
        o["Altitude"] = _waypoints[i].altitude();
        o["AltitudeMode"] = QGroundControlQmlGlobal::AltitudeModeRelative;
        o["autoContinue"] = true;
        o["command"] = MAV_CMD::MAV_CMD_NAV_WAYPOINT;
        o["doJumpId"] = i + 1;
        o["frame"] = MAV_FRAME::MAV_FRAME_GLOBAL_RELATIVE_ALT;
        o["type"] = "SimpleItem";
        QJsonArray params;
        params.push_back(0);
        params.push_back(0);
        params.push_back(0);
        params.push_back(QJsonValue::Null);
        params.push_back(_waypoints[i].latitude());
        params.push_back(_waypoints[i].longitude());
        params.push_back(_waypoints[i].altitude());
        o["params"] = params;
        items.push_back(o);
    }
    missionJson["items"] = items;
    planJson[kJsonMissionObjectKey] = missionJson;
    planJson[kJsonGeoFenceObjectKey] = fenceJson;
    planJson[kJsonRallyPointsObjectKey] = rallyJson;
    QFile file(filePath.path());
    if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
        return;

    file.write(QJsonDocument(planJson).toJson());
    file.close();
}

// when vehicle coordinate is updated, check if the distance to last waypoint int waypoints list
// is large then minimium distance. If yes, add coordinate to waypoints list
void CustomGPSWaypointRecoder::onPointAdded(QGeoCoordinate cor)
{
    if (cor.isValid() && cor.distanceTo(_waypoints.last()) >= _minimiumDistance) {
        _waypoints.push_back(cor);
//        qDebug() << "latitude: " << cor.latitude() << "longitude: " << cor.longitude()
//                 << "altitude: " << cor.altitude() << "isValid: " << cor.isValid();
    }
}

// minimium distance between waypoints, controlled by qml slider
qreal CustomGPSWaypointRecoder::minimiumDistance() const
{
    return _minimiumDistance;
}

void CustomGPSWaypointRecoder::setMinimiumDistance(const qreal &minimiumDistance)
{
    _minimiumDistance = minimiumDistance;
    emit minimiumDistanceChanged();
}

// start recording, connect Vehicle::coordinateChanged to CustomGPSWaypointRecoder::onPointAdded
// so we can record coordinate when vehicle coordinate updated
// clear last waypoint list, and add current vehicle coordinate as first waypoint
void CustomGPSWaypointRecoder::startRecording()
{
    _waypoints.clear();
    auto activeVehicle = _toolbox->multiVehicleManager()->activeVehicle();
    _waypoints.append(activeVehicle->coordinate());
    _recordConnection = connect(activeVehicle, &Vehicle::coordinateChanged,
                                this, &CustomGPSWaypointRecoder::onPointAdded);
    _isRecording = true;
    emit isRecordingChanged();
}

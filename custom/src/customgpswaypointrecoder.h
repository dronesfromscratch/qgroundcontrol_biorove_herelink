#ifndef CUSTOMGPSWAYPOINTRECODER_H
#define CUSTOMGPSWAYPOINTRECODER_H

#include <QObject>
#include <QMetaObject>
#include "QGCToolbox.h"
#include "TrajectoryPoints.h"
#include <QUrl>

class CustomGPSWaypointRecoder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isRecording MEMBER _isRecording NOTIFY isRecordingChanged)
    Q_PROPERTY(double minimiumDistance READ minimiumDistance WRITE setMinimiumDistance NOTIFY minimiumDistanceChanged)
public:
    explicit CustomGPSWaypointRecoder(QGCToolbox* toolbox, QObject *parent = nullptr);
    Q_INVOKABLE void startRecording();
    Q_INVOKABLE void stopRecording();
    Q_INVOKABLE void saveMission(QUrl filePath);

    double minimiumDistance() const;
    void setMinimiumDistance(const double &minimiumDistance);

signals:
    void isRecordingChanged();
    void minimiumDistanceChanged();
private slots:
    void onPointAdded(QGeoCoordinate cor);

private:
    QGCToolbox* _toolbox;
    QMetaObject::Connection _recordConnection;
    bool _isRecording;
    QList<QGeoCoordinate> _waypoints;
    double _minimiumDistance;
};

#endif // CUSTOMGPSWAYPOINTRECODER_H

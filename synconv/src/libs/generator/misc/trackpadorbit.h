#pragma once

#include <QPointer>
#include <QQuickItem>
#include <QQuickWindow>
#include <QtDataVisualization/Q3DCamera>

class TrackpadOrbit : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QtDataVisualization::Q3DCamera* camera READ camera WRITE setCamera NOTIFY cameraChanged)

public:
    explicit TrackpadOrbit(QQuickItem* parent = nullptr);
    QtDataVisualization::Q3DCamera* camera() const { return m_camera; }
    void setCamera(QtDataVisualization::Q3DCamera* camera);

signals:
    void cameraChanged();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    QPointer<QtDataVisualization::Q3DCamera> m_camera;
    QPointer<QQuickWindow> m_window;
};

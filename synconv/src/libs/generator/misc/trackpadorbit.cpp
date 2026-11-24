#include "trackpadorbit.h"

#include <QNativeGestureEvent>
#include <QWheelEvent>
#include <algorithm>
#include <cmath>

TrackpadOrbit::TrackpadOrbit(QQuickItem* parent) : QQuickItem(parent)
{
    connect(this, &QQuickItem::windowChanged, this, [this](QQuickWindow* window) {
        if (m_window)
            m_window->removeEventFilter(this);
        m_window = window;
        if (m_window)
            m_window->installEventFilter(this);
    });
}

void TrackpadOrbit::setCamera(QtDataVisualization::Q3DCamera* camera)
{
    if (m_camera != camera) {
        m_camera = camera;
        emit cameraChanged();
    }
}

bool TrackpadOrbit::eventFilter(QObject* watched, QEvent* event)
{
    if (watched != m_window || !m_camera || !isVisible() || !isEnabled())
        return false;

    if (event->type() == QEvent::Wheel) {
        auto* wheel = static_cast<QWheelEvent*>(event);
        if (!contains(mapFromScene(wheel->position())))
            return false;

        // Cocoa sends pixel deltas for a two-finger pan. A regular mouse wheel
        // normally only has angleDelta and should reach the graph's zoom handler.
        QPointF motion = wheel->pixelDelta();
        if (motion.isNull()) {
            if (wheel->phase() == Qt::NoScrollPhase)
                return false;
            motion = QPointF(wheel->angleDelta()) / 8.0;
        }
        constexpr float degreesPerPixel = 0.4f;
        m_camera->setXRotation(std::remainder(m_camera->xRotation() -
                              static_cast<float>(motion.x()) * degreesPerPixel, 360.0f));
        m_camera->setYRotation(m_camera->yRotation() + static_cast<float>(motion.y()) * degreesPerPixel);
    } else if (event->type() == QEvent::NativeGesture) {
        auto* gesture = static_cast<QNativeGestureEvent*>(event);
        if (gesture->gestureType() != Qt::ZoomNativeGesture || !contains(mapFromScene(gesture->windowPos())))
            return false;
        const float zoom = m_camera->zoomLevel() * static_cast<float>(1.0 + gesture->value());
        m_camera->setZoomLevel(std::clamp(zoom, m_camera->minZoomLevel(), m_camera->maxZoomLevel()));
    } else {
        return false;
    }

    event->accept();
    return true;
}

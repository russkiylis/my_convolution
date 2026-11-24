#pragma once

#include "generator.h"

#include <QFutureWatcher>
#include <QObject>
#include <QPointer>
#include <QThreadPool>
#include <QVariantList>
#include <QVariantMap>
#include <QtDataVisualization/QSurfaceDataProxy>

#include <optional>

class ViewerBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString summary READ summary NOTIFY summaryChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)

public:
    ViewerBackend();

    bool busy() const { return m_busy; }
    QString summary() const { return m_summary; }
    QString error() const { return m_error; }

    Q_INVOKABLE void generate(QObject* series, const QVariantList& peaks, const QVariantMap& noise,
                              double stepH, double stepV, int seed, bool preview);

signals:
    void busyChanged();
    void summaryChanged();
    void errorChanged();

private:
    struct Result
    {
        QVector<QtDataVisualization::QSurfaceDataRow> rows;
        QString summary;
        QString error;
    };

    struct Worker
    {
        QFutureWatcher<Result> watcher;
        quint64 revision = 0; // Ноль означает, что этот обработчик свободен.
    };

    static Result makeSurface(const synconv::generator::Config& config, bool preview);
    void startPending();
    void finish(Worker& worker);
    void publish(const Result& result);
    void setError(const QString& message);

    QThreadPool m_previewPool;
    Worker m_preview;
    Worker m_full;
    quint64 m_revision = 0;
    quint64 m_displayedRevision = 0;
    bool m_previewRequested = false;
    QPointer<QObject> m_series;
    std::optional<synconv::generator::Config> m_pending;
    bool m_busy = false;
    QString m_summary;
    QString m_error;
};

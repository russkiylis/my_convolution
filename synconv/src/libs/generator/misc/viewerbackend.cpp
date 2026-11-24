#include "viewerbackend.h"

#include <QtConcurrent/QtConcurrentRun>
#include <QtDataVisualization/QSurface3DSeries>

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

using namespace QtDataVisualization;
namespace gen = synconv::generator;

namespace
{
struct Bin
{
    int first;
    int end;
    float angle;
};

// Крайние углы остаются отдельными узлами. Внутренние диапазоны покрывают
// все остальные отсчёты, поэтому даже одиночный узкий пик входит в поверхность.
std::vector<Bin> axisBins(int count, int limit, double minimum, double step)
{
    const int displayed = std::min(count, limit);
    std::vector<Bin> bins;
    bins.reserve(displayed);
    bins.push_back({0, 1, static_cast<float>(minimum)});
    for (int i = 1; i < displayed - 1; ++i) {
        const int first = 1 + (i - 1) * (count - 2) / (displayed - 2);
        const int end = 1 + i * (count - 2) / (displayed - 2);
        const double midpoint = (first + end - 1) / 2.0;
        bins.push_back({first, end, static_cast<float>(minimum + midpoint * step)});
    }
    bins.push_back({count - 1, count, static_cast<float>(minimum + (count - 1) * step)});
    return bins;
}
} // namespace

ViewerBackend::ViewerBackend()
{
    m_previewPool.setMaxThreadCount(1);
    connect(&m_preview.watcher, &QFutureWatcher<Result>::finished, this, [this] { finish(m_preview); });
    connect(&m_full.watcher, &QFutureWatcher<Result>::finished, this, [this] { finish(m_full); });
}

void ViewerBackend::generate(QObject* series, const QVariantList& peaks, const QVariantMap& noise,
                             double stepH, double stepV, int seed, bool preview)
{
    if (!qobject_cast<QSurface3DSeries*>(series)) {
        setError(tr("Некорректная поверхность"));
        return;
    }

    gen::Config config;
    // До четырёх пиков кадровая сетка 361 × 181 остаётся лёгкой для пересчёта.
    const double previewMinStep = peaks.size() <= 4 ? 1.0 : 5.0;
    config.horizontal = {0, 360, preview ? std::max(stepH, previewMinStep) : stepH};
    config.vertical = {-90, 90, preview ? std::max(stepV, previewMinStep) : stepV};
    constexpr std::array shapes{gen::PeakShape::gaussian, gen::PeakShape::triangle, gen::PeakShape::rectangle};
    config.peaks.reserve(static_cast<std::size_t>(peaks.size()));
    for (const QVariant& item : peaks) {
        const auto peak = item.toMap();
        bool validShape = false;
        const int shape = peak.value("shape").toInt(&validShape);
        if (!validShape || shape < 0 || shape >= static_cast<int>(shapes.size())) {
            setError(tr("Некорректная форма пика"));
            return;
        }
        config.peaks.push_back({shapes[static_cast<std::size_t>(shape)],
                                peak.value("azimuth").toDouble(), peak.value("elevation").toDouble(),
                                peak.value("amplitude").toDouble(), peak.value("width").toDouble()});
    }
    const auto noiseType = noise.value("type").toString();
    if (noiseType == QStringLiteral("normal")) {
        const double mean = noise.value("mean").toDouble();
        const double sigma = noise.value("sigma").toDouble();
        if (mean != 0.0 || sigma != 0.0)
            config.noises.emplace_back(gen::NormalNoise{mean, sigma});
    } else if (noiseType == QStringLiteral("uniform")) {
        config.noises.emplace_back(gen::UniformNoise{noise.value("min").toDouble(), noise.value("max").toDouble()});
    } else {
        setError(tr("Неизвестный тип шума"));
        return;
    }
    config.seed = static_cast<std::uint32_t>(seed);

    m_series = series;
    m_pending = std::move(config);
    ++m_revision;
    m_previewRequested = preview;
    setError({});
    if (!m_busy) {
        m_busy = true;
        emit busyChanged();
    }
    startPending();
}

void ViewerBackend::startPending()
{
    Worker& worker = m_previewRequested ? m_preview : m_full;
    if (!m_pending || worker.revision != 0)
        return;

    worker.revision = m_revision;
    QThreadPool* pool = m_previewRequested ? &m_previewPool : QThreadPool::globalInstance();
    worker.watcher.setFuture(QtConcurrent::run(pool, [config = std::move(*m_pending), preview = m_previewRequested] {
        return makeSurface(config, preview);
    }));
    m_pending.reset();
}

ViewerBackend::Result ViewerBackend::makeSurface(const gen::Config& config, bool preview)
{
    Result result;
    try {
        const auto samples = gen::generate(config);
        // Полные диапазоны 360° и 180° кратны каждому разрешённому шагу.
        const int countH = static_cast<int>(std::lround(360.0 / config.horizontal.stepDeg)) + 1;
        const int countV = static_cast<int>(std::lround(180.0 / config.vertical.stepDeg)) + 1;
        if (samples.size() != static_cast<std::size_t>(countH) * countV)
            throw std::logic_error("Unexpected generator grid dimensions");

        const auto columns = axisBins(countH, 181, config.horizontal.minDeg, config.horizontal.stepDeg);
        const auto rows = axisBins(countV, 91, config.vertical.minDeg, config.vertical.stepDeg);
        double maximum = 0;
        result.rows.reserve(static_cast<int>(rows.size()));
        for (const Bin& row : rows) {
            QSurfaceDataRow items;
            items.reserve(static_cast<int>(columns.size()));
            for (const Bin& column : columns) {
                double height = 0;
                for (int v = row.first; v < row.end; ++v) {
                    const auto line = samples.begin() + static_cast<std::ptrdiff_t>(v) * countH;
                    height = std::max(height, *std::max_element(line + column.first, line + column.end));
                }
                items.append(QSurfaceDataItem(QVector3D(column.angle, static_cast<float>(height), row.angle)));
                maximum = std::max(maximum, height);
            }
            result.rows.append(std::move(items));
        }
        result.summary = (preview ? tr("Предпросмотр   ·   %1 × %2   ·   Максимум %3")
                                  : tr("Сетка %1 × %2   ·   Максимум %3"))
                             .arg(countH).arg(countV).arg(maximum, 0, 'f', 2);
        if (static_cast<int>(columns.size()) < countH || static_cast<int>(rows.size()) < countV)
            result.summary += tr("   ·   На экране %1 × %2").arg(columns.size()).arg(rows.size());
    } catch (const std::exception& exception) {
        result.error = QString::fromUtf8(exception.what());
    }
    return result;
}

void ViewerBackend::finish(Worker& worker)
{
    const quint64 finished = worker.revision;
    worker.revision = 0;
    // При непрерывном движении показываем готовые кадры, даже если уже ожидает
    // следующий. Результат старого полного расчёта не должен закрыть новый кадр.
    const bool liveFrame = &worker == &m_preview && m_previewRequested;
    if (finished > m_displayedRevision && (finished == m_revision || liveFrame)) {
        m_displayedRevision = finished;
        publish(worker.watcher.result());
    }
    if (finished == m_revision) {
        m_busy = false;
        emit busyChanged();
    }
    startPending();
}

void ViewerBackend::publish(const Result& result)
{
    if (!result.error.isEmpty()) {
        setError(result.error);
        return;
    }
    auto* series = qobject_cast<QSurface3DSeries*>(m_series.data());
    if (!series)
        return;

    auto* array = new QSurfaceDataArray;
    array->reserve(result.rows.size());
    for (const auto& row : result.rows)
        array->append(new QSurfaceDataRow(row));
    series->dataProxy()->resetArray(array);
    if (m_summary != result.summary) {
        m_summary = result.summary;
        emit summaryChanged();
    }
}

void ViewerBackend::setError(const QString& message)
{
    if (m_error != message) {
        m_error = message;
        emit errorChanged();
    }
}

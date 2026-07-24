#include "peaklistmodel.h"

PeakListModel::PeakListModel(std::vector<std::unique_ptr<AbstractPeak::PeakConfig>> &peakConfig, QObject *parent) :
    QAbstractListModel {parent},
    m_peakConfig(&peakConfig)
{
}

int PeakListModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(m_peakConfig->size());
}

QHash<int, QByteArray> PeakListModel::roleNames() const
{
    return {
    {PeakTypeRole, "peakType"},
    {CenterRole, "center"},
    {AmplitudeRole, "amplitude"},
    {SigmaRole, "sigma"},
    {HalfWidthRole, "halfWidth"}
    };
}

QVariant PeakListModel::data(const QModelIndex &index, const int role) const {
    // Проверка на адекватность значений
    if (!index.isValid() || m_peakConfig == nullptr || index.row() >= static_cast<int>(m_peakConfig->size()))
        return{};

    // Разыменовываем сырой указатель на вектор умных указателей, берем его по индексу, получаем сырой указатель через get();
    AbstractPeak::PeakConfig *config = (*m_peakConfig)[index.row()].get();

    switch (role) {
        case PeakTypeRole:
            return static_cast<int>(config->type());
        case CenterRole:
            return config->center;
        case AmplitudeRole:
            return config->amplitude;
        case SigmaRole:
            if (const auto configGauss = dynamic_cast<GaussPeak::GaussPeakConfig *>(config); configGauss != nullptr)
                return configGauss->sigma;
            return 0;
        case HalfWidthRole:
            if (const auto configTriangle = dynamic_cast<TrianglePeak::TrianglePeakConfig *>(config); configTriangle != nullptr)
                return configTriangle->halfWidth;
            if (const auto configRectangle = dynamic_cast<RectanglePeak::RectanglePeakConfig *>(config); configRectangle != nullptr)
                return configRectangle->halfWidth;
            return 0;
        default:
            return {};
    }
}

void PeakListModel::qmlUpdate() {
    //TODO: емиты и тд
}

std::vector<std::unique_ptr<AbstractPeak::PeakConfig>> &PeakListModel::peakConfig() const
{
    return *m_peakConfig;
}

void PeakListModel::setPeakConfig(std::vector<std::unique_ptr<AbstractPeak::PeakConfig>> &peakConfig)
{
    beginResetModel();
    m_peakConfig = &peakConfig;
    endResetModel();
    qmlUpdate();
}

void PeakListModel::setPeakType(const int row, const int peakType) {
    // Проверка на адекватность значений
    if (m_peakConfig == nullptr || row >= static_cast<int>(m_peakConfig->size()) || row < 0)
        throw std::logic_error("Неадекватный индекс модели пиков!");

    if ((*m_peakConfig)[row]->type() == static_cast<AbstractPeak::PeakType>(peakType))
        return;

    beginResetModel();
    switch (static_cast<AbstractPeak::PeakType>(peakType)) {
        case AbstractPeak::PeakType::Gauss:
            (*m_peakConfig)[row] = std::make_unique<GaussPeak::GaussPeakConfig>(180, 10, 5);
            break;
        case AbstractPeak::PeakType::Triangle:
            (*m_peakConfig)[row] = std::make_unique<TrianglePeak::TrianglePeakConfig>(180, 10, 25);
            break;
        case AbstractPeak::PeakType::Rectangle:
            (*m_peakConfig)[row] = std::make_unique<RectanglePeak::RectanglePeakConfig>(180, 10, 5);
            break;
        default:
            throw std::logic_error("Неадекватный тип пика!");
    }
    endResetModel();
}

void PeakListModel::setCenter(const int row, const double center) {
    // Проверка на адекватность значений
    if (m_peakConfig == nullptr || row >= static_cast<int>(m_peakConfig->size()) || row < 0)
        throw std::logic_error("Неадекватный индекс модели пиков!");
    if ((*m_peakConfig)[row]->center == center)
        return;

    auto * configGauss = dynamic_cast<GaussPeak::GaussPeakConfig *>((*m_peakConfig)[row].get());
    auto * configTriangle = dynamic_cast<TrianglePeak::TrianglePeakConfig *>((*m_peakConfig)[row].get());
    auto * configRectangle = dynamic_cast<RectanglePeak::RectanglePeakConfig *>((*m_peakConfig)[row].get());

    switch ((*m_peakConfig)[row]->type()) {
    case AbstractPeak::PeakType::Gauss:
        (*m_peakConfig)[row] = std::make_unique<GaussPeak::GaussPeakConfig>(center, configGauss->amplitude, configGauss->sigma);
        break;
    case AbstractPeak::PeakType::Triangle:
        (*m_peakConfig)[row] = std::make_unique<TrianglePeak::TrianglePeakConfig>(center, configTriangle->amplitude, configTriangle->halfWidth);
        break;
    case AbstractPeak::PeakType::Rectangle:
        (*m_peakConfig)[row] = std::make_unique<RectanglePeak::RectanglePeakConfig>(center, configRectangle->amplitude, configRectangle->halfWidth);
        break;
    default:
        throw std::logic_error("Неадекватный тип пика!");
    }
    const QModelIndex changedIndex = index(row, 0);

    emit dataChanged(changedIndex, changedIndex, {CenterRole});
}

void PeakListModel::setAmplitude(const int row, const double amplitude) {
    // Проверка на адекватность значений
    if (m_peakConfig == nullptr || row >= static_cast<int>(m_peakConfig->size()) || row < 0)
        throw std::logic_error("Неадекватный индекс модели пиков!");
    if ((*m_peakConfig)[row]->amplitude == amplitude)
        return;

    auto * configGauss = dynamic_cast<GaussPeak::GaussPeakConfig *>((*m_peakConfig)[row].get());
    auto * configTriangle = dynamic_cast<TrianglePeak::TrianglePeakConfig *>((*m_peakConfig)[row].get());
    auto * configRectangle = dynamic_cast<RectanglePeak::RectanglePeakConfig *>((*m_peakConfig)[row].get());

    switch ((*m_peakConfig)[row]->type()) {
    case AbstractPeak::PeakType::Gauss:
        (*m_peakConfig)[row] = std::make_unique<GaussPeak::GaussPeakConfig>(configGauss->center, amplitude, configGauss->sigma);
        break;
    case AbstractPeak::PeakType::Triangle:
        (*m_peakConfig)[row] = std::make_unique<TrianglePeak::TrianglePeakConfig>(configTriangle->center, amplitude, configTriangle->halfWidth);
        break;
    case AbstractPeak::PeakType::Rectangle:
        (*m_peakConfig)[row] = std::make_unique<RectanglePeak::RectanglePeakConfig>(configRectangle->center, amplitude, configRectangle->halfWidth);
        break;
    default:
        throw std::logic_error("Неадекватный тип пика!");
    }
    const QModelIndex changedIndex = index(row, 0);

    emit dataChanged(changedIndex, changedIndex, {AmplitudeRole});
}

void PeakListModel::setSigma(const int row, const double sigma) {
    // Проверка на адекватность значений
    if (m_peakConfig == nullptr || row >= static_cast<int>(m_peakConfig->size()) || row < 0)
        throw std::logic_error("Неадекватный индекс модели пиков!");

    auto * configGauss = dynamic_cast<GaussPeak::GaussPeakConfig *>((*m_peakConfig)[row].get());

    if (configGauss == nullptr)
        return;

    if (configGauss->sigma == sigma)
        return;

    (*m_peakConfig)[row] = std::make_unique<GaussPeak::GaussPeakConfig>(configGauss->center, configGauss->amplitude, sigma);

    const QModelIndex changedIndex = index(row, 0);
    emit dataChanged(changedIndex, changedIndex, {SigmaRole});
}

void PeakListModel::setHalfWidth(const int row, const double halfWidth) {
    // Проверка на адекватность значений
    if (m_peakConfig == nullptr || row >= static_cast<int>(m_peakConfig->size()) || row < 0)
        throw std::logic_error("Неадекватный индекс модели пиков!");

    auto * configTriangle = dynamic_cast<TrianglePeak::TrianglePeakConfig *>((*m_peakConfig)[row].get());
    auto * configRectangle = dynamic_cast<RectanglePeak::RectanglePeakConfig *>((*m_peakConfig)[row].get());

    if (configTriangle != nullptr) {
        if (configTriangle->halfWidth == halfWidth)
            return;
        (*m_peakConfig)[row] = std::make_unique<TrianglePeak::TrianglePeakConfig>(configTriangle->center, configTriangle->amplitude, halfWidth);

        const QModelIndex changedIndex = index(row, 0);
        emit dataChanged(changedIndex, changedIndex, {HalfWidthRole});

    } else if (configRectangle != nullptr) {
        if (configRectangle->halfWidth == halfWidth)
            return;
        (*m_peakConfig)[row] = std::make_unique<RectanglePeak::RectanglePeakConfig>(configRectangle->center, configRectangle->amplitude, halfWidth);

        const QModelIndex changedIndex = index(row, 0);
        emit dataChanged(changedIndex, changedIndex, {HalfWidthRole});
    }
}

int PeakListModel::addPeak() {
    const int newRowIndex = static_cast<int>(m_peakConfig->size());

    beginInsertRows(QModelIndex(), newRowIndex, newRowIndex);
    m_peakConfig->push_back(std::make_unique<GaussPeak::GaussPeakConfig>(180, 30, 10));
    endInsertRows();

    return newRowIndex;
}

int PeakListModel::removePeak(const int row) {
    if (const int oldSize = static_cast<int>(m_peakConfig->size()); row >= oldSize)
        return -1;

    beginRemoveRows(QModelIndex(), row, row);
    m_peakConfig->erase(m_peakConfig->begin() + row);
    endRemoveRows();

    const int newSize = static_cast<int>(m_peakConfig->size());

    // Если удалили все элементы
    if (newSize == 0)
        return -1;

    // Если удалили последний элемент
    if (row >= newSize)
        return newSize - 1;

    return row;
}

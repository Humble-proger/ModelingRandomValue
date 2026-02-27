#pragma once
#include "IObserver.h"
#include "DataSet.h"
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;
using namespace ModelingRandomValue::Data;

namespace ModelingRandomValue::Observers
{
    /// @brief Класс гистограммы
    ///
    /// Строит гистограмму по данным из DataSet.
    /// Автоматически обновляется при изменении данных в наблюдаемом наборе.
    ///
    /// Эмпирическая плотность вычисляется по формуле:
    ///
    /// f(x) = (количество элементов в столбце) / ( объем выборки * ширина столбца )
    class Histogram : public IObserver
    {
    private:
        DataSet &_dataSet;

        size_t _numCols = 0;

        vector<double> _densities;

        double _minBound = 0.0;
        double _maxBound = 0.0;
        double _colWidth = 0.0;

        mutable bool _dirty = true;

        /// @brief Пересчитать гистограмму по текущим данным
        void recalculate();

        /// @brief Определить индекс столбца для задаваемого значения
        /// @param x значение
        /// @return Индекс столбца (-1 если вне диапазона)
        int getColIndex(double x) const;

    public:
        /// @brief Конструктор
        /// @param ds Ссылка на набор данных
        /// @param cols Количество столбцов гистограммы (cols > 0)
        explicit Histogram(DataSet &ds, size_t cols = 10);

        /// @brief Диструктор
        ~Histogram();

        void update() override;

        /// @brief Проверить, актуальна ли гистограмма
        /// @return Соответствует ли гистограмма текущим данным
        bool isValid() const;

        /// @brief Принудительно пересчитать гистограмму
        void refresh();

#pragma region Info Histogram

        /// @brief Получить количество столбцов
        /// @return Количество столбцов
        size_t getNumCols() const;

        /// @brief Установить новое количество столбцов
        /// @param cols новое количество столбцов
        void setNumCols(size_t cols);

        /// @brief Получить левую границу гистограммы
        /// @return Минимальное значение выборки
        double getMinBound() const;

        /// @brief Получить правую границу гистограммы
        /// @return Максимальное значение выборки
        double getMaxBound() const;

        /// @brief Получить ширину столбца
        /// @return Ширина столбца
        double getColWidth() const;

        /// @brief Получить массив эмпирической плотности
        /// @return Вектор значений плотности для каждого столбца
        const vector<double> &getDensities() const;

        /// @brief Вычислить значение эмпирической плотности в заданной точке
        /// @param x аргумент
        /// @return Плотность в заданной точке (если x вне интервала, возвращается 0)
        double getEmpiricalDensity(double x) const;
#pragma endregion

#pragma region Save

        /// @brief Сохранить данные гистограммы в файл
        /// @param fileBasenameNoExtension имя файла без расширения
        void saveToFile(const string &fileBasenameNoExtension) const;
#pragma endregion
    };
}
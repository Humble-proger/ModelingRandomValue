#pragma once

#include "interfaces.h"

namespace ModelingRandomValue::Data
{
    /// @brief Класс набора данных
    ///
    /// Содержит выборку случайных чисел и предоставляет методы
    /// для работы с данными и вычисления выборочных характеристик.
    /// При изменении данных уведомляет всех зарегистрированных наблюдателей.
    class DataSet
    {
    private:
        vector<double> _data;
        list<Interfaces::IObserver *> _observers;

        // NOTE: Флаги для кэширования (для оптимизации)
        mutable bool _cacheValid = false;
        mutable double _cachedMin = 0.0;
        mutable double _cachedMax = 0.0;
        mutable double _cachedMean = 0.0;
        mutable double _cachedVariance = 0.0;
        mutable double _cachedSkewness = 0.0;
        mutable double _cachedKurtosis = 0.0;

        /// @brief Инвалидация кэша при изменении данных
        void invalidateCache()
        {
            _cacheValid = false;
        }

        /// @brief Обновление кэша (вычисление всех характеристик)
        void updateCache() const;

    public:
        /// @brief Конструктор по умолчанию
        DataSet();

        /// @brief Конструктор с начальными данными
        /// @param initialData вектор начальных данных
        explicit DataSet(const vector<double> &initialData);

        /// @brief Конструктор копирования
        DataSet(const DataSet &other);

        /// @brief Деструктор
        ~DataSet() = default;

#pragma region Manage Observer

        /// @brief Добавить наблюдателя
        /// @param observer указатель на наблюдателя
        void attach(Interfaces::IObserver *observer);

        /// @brief Удалить наблюдателя
        /// @param observer указатель на наблюдателя
        void detach(Interfaces::IObserver *observer);

        /// @brief Уведомить всех наблюдателей об изменении
        void notify();

#pragma endregion

#pragma region Manage Data

        /// @brief Добавить наблюдение в конец выборки
        /// @param value добавляемое значение
        void add(double value);

        /// @brief Вставить наблюдение по индексу
        /// @param index позиция вставки (0 <= index <= size())
        /// @param value добавляемое значение
        /// @throw out_of_range если index вне допустимого диапазона
        void insert(size_t index, double value);

        ///@brief Удалить наблюдение по индексу
        /// @param index позиция удаления (0 <= index < size())
        /// @throw std::out_of_range если index вне допустимого диапазона
        void remove(size_t index);

        /// @brief Изменить наблюдение по индексу
        /// @param index позиция элемента (0 <= index < size())
        /// @param value новое значение
        /// @throw out_of_range если index вне допустимого диапазона
        void set(size_t index, double value);

        /// @brief Получить наблюдение по индексу
        /// @param index позиция элемента (0 <= index < size())
        /// @return значение элемента
        /// @throw out_of_range если index вне допустимого диапазона
        double get(size_t index) const;

        /// @brief Очистить все данные
        void clear();

#pragma endregion

#pragma region Information Data

        /// @brief Получить размер выборки
        /// @return количество элементов
        size_t size() const;

        /// @brief Проверить, пуста ли выборка
        /// @return true если выборка пуста
        bool isEmpty() const;

        /// @brief Получить прямой доступ к данным (для гистограммы)
        /// @return константная ссылка на вектор данных
        const vector<double> &getData() const;

#pragma endregion

#pragma region Selected characteristics

        /// @brief Минимальное значение в выборке
        /// @return минимум
        /// @throw runtime_error если выборка пуста
        double min() const;

        /// @brief Максимальное значение в выборке
        /// @return максимум
        /// @throw runtime_error если выборка пуста
        double max() const;

        /// @brief Выборочное среднее (математическое ожидание)
        /// @return среднее арифметическое
        /// @throw std::runtime_error если выборка пуста
        double mean() const;

        /// @brief Выборочная дисперсия (несмещенная оценка)
        /// @return дисперсия
        /// @throw runtime_error если выборка содержит менее 2 элементов
        double variance() const;

        /// @brief Выборочное среднеквадратическое отклонение
        /// @return стандартное отклонение
        /// @throw runtime_error если выборка содержит менее 2 элементов
        double stddev() const;

        /// @brief Выборочный коэффициент асимметрии (skewness)
        /// @return коэффициент асимметрии
        /// @throw runtime_error если выборка содержит менее 3 элементов
        double skewness() const;

        /// @brief Выборочный коэффициент эксцесса (kurtosis)
        /// @return коэффициент эксцесса
        /// @throw runtime_error если выборка содержит менее 4 элементов
        double kurtosis() const;

#pragma endregion
    };

}
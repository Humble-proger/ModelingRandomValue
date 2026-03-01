#pragma once
#include <random>
#include <cmath>
#include <stdexcept>

#define interface struct

namespace ModelingRandomValue::Interfaces
{
    /// @brief Интерфейс распределения вероятностей
    interface IDistribution
    {
    public:
        /// @brief Вычисляет значение плотности распределения
        /// @param x значение случ. величины
        /// @return Плотность распределения в точке
        virtual double density(double x) const = 0;

        /// @brief Моделирует случайную величину
        /// @return Значение случайной величины
        virtual double random() = 0;

        /// @brief Вычисляет математическое ожидание случайной величины
        /// @return Значение математического ожидания случайной величины
        virtual double mean() const = 0;

        /// @brief Вычисляет дисперсию случайной величины
        /// @return Значение дисперсии случайной величины
        virtual double variance() const = 0;

        /// @brief Вычисляет коэффициент асимметрии случайной величины
        /// @return Значение коэффициента случайной величины
        virtual double skewness() const = 0;

        /// @brief Вычисляет коэффициент эксцесса случайной величины
        /// @return Значение коэффициента эксцесса случайной величины
        virtual double kurtosis() const = 0;

    protected:
        static std::default_random_engine generator;
    };
}
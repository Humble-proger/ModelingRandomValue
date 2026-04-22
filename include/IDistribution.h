#pragma once
#include "Common.h"

#define interface struct

namespace ModelingRandomValue::Interfaces
{
    /// @brief Интерфейс распределения вероятностей
    interface IDistribution
    {
    public:
#pragma region Первая работа
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

        /// @brief Получить параметр сдвига распределения
        /// @return Параметр сдвига распределения
        virtual double getLocation() const = 0;

        /// @brief Получить параметр масштаба распределения
        /// @return Параметр масштаба распределения
        virtual double getScale() const = 0;

        /// @brief Установить параметр сдвига равномерного распределения
        /// @param loc Параметр сдвига равномерного распределения
        virtual void setLocation(double loc) = 0;

        /// @brief Установить параметр масштаба равномерного распределения
        /// @param scale Параметр масштаба равномерного распределения
        virtual void setScale(double scale) = 0;
#pragma endregion

#pragma region Вторая работа

        /// @brief Создать полную копию объекта
        virtual IDistribution *clone() const = 0;

        /// @brief Получить имя класса распределения
        virtual string name() const = 0;

#pragma endregion

    protected:
        static default_random_engine generator;
    };
}
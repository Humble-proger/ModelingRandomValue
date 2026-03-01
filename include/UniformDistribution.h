#pragma once
#include "interfaces.h"
using namespace std;

namespace ModelingRandomValue::Distribution
{
    /// @brief Равномерное распределение
    ///
    /// Параметры:
    ///
    ///     - lower - левая граница (по умолчанию 0)
    ///
    ///     - upper - правая граница (по умолчанию 1)
    /// @note Распределение симметрично относительно location
    class UniformDistribution : public IDistribution, public IPersistent
    {
    private:
        double _lower = 0.0;
        double _upper = 1.0;
        uniform_real_distribution<double> _uniform;
    public:
        /// @brief Конструктор
        /// @param a левая граница
        /// @param b правая граница (b > a)
        /// @throw std::invalid_argument если b <= a
        explicit UniformDistribution(double a = 0.0, double b = 1.0);

// NOTE: Реализация методов IDistribution
#pragma region IDistribution
        double density(double x) const override;
        double random() override;
        double mean() const override;
        double variance() const override;
        double skewness() const override;
        double kurtosis() const override;
#pragma endregion

// NOTE: Реализация методов IPersistent
#pragma region IPersistent
        void save(ostream &out) const override;
        void load(istream &in) override;
#pragma endregion

        /// @brief Получить параметр левой границы равномерного распределения
        /// @return Параметр левой границы равномерного распределения
        double getLower() const 
        {
            return _lower;
        }

        /// @brief Получить параметр правой границы равномерного распределения
        /// @return Параметр правой границы равномерного распределения
        double getUpper() const 
        {
            return _upper;
        }

        /// @brief Установить параметры границ равномерного распределения
        /// @param a левая границы
        /// @param b правая граница
        void setBounds(double a, double b);

        /// @brief Функция распределения равномерного распределения
        /// @param x аргумент
        /// @return F(x) = (x - a) / (b - a)
        double cdf(double x) const;

        /// @brief Квантиль равномерного распределения
        /// @param p вероятность (0 <= p <= 1)
        /// @return x = a + p * (b - a)
        double quantile(double p) const;

        /// @brief Диструктор
        virtual ~UniformDistribution() = default;
    };
}
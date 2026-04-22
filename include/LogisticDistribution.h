#pragma once
#include "UniformDistribution.h"

namespace ModelingRandomValue::Distribution
{
    /// @brief Логистическое распределение
    ///
    /// Параметры:
    ///
    ///    - location - параметр сдвига (по умолчанию 0)
    ///
    ///    - scale - параметр масштаба (по умолчанию 1)
    ///
    /// @note Распределение симметрично относительно location
    class LogisticDistribution : public Interfaces::IDistribution, public Interfaces::IPersistent
    {
    private:
        double _loc = 0.0;
        double _scale = 1.0;
        UniformDistribution _uniform;

    public:
        /// @brief Конструктор
        /// @param loc параметр сдвига
        /// @param s параметр масштаба (s > 0)
        /// @throw std::invalid_argument если s <= 0
        explicit LogisticDistribution(double loc = 0.0, double s = 1.0);

// NOTE: Реализация методов IDistribution
#pragma region IDistribution
        double density(double x) const override;
        double random() override;
        double mean() const override;
        double variance() const override;
        double skewness() const override;
        double kurtosis() const override;
        double getLocation() const override { return _loc; }
        double getScale() const override { return _scale; }
        void setLocation(double loc) override;
        void setScale(double s) override;
#pragma endregion

// NOTE: Реализация методов IPersistent
#pragma region IPersistent
        void save(std::ostream &out) const override;
        void load(std::istream &in) override;
#pragma endregion

#pragma region Вторая работа
        IDistribution* clone() const override { return new LogisticDistribution(_loc, _scale); }
        string name() const override { return "LogisticDistribution"; }
#pragma endregion

        /// @brief Функция распределения логистического закона
        /// @param x аргумент
        /// @return F(x) = 1/(1 + e^(-(x-μ)/s))
        double cdf(double x) const;
    
        ~LogisticDistribution() = default;
    };
}
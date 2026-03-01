#pragma once
#include "interfaces.h"
#include "UniformDistribution.h"
using namespace std;

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
    class LogisticDistribution : public IDistribution, public IPersistent
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
#pragma endregion

// NOTE: Реализация методов IPersistent
#pragma region IPersistent
        void save(std::ostream &out) const override;
        void load(std::istream &in) override;
#pragma endregion

        /// @brief Получить параметр сдвига логистического распределения
        /// @return Параметр сдвига логистического распределения
        double getLocation() const
        {
            return _loc;
        }

        /// @brief Получить параметр масштаба логистического распределения
        /// @return Параметр масштаба логистического распределения
        double getScale() const
        {
            return _scale;
        }

        /// @brief Установить параметр сдвига логистического распределения
        /// @param loc Параметр сдвига логистического распределения
        void setLocation(double loc)
        {
            _loc = loc;
        }

        /// @brief Установить параметр масштаба логистического распределения
        /// @param s Параметр масштаба логистического распределения
        void setScale(double s);

        /// @brief Функция распределения логистического закона
        /// @param x аргумент
        /// @return F(x) = 1/(1 + e^(-(x-μ)/s))
        double cdf(double x) const;
    };
}
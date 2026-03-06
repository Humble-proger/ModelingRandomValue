#pragma once
#include "interfaces.h"
using namespace std;

namespace ModelingRandomValue::Distribution
{
    /// @brief Равномерное распределение
    ///
    /// Параметры:
    ///
    ///     - loc - сдвиг (по умолчанию 0.5)
    ///
    ///     - scale - масштаб (по умолчанию 1)
    /// @note Распределение симметрично относительно location
    class UniformDistribution : public Interfaces::IDistribution, public Interfaces::IPersistent
    {
    private:
        double _loc = 0.5;
        double _scale = 1.0;
        double _scaleDiv2 = 0.5;
        uniform_real_distribution<double> _uniform;

    public:
        /// @brief Конструктор
        /// @param loc сдвиг
        /// @param scale масштаб
        /// @throw std::invalid_argument если scale <= 0
        explicit UniformDistribution(double loc = 0.5, double scale = 1.0);

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
        void save(ostream &out) const override;
        void load(istream &in) override;
#pragma endregion

        ~UniformDistribution() = default;
    };
}
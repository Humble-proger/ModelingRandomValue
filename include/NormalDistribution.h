#pragma once
#include "interfaces.h"
using namespace std;

namespace ModelingRandomValue::Distribution
{
    /// @brief Нормальное распределение
    ///
    /// Параметры:
    ///
    ///    - mean - математическое ожидание (по умолчанию 0)
    ///
    ///    - stddev - среднеквадратичное отклонение (по умолчанию 1)
    /// @note Распределение симметрично относительно location
    class NormalDistribution : public Interfaces::IDistribution, public Interfaces::IPersistent
    {
    private:
        double _mean = 0.0;
        double _stddev = 1.0;
        normal_distribution<double> _normal;

    public:
        /// @brief Конструктор
        /// @param mean математическое ожидание
        /// @param stddev среднеквадратичное отклонение (stddev > 0)
        /// @throw std::invalid_argument если stddev <= 0
        explicit NormalDistribution(double mean = 0.0, double stddev = 1.0);

// NOTE: Реализация методов IDistribution
#pragma region IDistribution
        double density(double x) const override;
        double random() override;
        double mean() const override;
        double variance() const override;
        double skewness() const override;
        double kurtosis() const override;
        double getLocation() const override { return _mean; }
        double getScale() const override { return _stddev; }
        void setLocation(double loc) override;
        void setScale(double s) override;
#pragma endregion

// NOTE: Реализация методов IPersistent
#pragma region IPersistent
        void save(ostream &out) const override;
        void load(istream &in) override;
#pragma endregion

#pragma region Вторая работа
        IDistribution* clone() const override { return new NormalDistribution(_mean, _stddev); }
        string name() const override { return "NormalDistribution"; }
#pragma endregion

    ~NormalDistribution() = default;
    };
}
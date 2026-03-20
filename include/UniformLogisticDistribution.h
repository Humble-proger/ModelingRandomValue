#pragma once
#include "LogisticDistribution.h"
#include "UniformDistribution.h"
using namespace std;

namespace ModelingRandomValue::Distribution
{
    /// @brief Равномерное распределение, сглаженное логистическим (вариант 13)
    ///
    /// Представляет собой сумму двух независимых случайных величин:
    ///
    ///          - равномерной на интервале (-1, 1)
    ///
    ///          - логистической с параметром масштаба s
    /// 
    ///  Параметры:
    ///
    ///     - loc - сдвиг (по умолчанию 0)
    ///
    ///     - scale - масштаб (по умолчанию 1)
    /// @note Распределение симметрично относительно 0
    class UniformLogisticDistribution : public Interfaces::IDistribution, public Interfaces::IPersistent
    {
    private:
        double _loc = 0.0;
        double _scale = 1.0;
        UniformDistribution _uniform;
        LogisticDistribution _logistic;

    public:
        /// @brief Конструкторы класса
        /// @param loc Сдвиг сглаженного равномерного распределения
        /// @param s Масштаб сглаженного равномерного распределения
        /// @throw std::invalid_argument если s <= 0
        explicit UniformLogisticDistribution(double loc = 0.0, double s = 1.0);

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

// NOTE: Реализация методов  IPersistent
#pragma region IPersistent
        void save(ostream &out) const override;
        void load(istream &in) override;
#pragma endregion

        ~UniformLogisticDistribution() = default;
    };
}
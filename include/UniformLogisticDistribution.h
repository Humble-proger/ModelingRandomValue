#pragma once
#include "interfaces.h"
#include "LogisticDistribution.h"
#include <cmath>
#include <stdexcept>
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
     /// @note Распределение симметрично относительно 0
    class UniformLogisticDistribution : public IDistribution, public IPersistent
    {
    private:
        double _scale = 1.0;
        uniform_real_distribution<double> _uniform;
        LogisticDistribution _logistic;

    public:
        /// @brief Конструкторы класса
        /// @param s Масштаб логистического распределения
        /// @throw std::invalid_argument если s <= 0
        explicit UniformLogisticDistribution(double s = 1.0);

        // NOTE: Реализация методов IDistribution
        #pragma region IDistribution
        double density(double x) const override;
        double random() override;
        double mean() const override;
        double variance() const override;
        double skewness() const override;
        double kurtosis() const override;
        #pragma endregion

        // NOTE: Реализация методов  IPersistent
        #pragma region IPersistent
        void save(std::ostream &out) const override;
        void load(std::istream &in) override;
        #pragma endregion

        /// @brief Получить масштаб логистического распределения
        /// @return Масштаб логистического распределения
        double getScale() const 
        {
            return _scale;
        }

        /// @brief Установить значение масштаба логистического распределения
        /// @param s Масштаб логистического распределения
        void setScale(double s);
    };
}
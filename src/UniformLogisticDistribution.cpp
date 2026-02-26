#include "../include/UniformLogisticDistribution.h"
#define _USE_MATH_DEFINES

namespace ModelingRandomValue::Distribution {
    
    UniformLogisticDistribution::UniformLogisticDistribution(double s) 
    : _scale(s)
    , _uniform(-1.0, 1.0)
    , _logistic(0.0, s)
    {
        if (s <= 0.0) {
            throw std::invalid_argument("Параметр масштаба должен быть положительным");
        }
    }

    /// @brief Формула: 
    ///        f(x) = 1/2 * [F((x+1)/s) - F((x-1)/s)], 
    ///        где F(u) - функция логистического распределения
    double UniformLogisticDistribution::density(double x) const 
    {
        double _u1 = (x + 1.0) / _scale;
        double _u2 = (x - 1.0) / _scale;

        return 0.5 * (_logistic.cdf(_u1) - _logistic.cdf(_u2));
    }

    /// @note Формула:
    ///        x = u + y, 
    ///        где u ~ Uniform(-1,1), y ~ Logistic(0,s)
    double UniformLogisticDistribution::random()
    {
        double _u = _uniform.random();

        double _y = _logistic.random();

        return _u + _y;
    }

    /// @note Математическое ожидание равно 0 для симметричного распределения
    double UniformLogisticDistribution::mean() const
    {
        return 0.0;
    }

    /// @note Дисперсия: D = 1/3 + (pi^2 * s^2) / 3
    double UniformLogisticDistribution::variance() const 
    {
        return (1.0 / 3.0) + _logistic.variance();
    }

    /// @note Коэффициент асимметрии равен 0 для симметричного распределения
    double UniformLogisticDistribution::skewness() const 
    {
        return 0.0;
    }

    /// @note Коэффициент эксцесса = -1.2 + (6 * pi^2 * s^2 / 5) / D^2
    double UniformLogisticDistribution::kurtosis() const 
    {
        double _var = variance();
        double _secTerm = (6.0 * M_PI * M_PI * _scale * _scale / 5.0) / (_var * _var);

        return -1.2 + _secTerm;
    }

    void UniformLogisticDistribution::save(ostream& out) const 
    {
        out << _scale;
    }

    void UniformLogisticDistribution::load(istream& in) 
    {
        double s;
        in >> s;
        if (in.fail()) 
        {
            throw runtime_error("Ошибка чтения параметра из файла");
        }
        if (s <= 0.0) 
        {
            throw runtime_error("Некорректное значение параметра в файле");
        }
        _scale = s;
        _logistic.setScale(s);
    }

    void UniformLogisticDistribution::setScale(double s) 
    {
        if (s <= 0.0) 
        {
            throw invalid_argument("Параметр масштаба должен быть положительным");
        }
        _scale = s;
        _logistic.setScale(s);
    }
};
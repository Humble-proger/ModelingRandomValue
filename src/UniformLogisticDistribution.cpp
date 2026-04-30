#include "../include/UniformLogisticDistribution.h"
#define _USE_MATH_DEFINES

namespace ModelingRandomValue::Distribution {
    
    UniformLogisticDistribution::UniformLogisticDistribution(double loc, double s, double shape) 
    : _scale(s)
    , _loc(loc)
    , _shape(shape)
    , _uniform(0.0, 2.0)
    , _logistic(0.0, shape)
    {
        if (s <= 0.0)
        {
            throw invalid_argument("Параметр масштаба должен быть положительным");
        }
        if (shape <= 0.0) 
        {
            throw invalid_argument("Параметр формы должен быть положительным");
        }
    }

    /// @brief Формула: 
    ///        f(x) = 1/2 * [F((x+1)/s) - F((x-1)/s)], 
    ///        где F(u) - функция логистического распределения
    double UniformLogisticDistribution::density(double x) const 
    {
        double _z = (x - _loc) / _scale;
        
        double _u1 = (_z + 1.0);
        double _u2 = (_z - 1.0);

        return 0.5 * (_logistic.cdf(_u1) - _logistic.cdf(_u2)) / _scale;
    }

    /// @note Формула:
    ///        x = u + y, 
    ///        где u ~ Uniform(-1,1), y ~ Logistic(0,s)
    double UniformLogisticDistribution::random()
    {
        double _u = _uniform.random();
        double _y = _logistic.random();

        return _loc + _scale * (_u + _y);
    }

    /// @note Математическое ожидание равно 0 для симметричного распределения
    double UniformLogisticDistribution::mean() const
    {
        return _loc;
    }

    /// @note Дисперсия: D = 1/3 + (pi^2 * s^2) / 3
    double UniformLogisticDistribution::variance() const 
    {
        return ((1.0 / 3.0) + _logistic.variance()) * _scale * _scale;
    }

    /// @note Коэффициент асимметрии равен 0 для симметричного распределения
    double UniformLogisticDistribution::skewness() const 
    {
        return 0.0;
    }

    /// @note Коэффициент эксцесса = 1.2 * (pi^2 * s^2 - 1) / (pi^2 * s^2 + 1)
    double UniformLogisticDistribution::kurtosis() const 
    {
        double _piShapeDegree2 = M_PI * M_PI * _shape * _shape;
        double _denominator = (_piShapeDegree2 + 1.0);

        return 1.2 * (_piShapeDegree2 * _piShapeDegree2 - 1.0) / (_denominator * _denominator);
    }

    void UniformLogisticDistribution::save(ostream& out) const 
    {
        out << _loc << ' ' << _scale << ' ' << _shape;
    }

    void UniformLogisticDistribution::load(istream& in) 
    {
        double loc;
        double s;
        double shape;
        in >> loc >> s >> shape;
        if (in.fail()) 
        {
            throw runtime_error("Ошибка чтения параметра из файла");
        }
        if (s <= 0.0) 
        {
            throw runtime_error("Некорректное значение параметра масштаба в файле");
        }
        if (shape <= 0.0) 
        {
            throw runtime_error("Некорректное значение параметра формы в файле");
        }
        _loc = loc;
        _scale = s;
        _shape = shape;
    }

    void UniformLogisticDistribution::setScale(double s) 
    {
        if (s <= 0.0) 
        {
            throw invalid_argument("Параметр масштаба должен быть положительным");
        }
        _scale = s;
    }

    void UniformLogisticDistribution::setLocation(double loc) 
    {
        _loc = loc;
    }

    void UniformLogisticDistribution::setShape(double shape) 
    {
        if (shape <= 0) 
        {
            throw invalid_argument("Параметр формы должен быть положительным");
        }

        _shape = shape;
        _logistic.setScale(_shape);
    }
}

#include "../include/DistributionFactory.h"

namespace 
{
    using namespace ModelingRandomValue;

    Interfaces::IDistribution* CreateUniformLogisticDistribution() 
    {
        return new Distribution::UniformLogisticDistribution();
    }

    const bool registered = Factories::DistributionFactory::instance()->registerDistribution("UniformLogisticDistribution", CreateUniformLogisticDistribution);
}
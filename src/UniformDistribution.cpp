#include "../include/UniformDistribution.h"
#define _USE_MATH_DEFINES

namespace ModelingRandomValue::Distribution 
{
    UniformDistribution::UniformDistribution(double a, double b) 
    : _lower(a)
    , _upper(b)
    , _uniform(a, b) 
    {
        if (b <= a) 
        {
            throw invalid_argument("Правая граница должна быть больше левой");
        }
    }

    double UniformDistribution::density(double x) const
    {
        if (x >= _lower && x <= _upper) 
        {
            return 1.0 / (_upper - _lower);
        }
        return 0.0;
    }

    double UniformDistribution::random() 
    {
        return _uniform(generator);
    }

    double UniformDistribution::mean() const 
    {
        return (_lower + _upper) / 2.0;
    }

    double UniformDistribution::variance() const 
    {
        double _width = _upper - _lower;
        return _width * _width / 12.0;
    }

    /// @note Коэффициент асимметрии = 0 для симметричного закона
    double UniformDistribution::skewness() const 
    {
        return 0.0;
    }

    /// @note Коэффициент эксцесса = -1.2 для равномерного закона
    double UniformDistribution::kurtosis() const 
    {
        return -1.2;
    }

    double UniformDistribution::cdf(double x) const 
    {
        if (x < _lower) return 0.0;
        if (x > _upper) return 1.0;
        return (x - _lower) / (_upper - _lower);
    }

    double UniformDistribution::quantile(double p) const 
    {
        if (p < 0.0 || p > 1.0) 
        {
            throw invalid_argument("p должно быть в интервале [0, 1]");
        }
        return _lower + p * (_upper - _lower);
    }
    
    void UniformDistribution::save(ostream& out) const {
        out << _lower << " " << _upper;
    }

    void UniformDistribution::load(istream& in) {
        double a, b;
        in >> a >> b;
        if (in.fail()) {
            throw std::runtime_error("Ошибка чтения параметров из файла");
        }
        if (b <= a) {
            throw std::runtime_error("Некорректные границы в файле");
        }
        _lower = a;
        _upper = b;
        _uniform = uniform_real_distribution<double>(a, b);
    }

    void UniformDistribution::setBounds(double a, double b) {
        if (b <= a) {
            throw std::invalid_argument("Правая граница должна быть больше левой");
        }
        _lower = a;
        _upper = b;
        _uniform = uniform_real_distribution<double>(a, b);
    }
}
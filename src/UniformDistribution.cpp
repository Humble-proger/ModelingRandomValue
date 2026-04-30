#include "../include/UniformDistribution.h"
#define _USE_MATH_DEFINES

namespace ModelingRandomValue::Distribution 
{
    UniformDistribution::UniformDistribution(double loc, double scale) 
    : _loc(loc)
    , _scale(scale)
    {
        if (scale <= 0) 
        {
            throw invalid_argument("Параметр масштаба должен быть положительным");
        }
        double _scaleDiv2 = scale / 2;
        _uniform = uniform_real_distribution<double>(loc - _scaleDiv2, loc + _scaleDiv2);
    }

    double UniformDistribution::density(double x) const
    {
        if (abs(x - _loc) <= _scale) 
        {
            return 1.0 / _scale;
        }
        return 0.0;
    }

    double UniformDistribution::random() 
    {
        return _uniform(generator);
    }

    double UniformDistribution::mean() const 
    {
        return _loc;
    }

    double UniformDistribution::variance() const 
    {
        return _scale * _scale / 12.0;
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
    
    void UniformDistribution::save(ostream& out) const {
        out << _loc << " " << _scale;
    }

    void UniformDistribution::load(istream& in) {
        double loc, scale;
        in >> loc >> scale;
        if (in.fail()) {
            throw runtime_error("Ошибка чтения параметров из файла");
        }
        if (scale <= 0) {
            throw runtime_error("Параметр масштаба должен быть положительным");
        }
        _loc = loc;
        _scale = scale;
        _scaleDiv2 = scale / 2;
        _uniform = uniform_real_distribution<double>(loc - _scaleDiv2, loc + _scaleDiv2);
    }

    void UniformDistribution::setLocation(double loc) 
    {
        _loc = loc;
        _uniform = uniform_real_distribution<double>(loc - _scaleDiv2, loc + _scaleDiv2);
    }

    void UniformDistribution::setScale(double s) 
    {
        if (s <= 0) 
        {
            throw invalid_argument("Параметр масштаба должен быть положительным");
        }
        _scale = s;
        _scaleDiv2 = _scale / 2;
        _uniform = uniform_real_distribution<double>(_loc - _scaleDiv2, _loc + _scaleDiv2);
    }
}

#include "../include/DistributionFactory.h"

namespace 
{
    using namespace ModelingRandomValue;

    Interfaces::IDistribution* CreateUniformDistribution() 
    {
        return new Distribution::UniformDistribution();
    }

    const bool registered = Factories::DistributionFactory::instance()->registerDistribution("UniformDistribution", CreateUniformDistribution);
}
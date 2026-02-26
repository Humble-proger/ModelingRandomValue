#include "../include/LogisticDistribution.h"
#define _USE_MATH_DEFINES
using namespace std;

namespace ModelingRandomValue::Distribution 
{
    LogisticDistribution::LogisticDistribution(double loc, double s) 
    : _loc(loc)
    , _scale(s)
    , _uniform(0.0, 1.0) 
    {
        if (s <= 0.0) {
            throw std::invalid_argument("Параметр масштаба должен быть положительным");
        }
    }

    double LogisticDistribution::cdf(double x) const 
    {
        double _u = (x - _loc) / _scale;

        if (x > 100.0) return 1.0;
        if (x < -100.0) return 0.0;
        return 1.0 / (1.0 + exp(-_u));
    }

    double LogisticDistribution::quantile(double p) const 
    {
        if (p <= 0.0 || p >= 1.0) 
        {
            throw invalid_argument("p должно быть в интервале (0, 1)");
        }
        return _loc - _scale * log((1.0 - p) / p);
    }

    double LogisticDistribution::density(double x) const 
    {
        double _u = (x - _loc) / _scale;

        if (_u > 100.0) return 0.0;
        if (_u < -100.0) return 0.0;

        double _exp = exp(-_u);
        return _exp / (_scale * pow(1.0 + _exp, 2));
    }

    double LogisticDistribution::random() 
    {
        double _p = _uniform.random();
        _p = max(min(_p, 1.0 - 1e-10), 1e-10);

        return quantile(_p);
    }

    double LogisticDistribution::mean() const 
    {
        return _loc;
    }

    // NOTE: D = (pi^2 * s^2) / 3
    double LogisticDistribution::variance() const 
    {
        return (M_PI * M_PI * _scale * _scale) / 3.0;
    }

    // NOTE: Коэффициент асимметрии = 0 для симметричного распределения
    double LogisticDistribution::skewness() const 
    {
        return 0.0;
    }

    // NOTE: Коэффициент эксцесса для логического распределения статичен
    double LogisticDistribution::kurtosis() const 
    {
        return 1.2;
    }

    void LogisticDistribution::save(ostream& out) const 
    {
        out << _loc << " " << _scale;
    }
    void LogisticDistribution::load(istream& in) 
    {
        double loc, s;
        in >> loc >> s;
        if (in.fail()) {
            throw runtime_error("Ошибка чтения параметров из файла");
        }
        if (s <= 0.0) 
        {
            throw runtime_error("Некорректное значение масштаба в файле");
        }
        _loc = loc;
        _scale = s;
    }

    void LogisticDistribution::setScale(double s) 
    {
        if (s <= 0.0) 
        {
            throw invalid_argument("Параметр масштаба должен быть положительным");
        }
        _scale = s;
    }
}
#include "../include/NormalDistribution.h"
#define _USE_MATH_DEFINES

namespace ModelingRandomValue::Distribution 
{
    NormalDistribution::NormalDistribution(double mean, double stddev) 
    : _mean(mean)
    , _stddev(stddev)
    , _normal(mean, stddev) 
    {
        if (stddev <= 0.0) 
        {
            throw invalid_argument("Стандартное отклонение должно быть положительным");
        }
    }

    double NormalDistribution::density(double x) const 
    {
        static const double _sqrt2pi = 2.0 * M_SQRT2 / M_2_SQRTPI;
        double _u = (x - _mean) / _stddev;
        return exp(-0.5 * _u * _u) / (_stddev * _sqrt2pi);
    }

    double NormalDistribution::random() 
    {
        return _normal(generator);
    }

    double NormalDistribution::mean() const
    {
        return _mean;
    }

    double NormalDistribution::variance() const 
    {
        return _stddev * _stddev;
    }

    /// @note Коэффициент асимметрии = 0 для симметричного закона
    double NormalDistribution::skewness() const 
    {
        return 0.0;
    }

    /// @note Коэффициент эксцесса = 0 для нормального закона
    double NormalDistribution::kurtosis() const 
    {
        return 0.0;
    }

    void NormalDistribution::save(ostream& out) const {
        out << _mean << " " << _stddev;
    }

    void NormalDistribution::load(istream& in) {
        double mean, stddev;
        in >> mean >> stddev;
        if (in.fail()) 
        {
            throw runtime_error("Ошибка чтения параметров из файла");
        }
        if (stddev <= 0.0) 
        {
            throw runtime_error("Некорректное стандартное отклонение в файле");
        }
        _mean = mean;
        _stddev = stddev;
        _normal = std::normal_distribution<double>(mean, stddev);
    }

    void NormalDistribution::setStddev(double stddev) {
        if (stddev <= 0.0) 
        {
            throw invalid_argument("Стандартное отклонение должно быть положительным");
        }
        _stddev = stddev;
        _normal = normal_distribution<double>(_mean, stddev);
    }
} 
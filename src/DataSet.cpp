#include "../include/DataSet.h"

namespace ModelingRandomValue::Data
{
    using namespace Interfaces;

    DataSet::DataSet() {}

    DataSet::DataSet(const vector<double>& initialData) : _data(initialData) {}

    DataSet::DataSet(const DataSet& other)
    : _data(other._data)
    , _cacheValid(other._cacheValid)
    , _cachedMin(other._cachedMin)
    , _cachedMax(other._cachedMax)
    , _cachedMean(other._cachedMean)
    , _cachedVariance(other._cachedVariance)
    , _cachedSkewness(other._cachedSkewness)
    , _cachedKurtosis(other._cachedKurtosis) {}

    void DataSet::updateCache() const 
    {
        if (_cacheValid || _data.empty()) { return; }

        _cacheValid = true;
        size_t _n = _data.size();

        _cachedMin = _data.front();
        _cachedMax = _cachedMin;

        double _sum = 0.0;

        for (double _x : _data) 
        {
            _sum += _x;

            if (_x < _cachedMin) { _cachedMin = _x; }
            if (_x > _cachedMax) { _cachedMax = _x; }
        }

        _cachedMean = _sum / _n;

        if (_n < 2) { return; }

        double _sumSq = 0.0, _sumCube = 0.0, _sumFourth = 0.0;

        for (double _x : _data) 
        {
            double _dev = _x - _cachedMean;
            double _devSq = _dev * _dev;

            _sumSq += _devSq;
            _sumCube += _devSq * _dev;
            _sumFourth += _devSq * _devSq;
        }

        _cachedVariance = _sumSq / (_n - 1);

        if (_n < 3) { return; }

        double _s3 = sqrt(_cachedVariance) * _cachedVariance;
        _cachedSkewness = (_sumCube / _n) / _s3;

        if (_n < 4) { return; }

        double _varSq = _cachedVariance * _cachedVariance;
        _cachedKurtosis = (_sumFourth / _n) / _varSq - 3.0;
    }

    void DataSet::attach(IObserver* observer) 
    {
        if (observer) 
        {
            _observers.push_back(observer);
        }
    }

    void DataSet::detach(IObserver* observer) 
    {
        _observers.remove(observer);
    }

    void DataSet::notify() 
    {
        for (IObserver* _observer : _observers) 
        {
            if (_observer) 
            {
                _observer->update();
            }
        }
    }

    void DataSet::add(double value) 
    {
        _data.push_back(value);
        invalidateCache();
        notify();
    }

    void DataSet::insert(size_t index, double value) 
    {
        if (index > _data.size()) 
        {
            throw out_of_range("Индекс вставки вне диапазона");
        }

        _data.insert(_data.begin() + index, value);
        invalidateCache();
        notify();
    }

    void DataSet::remove(size_t index) 
    {
        if (index >- _data.size()) 
        {
            throw out_of_range("Индекс удаления вне диапазона");
        }

        _data.erase(_data.begin() + index);
        invalidateCache();
        notify();
    }

    void DataSet::set(size_t index, double value) 
    {
        if (index >= _data.size()) 
        {
            throw out_of_range("Индекс вне диапазона");
        }

        if (_data[index] != value) 
        {
            _data[index] = value;
            invalidateCache();
            notify();
        }
    }

    double DataSet::get(size_t index) const
    {
        if (index >= _data.size()) 
        {
            throw out_of_range("Индекс вне диапазона");
        }
        return _data[index];
    }

    void DataSet::clear() 
    {
        if (_data.empty()) { return; }

        _data.clear();
        invalidateCache();
        notify();
    }

    size_t DataSet::size() const 
    {
        return _data.size();
    }

    bool DataSet::isEmpty() const 
    {
        return _data.empty();
    }

    const vector<double>& DataSet::getData() const 
    {
        return _data;
    }

    double DataSet::min() const
    {
        if (_data.empty()) 
        {
            throw runtime_error("Невозможно вычислить минимум: у пустой выборки");
        }

        updateCache();
        return _cachedMin;
    }

    double DataSet::max() const
    {
        if (_data.empty()) 
        {
            throw runtime_error("Невозможно вычислить максимум: у пустой выборки");
        }

        updateCache();
        return _cachedMax;
    }

    double DataSet::mean() const
    {
        if (_data.empty()) 
        {
            throw runtime_error("Невозможно вычислить среднее: у пустой выборки");
        }

        updateCache();
        return _cachedMean;
    }

    double DataSet::variance() const
    {
        if (_data.size() < 2) 
        {
            throw runtime_error("Невозможно вычислить дисперсию: недостаточно элементов (нужно >= 2)");
        }

        updateCache();
        return _cachedVariance;
    }

    double DataSet::stddev() const
    {
        return sqrt(variance());
    }

    double DataSet::skewness() const 
    {
        if (_data.size() < 3) 
        {
            throw runtime_error("Невозможно вычислить асимметрию: недостаточно элементов (нужно >= 3)");
        }

        updateCache();
        return _cachedSkewness;
    }

    double DataSet::kurtosis() const 
    {
        if (_data.size() < 4) 
        {
            throw runtime_error("Невозможно вычислить эксцесс: недостаточно элементов (нужно >= 4)");
        }

        updateCache();
        return _cachedKurtosis;
    }
}
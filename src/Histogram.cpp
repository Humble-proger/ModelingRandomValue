#include "../include/Histogram.h"
#include <fstream>
#include <iomanip>

namespace ModelingRandomValue::Observers
{
    Histogram::Histogram(DataSet &ds, size_t cols)
        : _dataSet(ds), _numCols(cols)
    {
        _densities.resize(_numCols, 0.0);
        _dataSet.attach(this);

        recalculate();
    }

    Histogram::~Histogram()
    {
        _dataSet.detach(this);
    }

    void Histogram::update()
    {
        _dirty = true;
        recalculate();
    }

    void Histogram::recalculate()
    {
        if (!_dirty)
        {
            return;
        }

        const vector<double> &_data = _dataSet.getData();
        if (_data.empty())
        {
            fill(_densities.begin(), _densities.end(), 0.0);

            _minBound = 0.0;
            _maxBound = 0.0;
            _colWidth = 0.0;
            _dirty = false;
            return;
        }

        _minBound = _dataSet.min();
        _maxBound = _dataSet.max();

        if (_minBound == _maxBound)
        {
            _minBound -= 0.5;
            _maxBound += 0.5;
        }

        _colWidth = (_maxBound - _minBound) / _numCols;

        vector<int> _counts(_numCols, 0);
        double _n = static_cast<double>(_data.size());
        int _endCol = static_cast<int>(_numCols - 1);

        for (double _x : _data)
        {
            int _col = static_cast<int>((_x - _minBound) / _colWidth);

            _col = min(max(_col, 0), _endCol);

            _counts[_col]++;
        }

        for (int i = 0; i < _numCols; i++)
        {
            _densities[i] = _counts[i] / (_n * _colWidth);
        }

        _dirty = false;
    }

    int Histogram::getColIndex(double x) const
    {
        if (x < _minBound || x > _maxBound || _numCols == 0)
        {
            return -1;
        }

        int _col = static_cast<int>((x - _minBound) / _colWidth);

        if (_col == _numCols)
        {
            _col -= 1;
        }

        return (_col >= 0 && _col < _numCols) ? _col : -1;
    }

    double Histogram::getEmpiricalDensity(double x) const
    {
        if (_dirty)
        {
            const_cast<Histogram *>(this)->recalculate();
        }

        int _col = getColIndex(x);
        return (_col >= 0) ? _densities[_col] : 0.0;
    }

    size_t Histogram::getNumCols() const { return _numCols; }

    void Histogram::setNumCols(size_t cols)
    {
        if (_numCols == cols)
        {
            return;
        }

        _numCols = cols;
        _densities.resize(_numCols);
        _dirty = true;
        recalculate();
    }

    double Histogram::getMinBound() const
    {
        if (_dirty)
        {
            const_cast<Histogram *>(this)->recalculate();
        }

        return _minBound;
    }

    double Histogram::getMaxBound() const
    {
        if (_dirty)
        {
            const_cast<Histogram *>(this)->recalculate();
        }

        return _maxBound;
    }

    double Histogram::getColWidth() const
    {
        if (_dirty)
        {
            const_cast<Histogram *>(this)->recalculate();
        }

        return _colWidth;
    }

    const vector<double> &Histogram::getDensities() const
    {
        if (_dirty)
        {
            const_cast<Histogram *>(this)->recalculate();
        }

        return _densities;
    }

    bool Histogram::isValid() const { return !_dirty; }

    void Histogram::refresh()
    {
        _dirty = true;
        recalculate();
    }

    void Histogram::saveToFile(const string &fileBasenameNoExtension) const
    {
        if (_dirty)
        {
            const_cast<Histogram *>(this)->recalculate();
        }

        ofstream _file(fileBasenameNoExtension + ".csv");
        if (!_file.is_open())
        {
            throw runtime_error("Невозможно открыть файл для записи: " + fileBasenameNoExtension + ".csv");
        }

        _file << fixed << setprecision(6);

        _file << "x_left,x_right,x_center,empirical_density\n";

        for (int _i = 0; _i < _numCols; _i++)
        {
            double _left = _minBound + _i * _colWidth;
            double _right = (_i == _numCols - 1) ? _maxBound : _left + _colWidth;
            double _center = (_left + _right) / 2.0;

            _file << _left << "," << _right << "," << _center << "," << _densities[_i] << "\n";
        }

        _file.close();
    }
}
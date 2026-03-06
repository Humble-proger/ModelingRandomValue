#include "../include/AdditionalFunc.h"

namespace ModelingRandomValue::AdditionalFunc
{
    using namespace Observers;
    using namespace Data;
    using namespace Interfaces;
    
    void printSeparator(char c, int length)
    {
        cout << string(length, c) << endl;
    }

    void printHeader(const string &title)
    {
        cout << endl;
        printSeparator();
        cout << title << endl;
        printSeparator();
    }

    void printSubHeader(const string &title)
    {
        cout << endl
             << "--- " << title << " ---" << endl;
    }

    void printText(const string &text, size_t countSpace)
    {
        cout << string(countSpace, ' ') << text << endl;
    }

    void printValue(const string &name, double value, size_t countSpace)
    {
        cout << fixed << setprecision(6) << defaultfloat << noshowpoint;
        cout << string(countSpace, ' ') << name << ": " << value << endl;
    }

    void printDataStatistic(DataSet &data)
    {
        cout << "Статистика выборки (n = " << data.size() << "):" << endl;

        if (data.isEmpty())
        {
            printText("Выборка пуста");
            return;
        }

        printValue("Минимум", data.min());
        printValue("Максимум", data.max());
        printValue("Среднее", data.mean());

        if (data.size() < 2)
        {
            return;
        }

        printValue("Дисперсия", data.variance());
        printValue("Стд. отклонение", data.stddev());

        if (data.size() < 3)
        {
            return;
        }

        printValue("Асимметрия", data.skewness());

        if (data.size() < 4)
        {
            return;
        }

        printValue("Эксцесс", data.kurtosis());
    }

    void printHistStatistic(Histogram &hist, bool detail)
    {
        printValue("Количество столбцов", hist.getNumCols());
        cout << string(2, ' ') << "Диапазон: [" << setprecision(2) << hist.getMinBound() << ", " << hist.getMaxBound() << "]" << endl;
        printValue("Ширина столбца", hist.getColWidth());

        if (!detail)
        {
            return;
        }

        auto _bounds = hist.getColBounds();
        auto _densities = hist.getDensities();

        cout << endl;

        for (int i = 0; i < hist.getNumCols(); i++)
        {
            cout << string(2, ' ') << "Столбец " << i << ": [" << _bounds[i].first << ", " << _bounds[i].second << "]" << endl;
            printValue("Плотность", _densities[i]);
        }
    }

    void saveTheoreticalDensity(const string &fileBasenameNoExtension, IDistribution &dist, pair<double, double> bounds, size_t n)
    {
        if (bounds.first >= bounds.second)
        {
            throw invalid_argument("Правая граница меньше левой границы");
        }

        if (n < 2)
        {
            return;
        }

        double _step = (bounds.second - bounds.first) / (n - 1);

        ofstream _file("output/" + fileBasenameNoExtension + ".csv");
        if (!_file.is_open())
        {
            throw runtime_error("Невозможно открыть файл для записи: " + fileBasenameNoExtension + ".csv");
        }

        _file << fixed << setprecision(6);

        _file << "x,theoretical_density" << endl;

        for (double _elapsedX = bounds.first; _elapsedX < bounds.second; _elapsedX += _step)
        {
            _file << _elapsedX << "," << dist.density(_elapsedX) << endl;
        }
    }
}
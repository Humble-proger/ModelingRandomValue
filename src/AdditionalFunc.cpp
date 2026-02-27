#include "../include/AdditionalFunc.h"

namespace ModelingRandomValue::AdditionalFunc
{

    void printSeparator(char c, int length)
    {
        cout << string(length, c) << endl;
    }

    void printHeader(const string &title)
    {
        printSeparator();
        cout << " " << title << endl;
        printSeparator();
    }

    void printSubHeader(const string &title)
    {
        cout << "\n--- " << title << " ---" << endl;
    }

    void printText(const string &text, size_t countSpace)
    {
        cout << setw(countSpace) << text << endl;
    }

    void printValue(const string &name, double value, size_t countSpace)
    {
        cout << setw(countSpace) << name << ": " << value << endl;
    }

    void printDataStatistic(DataSet &data)
    {
        printText("Статистика выборки (n = " + to_string(data.size()) + "):", 0);

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
        printText("Диапазон: [" + to_string(hist.getMinBound()) + ", " + to_string(hist.getMaxBound()) + "]");
        printValue("Ширина столбца", hist.getColWidth());

        if (!detail)
        {
            return;
        }

        auto _bounds = hist.getBinBounds();
        auto _densities = hist.getDensities();

        for (int i = 0; i < hist.getNumCols(); i++)
        {
            printText("Столбец " + to_string(i) + ": [" + to_string(_bounds[i].first) + ", " + to_string(_bounds[i].second) + "]");
            printValue("Плотность", _densities[i]);
        }
    }
}
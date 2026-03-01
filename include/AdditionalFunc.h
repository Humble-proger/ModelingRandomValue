#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include "DataSet.h"
#include "Histogram.h"
#include "IDistribution.h"
using namespace std;
using namespace ModelingRandomValue::Data;
using namespace ModelingRandomValue::Observers;
using namespace ModelingRandomValue::Interfaces;

namespace ModelingRandomValue::AdditionalFunc
{

    /// @brief Печать разделителя для красивого вывода
    /// @param c символ сепаратора
    /// @param length количество символов
    void printSeparator(char c = '=', int length = 70);

    /// @brief Печать заголовка раздела
    /// @param title раздел
    void printHeader(const string &title);

    /// @brief Печать саб заголовка раздела
    /// @param title раздел
    void printSubHeader(const string &title);

    /// @brief Печать текста
    /// @param text текст
    /// @param countSpace количество пробелов
    void printText(const string &text, size_t countSpace = 2);

    /// @brief Печать информацию об значении
    /// @param name имя значения
    /// @param value значение
    /// @param countSpace количество пробелов
    void printValue(const string &name, double value, size_t countSpace = 2);

    /// @brief Распечатать основные характеристики распределения
    /// @tparam Dist Распределение, которое наследовано от IDistribution
    /// @param nameDist имя распределения
    /// @param dist распределение
    /// @param x точка в которой будет получена значение плотности
    template <typename Dist>
    void printDist(const string &nameDist, Dist &dist, double x)
    {
        static_assert(is_base_of<IDistribution, Dist>::value, "Передаваемый класс не является распределением");
        printSubHeader(nameDist);

        cout << string(2, ' ') << "Плотность в x = " << setprecision(1) << x << ": " << defaultfloat << noshowpoint << dist.density(x) << endl;
        printValue("Мат. ожидание", dist.mean());
        printValue("Дисперсия", dist.variance());
        printValue("Коэф. асимметрии", dist.skewness());
        printValue("Коэф. эксцесса", dist.kurtosis());
        printValue("Случайное число", dist.random());
    }

    /// @brief Вывести статистику набора данных
    /// @param data набор данных
    void printDataStatistic(DataSet &data);

    /// @brief Вывести статистику гистограммы
    /// @param hist гистограмма
    void printHistStatistic(Histogram &hist, bool detail = false);

    /// @brief Распечатать строку таблицы
    /// @tparam Elem Тип, который можно перевести в строку
    /// @param list Список из пар (значение столбца, количество пробелов перед столбцом)
    /// @note Данная функция поддерживает вывод только английскими символами, ввиду того, что символы кириллицы
    ///       занимают 2 байта, вместо 1 байта. Если подавать символы с кириллицей функция не будет корректно
    ///       выравнивать вывод в консоль. Для исправления этого, необходима отдельная реализация отслеживающая
    ///       многобайтовые символы.
    template <typename Elem>
    void printStringTable(initializer_list<pair<Elem, size_t>> list)
    {
        for (const auto &_elem_list : list)
        {
            cout << left << setw(_elem_list.second) << defaultfloat << noshowpoint << _elem_list.first;
        }
        cout << endl;
    }

    /// @brief Замер времени выполнения функции
    /// @param func функция
    template <typename Func>
    double measureTime(Func func)
    {
        auto start = chrono::high_resolution_clock::now();
        func();
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> elapsed = end - start;
        return elapsed.count();
    }

    /// @brief Сохранить теоретическую выборку значений функции плотности
    /// @param fileBasenameNoExtension имя файла без расширения
    /// @param dist распределение
    /// @param bounds пара границ (левая граница, правая граница)
    /// @param n количество разбиений
    void saveTheoreticalDensity(const string &fileBasenameNoExtension, IDistribution &dist, pair<double, double> bounds, size_t n = 500);

    /// @brief Сравнение эмпирической плотности с теоретической
    /// @tparam Dist распределение, которое наследовано от IDistribution
    /// @param distName имя распределения
    /// @param dist распределение
    /// @param sizes список пар ключей (размер выборки, количество столбцов)
    template<typename Dist>
    void compareDensity(const string& distName, Dist &dist, vector<pair<size_t, size_t>>& sizes) 
    {
        static_assert(is_base_of<IDistribution, Dist>::value, "Передаваемый класс не является распределением");
        for (const auto& _elem : sizes)
        {
            printSubHeader("Объем выборки n = " + to_string(_elem.first));

            DataSet _dataSet;
            for (int _i = 0; _i < _elem.first; _i++)
            {
                _dataSet.add(dist.random());
            }

            Histogram _hist(_dataSet, _elem.second);

            double _minVal = _dataSet.min();
            double _maxVal = _dataSet.max();
            double _step = (_maxVal - _minVal) / 100.0;

            double _totalError = 0.0;
            int _points = 0;

            for (double _x = _minVal; _x <= _maxVal; _x += _step)
            {
                double _theoretical = dist.density(_x);
                double _empirical = _hist.getEmpiricalDensity(_x);
                _totalError += abs(_theoretical - _empirical);
                _points++;
            }

            double _meanError = _totalError / _points;
            printValue("Среднее абсолютное отклонение", _meanError);

            string _filename = distName + "_n" + to_string(_elem.second);
            _hist.saveToFile(_filename);
            printText("Данные сохранены в " + _filename + ".csv");
            saveTheoreticalDensity(_filename + "_theoretical", dist, { _hist.getMinBound(), _hist.getMaxBound() }, 100);
            printText("Данные о теоретической плотности сохранены в " + _filename + "_theoretical.csv");
        }
    }
}

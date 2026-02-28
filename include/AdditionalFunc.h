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

        cout << setw(2) << "Плотность в x =" << setprecision(1) << x << ": " << setprecision(6) << dist.density(x) << endl;
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
}

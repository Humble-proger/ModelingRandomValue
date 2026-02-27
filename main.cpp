#include <iostream> // Подключение библиотеки для ввода-вывода
#include <vector>
#include "include/LogisticDistribution.h"
#include "include/UniformLogisticDistribution.h"
#include "include/UniformDistribution.h"
#include "include/NormalDistribution.h"

using namespace std;
using namespace ModelingRandomValue::Distribution;

template <typename Distribution>
void demonstrateDistribution(const string &name, Distribution &dist)
{
    cout << "=== Демонстрация " << name << " ===" << endl;

    cout << "Теоретические характеристики:" << std::endl;
    cout << "  Мат. ожидание: " << dist.mean() << endl;
    cout << "  Дисперсия: " << dist.variance() << endl;
    cout << "  Коэфф. асимметрии: " << dist.skewness() << endl;
    cout << "  Коэфф. эксцесса: " << dist.kurtosis() << endl;

    cout << "\nНесколько значений функции плотности:" << endl;
    for (double x = -3.0 + dist.mean(); x <= 3.0 + dist.mean(); x += 1.0)
    {
        cout << "  f(" << x << ") = " << dist.density(x) << endl;
    }

    cout << "\nНесколько случайных чисел:" << endl;
    for (int i = 0; i < 5; i++)
    {
        cout << "  " << dist.random() << endl;
    }
    cout << endl;
}

int main()
{

    UniformDistribution _uniform(1.0, 9.0);
    NormalDistribution _normal(0.0, 1.0);
    LogisticDistribution _logistic(0.0, 1.0);
    UniformLogisticDistribution _uniformLogistic(1.0);
    demonstrateDistribution("равномерного распределения", _uniform);
    demonstrateDistribution("нормального распределения", _normal);
    demonstrateDistribution("логистического распределения", _logistic);
    demonstrateDistribution("равномерного распределения, сглаженным логистическим распределением", _uniformLogistic);

    return 0;
}

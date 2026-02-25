#include <iostream> // Подключение библиотеки для ввода-вывода
#include <vector>
#include "LogisticDistribution.h"
#include "UniformLogisticDistribution.h"

using namespace std;
using namespace ModelingRandomValue::Distribution;


void demonstrateLogicDistribution(double loc = 0.0, double scale = 1.0) 
{
    cout << "=== Демонстрация логистического распределения ===" << endl;
    
    LogisticDistribution _logistic(loc, scale);
    
    cout << "Теоретические характеристики:" << std::endl;
    cout << "  Мат. ожидание: " << _logistic.mean() << endl;
    cout << "  Дисперсия: " << _logistic.variance() << endl;
    cout << "  Коэфф. эксцесса: " << _logistic.kurtosis() << endl;
    
    cout << "\nНесколько значений функции распределения:" << endl;
    for (double x = -3.0 + loc; x <= 3.0 + loc; x += 1.0) {
        cout << "  F(" << x << ") = " << _logistic.cdf(x) << endl;
    }
    
    cout << "\nНесколько случайных чисел:" << endl;
    for (int i = 0; i < 5; i++) {
        cout << "  " << _logistic.random() << endl;
    }
    cout << endl;
}

void demonstrateUniformLogicDistribution(double scale = 1.0) 
{
    cout << "=== Демонстрация равномерного распределения, сглаженное логистическим ===" << endl;
    
    UniformLogisticDistribution _uniformLogistic(scale);
    
    cout << "Теоретические характеристики:" << std::endl;
    cout << "  Мат. ожидание: " << _uniformLogistic.mean() << endl;
    cout << "  Дисперсия: " << _uniformLogistic.variance() << endl;
    cout << "  Коэфф. эксцесса: " << _uniformLogistic.kurtosis() << endl;
    
    cout << "\nНесколько значений функции плотности:" << endl;
    for (double x = -3.0; x <= 3.0; x += 1.0) {
        cout << "  F(" << x << ") = " << _uniformLogistic.density(x) << endl;
    }
    
    cout << "\nНесколько случайных чисел:" << endl;
    for (int i = 0; i < 5; i++) {
        cout << "  " << _uniformLogistic.random() << endl;
    }
    cout << endl;
}

int main() {
    
    demonstrateLogicDistribution(3.0, 2.0);
    demonstrateUniformLogicDistribution();
    
    return 0;
}

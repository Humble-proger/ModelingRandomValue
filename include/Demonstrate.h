#pragma once

#include "AdditionalFunc.h"
#include "LogisticDistribution.h"
#include "UniformLogisticDistribution.h"
#include "UniformDistribution.h"
#include "NormalDistribution.h"

namespace ModelingRandomValue::Demonstrate 
{
    
    /// @brief Демонстрация создания и базовых методов распределений
    void demonstrateDistributions();

    /// @brief Демонстрация генерации выборок разного объема и проверка сходимости 
    void demonstrateSampling();
    
    /// @brief Демонстрация методов класса DataSet 
    void demonstrateDataSet();

    /// @brief Демонстрация класса Histogram 
    void demonstrateHistogram();
  
    /// @brief Демонстрация сравнения теоретической и эмпирической плотности 
    void demonstrateDensityComparison();

    /// @brief Демонстрация сохранения и загрузки распределений 
    void demonstratePersistence();
        
    /// @brief Демонстрация производительности генерации 
    void demonstratePerformance();

    /// @brief Демонстрация паттерна Наблюдатель на примере гистограммы
    void demonstrateHistogramObserver();
}
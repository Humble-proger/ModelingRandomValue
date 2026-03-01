#include "include/Demonstrate.h"
using namespace ModelingRandomValue::Demonstrate;

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF-8");

    demonstrateDistributions();
    demonstrateSampling();
    demonstrateDataSet();
    demonstrateHistogram();
    demonstrateDensityComparison();
    demonstratePersistence();
    demonstratePerformance();
    demonstrateHistogramObserver();
    return 0;
}

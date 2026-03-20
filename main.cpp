#include "include/Demonstrate.h"

int main()
{
    using namespace ModelingRandomValue::Demonstrate;

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

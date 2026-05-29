#ifndef MIXTURE_BUILDER_H
#define MIXTURE_BUILDER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int k;          // количество компонент
    double* weights; // веса (k)
    double* means;   // средние (k)
    double* stddevs; // стандартные отклонения (k)
    // Для робастного варианта дополнительно возвращается вес равномерной компоненты
    double uniformWeight;
} MixtureParams;

/// @brief Функция построения смеси.
/// @param data массив наблюдений (n)
/// @param n количество наблюдений
/// @param maxK максимальное число компонент для перебора
/// @param robust 0 = обычная смесь нормальных, 1 = робастная (нормальные + равномерная)
/// @param params выходная структура (память выделяется внутри, нужно освободить через freeMixtureParams)
/// @return 0 при успехе, иначе код ошибки
int buildMixture(const double* data, int n, int maxK, int robust, MixtureParams* params);

/// @brief Освобождение памяти, выделенной в MixtureParams
void freeMixtureParams(MixtureParams* params);

#ifdef __cplusplus
}
#endif

#endif
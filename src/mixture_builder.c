#include "../include/mixture_builder.h"
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <time.h>

// NOTE: Вспомогательные функции

/// @brief Плотность нормального распределения
static double gauss_pdf(double x, double mu, double sigma) {
    double u = (x - mu) / sigma;
    return exp(-0.5 * u * u) / (sigma * sqrt(2.0 * M_PI));
}

/// @brief Плотность равномерного распределения 
static double uniform_pdf(double x, double a, double b) {
    return (x >= a && x <= b) ? 1.0 / (b - a) : 0.0;
}

/// @brief Моделирование равномерного закона
static double uniform_rand(double min, double max) {
    return min + (max - min) * (rand() / (RAND_MAX + 1.0));
}

/// @brief Рандомные веса
/// @note Генерируем рандомное положительное значение и нормируем под условие
static void random_weights(double* w, int k) {
    double sum = 0.0;
    for (int i = 0; i < k; ++i) {
        w[i] = uniform_rand(0.0, 1.0);
        sum += w[i];
    }
    for (int i = 0; i < k; ++i) w[i] /= sum;
}

/// @brief Рандомные веса для робастного случая
/// @note Генерируем рандомное положительное значение и нормируем под условие
static void random_weights_robust(double* w, int k, double* uniformWeight) {
    *uniformWeight = uniform_rand(0.05, 0.5);
    double sum = 0.0;
    for (int i = 0; i < k; ++i) {
        w[i] = uniform_rand(0.0, 1.0);
        sum += w[i];
    }
    for (int i = 0; i < k; ++i) w[i] /= sum;
    for (int i = 0; i < k; ++i) w[i] *= (1.0 - *uniformWeight);
}

/// @brief EM-алгоритм для обычной смеси нормальных (k компонент)
static int em_normal_mixture(const double* data, int n, int k,
                             double* weights, double* means, double* stddevs,
                             double* logLik) {
    int i, j, iter;
    double* gamma = (double*)malloc(n * k * sizeof(double));
    if (!gamma) return -1;

    double prevLL = -1e100;
    // NOTE: Ограничиваем количество итераций алгоритма до 200
    for (iter = 0; iter < 200; ++iter) {
        // NOTE: E-step используем формулу Байеса 
        // для вычисления апостериорной вероятности (Лекция ч4 -> 5 формула)
        // Получаем вероятность того, что наблюдение i было сгенерировано j компонентой
        double totalLL = 0.0;
        for (i = 0; i < n; ++i) {
            double sum = 0.0;
            for (j = 0; j < k; ++j) {
                double p = weights[j] * gauss_pdf(data[i], means[j], stddevs[j]);
                gamma[i * k + j] = p;
                sum += p;
            }
            // NOTE: Защита от нуля (вырожденный случай, знаменатель равен 0)
            if (sum < 1e-300) sum = 1e-300;
            totalLL += log(sum);
            for (j = 0; j < k; ++j) gamma[i * k + j] /= sum;
        }
        // NOTE: M-step обновляем параметры каждой компоненты используя формулу Лагранжа
        // с использованием вероятности полученной на шаге E-step (Лекция ч4 -> под 5 формулой)
        // обновление среднего и дисперсии (Лекция ч4 -> 10 стр)
        for (j = 0; j < k; ++j) {
            double sum_gamma = 0.0, sum_x = 0.0, sum_x2 = 0.0;
            for (i = 0; i < n; ++i) {
                double g = gamma[i * k + j];
                sum_gamma += g;
                sum_x += g * data[i];
                sum_x2 += g * data[i] * data[i];
            }
            // NOTE: Защита от деления на 0
            if (sum_gamma < 1e-10) sum_gamma = 1e-10;
            
            // NOTE: Обновление компонент
            weights[j] = sum_gamma / n;
            means[j] = sum_x / sum_gamma;
            double var = sum_x2 / sum_gamma - means[j] * means[j];
            if (var < 1e-6) var = 1e-6;
            stddevs[j] = sqrt(var);
        }
        // NOTE: Проверка сходимости
        if (fabs(totalLL - prevLL) < 1e-6) break;
        prevLL = totalLL;
    }
    if (logLik) *logLik = prevLL;
    free(gamma);
    return 0;
}

/// @brief EM-алгоритм для робастной смеси: нормальные + равномерная на [min, max]
static int em_robust_mixture(const double* data, int n, int k,
                             double* weights, double* means, double* stddevs,
                             double* uniformWeight, double* logLik) {
    double min = data[0], max = data[0];
    for (int i = 1; i < n; ++i) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
    }
    double range = max - min;
    if (range < 1e-10) range = 1.0;

    // NOTE: Количество компонент: k нормальных + 1 равномерная
    int total_comps = k + 1;
    double* gamma = (double*)malloc(n * total_comps * sizeof(double));
    if (!gamma) return -1;

    double prevLL = -1e100;
    // NOTE: Ограничиваем количество итераций алгоритма до 200
    for (int iter = 0; iter < 200; ++iter) {
        // NOTE: E-step вычисляется аналогично смеси нормальным, 
        // только с добавленной равномерной компонентой
        double totalLL = 0.0;
        for (int i = 0; i < n; ++i) {
            double sum = 0.0;
            for (int j = 0; j < k; ++j) {
                double p = weights[j] * gauss_pdf(data[i], means[j], stddevs[j]);
                gamma[i * total_comps + j] = p;
                sum += p;
            }
            // NOTE: Добавляем равномерную компоненту
            double p_unif = (*uniformWeight) * uniform_pdf(data[i], min, max);
            gamma[i * total_comps + k] = p_unif;
            sum += p_unif;
            
            // NOTE: Защита от нуля (вырожденный случай, знаменатель равен 0)
            if (sum < 1e-300) sum = 1e-300;
            
            totalLL += log(sum);
            for (int j = 0; j < total_comps; ++j)
                gamma[i * total_comps + j] /= sum;
        }
        // NOTE: M-step для нормальных компонент, 
        // считается также как для смеси из нормальных
        for (int j = 0; j < k; ++j) {
            double sum_gamma = 0.0, sum_x = 0.0, sum_x2 = 0.0;
            for (int i = 0; i < n; ++i) {
                double g = gamma[i * total_comps + j];
                sum_gamma += g;
                sum_x += g * data[i];
                sum_x2 += g * data[i] * data[i];
            }

            // NOTE: Защита от деления на 0
            if (sum_gamma < 1e-10) sum_gamma = 1e-10;
            weights[j] = sum_gamma / n;
            means[j] = sum_x / sum_gamma;
            double var = sum_x2 / sum_gamma - means[j] * means[j];
            if (var < 1e-6) var = 1e-6;
            stddevs[j] = sqrt(var);
        }
        // NOTE: M-step для равномерной компоненты, 
        // обновляет вес для равномерной компоненты (Лекция ч4 -> стр 18)
        double sum_gamma_unif = 0.0;
        for (int i = 0; i < n; ++i)
            sum_gamma_unif += gamma[i * total_comps + k];
        *uniformWeight = sum_gamma_unif / n;

        double sum_norm_weights = 0.0;
        for (int j = 0; j < k; ++j) {
            sum_norm_weights += weights[j];
        }
        if (sum_norm_weights > 1e-10 && sum_norm_weights < 0.999) {
            // Пересчитываем, чтобы сумма стала 1
            for (int j = 0; j < k; ++j) {
                weights[j] /= sum_norm_weights;
            }
        }

        // NOTE: Проверка сходимости
        if (fabs(totalLL - prevLL) < 1e-6) break;
        prevLL = totalLL;
    }
    if (logLik) *logLik = prevLL;
    free(gamma);
    return 0;
}

/// @brief Вычисление BIC
static double bic(double logLik, int n, int params) {
    return -2.0 * logLik + params * log(n);
}

/// @brief Обертка для множественных запусков обычной смеси
static int em_normal_mixture_with_tries(const double* data, int n, int k,
                                        double* best_weights, double* best_means, double* best_stddevs,
                                        double* best_logLik, int tries) {
    double best_ll = -1e300;
    int success = 0;
    double minVal = data[0], maxVal = data[0];
    double sum = 0.0;
    for (int i = 0; i < n; ++i) {
        if (data[i] < minVal) minVal = data[i];
        if (data[i] > maxVal) maxVal = data[i];
        sum += data[i];
    }
    double mean = sum / n;
    double empVar = 0.0;
    for (int i = 0; i < n; ++i) {
        double dev = data[i] - mean;
        empVar += dev * dev;
    }
    empVar /= (n - 1);
    if (empVar < 1e-6) empVar = 1.0;
    double maxVar = empVar * 2.0;  // NOTE: Удваеваем эмпирическую дисперсию
    double minVar = 1e-6; // NOTE: минимально возможная дисперсия
    if (maxVar < minVar) maxVar = minVar * 10.0;

    for (int t = 0; t < tries; ++t) {
        // NOTE: Инициализация
        double weights[k], means[k], stddevs[k];
        for (int j = 0; j < k; ++j) {
            means[j] = uniform_rand(minVal, maxVal);
            stddevs[j] = sqrt(uniform_rand(minVar, maxVar));
        }
        random_weights(weights, k);
        // NOTE: Сравниваем и сохраняем лучший результат
        double logLik;
        int err = em_normal_mixture(data, n, k, weights, means, stddevs, &logLik);
        if (err == 0 && logLik > best_ll) {
            best_ll = logLik;
            memcpy(best_weights, weights, k * sizeof(double));
            memcpy(best_means, means, k * sizeof(double));
            memcpy(best_stddevs, stddevs, k * sizeof(double));
            success = 1;
        }
    }
    if (success) *best_logLik = best_ll;
    return success ? 0 : -1;
}

/// @brief Обертка для множественных запусков робастной смеси
static int em_robust_mixture_with_tries(const double* data, int n, int k,
                                        double* best_weights, double* best_means, double* best_stddevs,
                                        double* best_uniformWeight, double* best_logLik, int tries) {
    double best_ll = -1e300;
    int success = 0;
    double minVal = data[0], maxVal = data[0];
    double sum = 0.0;
    for (int i = 0; i < n; ++i) {
        if (data[i] < minVal) minVal = data[i];
        if (data[i] > maxVal) maxVal = data[i];
        sum += data[i];
    }
    double mean = sum / n;
    double empVar = 0.0;
    for (int i = 0; i < n; ++i) {
        double dev = data[i] - mean;
        empVar += dev * dev;
    }
    empVar /= (n - 1);
    if (empVar < 1e-6) empVar = 1.0;
    double maxVar = empVar * 2.0;  // NOTE: Удваеваем эмпирическую дисперсию
    double minVar = 1e-6; // NOTE: минимально возможная дисперсия
    if (maxVar < minVar) maxVar = minVar * 10.0;

    for (int t = 0; t < tries; ++t) {
        // NOTE: Инициализация
        double weights[k], means[k], stddevs[k], uniformWeight;
        for (int j = 0; j < k; ++j) {
            means[j] = uniform_rand(minVal, maxVal);
            stddevs[j] = sqrt(uniform_rand(minVar, maxVar));
        }
        random_weights_robust(weights, k, &uniformWeight);
        // NOTE: Сравниваем и сохраняем лучший результат
        double logLik;
        int err = em_robust_mixture(data, n, k, weights, means, stddevs, &uniformWeight, &logLik);
        if (err == 0 && logLik > best_ll) {
            best_ll = logLik;
            memcpy(best_weights, weights, k * sizeof(double));
            memcpy(best_means, means, k * sizeof(double));
            memcpy(best_stddevs, stddevs, k * sizeof(double));
            *best_uniformWeight = uniformWeight;
            success = 1;
        }
    }
    if (success) *best_logLik = best_ll;
    return success ? 0 : -1;
}

/// @brief Основная функция построения смеси с выбором числа компонент
int buildMixture(const double* data, int n, int maxK, int robust, int numTries, MixtureParams* params) {
    if (n < 2 || maxK < 1) return -1;
    srand((unsigned)time(NULL));

    int bestK = 1;
    double bestBIC = 1e100;
    double* bestWeights = NULL;
    double* bestMeans = NULL;
    double* bestStddevs = NULL;
    double bestUnifWeight = 0.0;
    int bestParamsCount = 0;

    for (int k = 1; k <= maxK; ++k) {
        double* weights = (double*)malloc(k * sizeof(double));
        double* means = (double*)malloc(k * sizeof(double));
        double* stddevs = (double*)malloc(k * sizeof(double));
        if (!weights || !means || !stddevs) {
            if (weights) free(weights); if (means) free(means); if (stddevs) free(stddevs);
            continue;
        }
        double logLik = 0.0;
        int err;
        if (robust) {
            double unifWeight;
            err = em_robust_mixture_with_tries(data, n, k, weights, means, stddevs, &unifWeight, &logLik, numTries);
            if (err == 0) {
                // NOTE: число параметров: для k нормальных: 3k (веса, средние, сигмы) минус 1 связь весов (сумма=1) + 1 вес равномерной = 3k
                int nParams = 3 * k;
                double bic_val = bic(logLik, n, nParams);
                if (bic_val < bestBIC) {
                    bestBIC = bic_val;
                    bestK = k;
                    if (bestWeights) free(bestWeights);
                    if (bestMeans) free(bestMeans);
                    if (bestStddevs) free(bestStddevs);
                    bestWeights = weights;
                    bestMeans = means;
                    bestStddevs = stddevs;
                    bestUnifWeight = unifWeight;
                    bestParamsCount = nParams;
                    weights = means = stddevs = NULL;
                }
            }
        } else {
            err = em_normal_mixture_with_tries(data, n, k, weights, means, stddevs, &logLik, numTries);
            if (err == 0) {
                // NOTE: k весов (сумма=1) + k средних + k дисперсий
                int nParams = 3 * k - 1;
                double bic_val = bic(logLik, n, nParams);
                if (bic_val < bestBIC) {
                    bestBIC = bic_val;
                    bestK = k;
                    if (bestWeights) free(bestWeights);
                    if (bestMeans) free(bestMeans);
                    if (bestStddevs) free(bestStddevs);
                    bestWeights = weights;
                    bestMeans = means;
                    bestStddevs = stddevs;
                    bestParamsCount = nParams;
                    weights = means = stddevs = NULL;
                }
            }
        }
        if (weights) free(weights);
        if (means) free(means);
        if (stddevs) free(stddevs);
    }

    if (bestWeights == NULL) return -1;
    params->k = bestK;
    params->weights = bestWeights;
    params->means = bestMeans;
    params->stddevs = bestStddevs;
    params->uniformWeight = robust ? bestUnifWeight : 0.0;
    params->bic = bestBIC;
    return 0;
}

void freeMixtureParams(MixtureParams* params) {
    if (params) {
        if (params->weights) free(params->weights);
        if (params->means) free(params->means);
        if (params->stddevs) free(params->stddevs);
        memset(params, 0, sizeof(MixtureParams));
    }
}
#include "../include/UniversalApproximator.h"
#include "../include/NormalDistribution.h"
#include "../include/UniformDistribution.h"
#include "../include/mixture_builder.h"
#include <cstdlib>
#include <ctime>

namespace ModelingRandomValue::Approximation
{
    using Distribution::UniversalDistribution;

    UniversalApproximator::UniversalApproximator(Data::DataSet &ds, bool robust)
        : _dataSet(ds), _robust(robust), _isBatchObserver(true)
    {
        // Регистрируем себя как наблюдателя с batch-режимом
        // Для этого модифицируем DataSet: добавим метод attachBatch (или флаг).
        // Здесь предполагаем, что DataSet имеет метод attach(IObserver*, bool batch)
        // Реализуем это в DataSet позже.
        // _dataSet.attach(this, true);
        srand(static_cast<unsigned>(time(nullptr)));
        approximate();
    }

    UniversalApproximator::~UniversalApproximator()
    {
        // _dataSet.detach(this, true);
    }

    void UniversalApproximator::approximate()
    {
        const std::vector<double> &data = _dataSet.getData();
        int n = static_cast<int>(data.size());
        if (n < 2)
            return;

        MixtureParams params;
        int maxK = 5;
        int result = buildMixture(data.data(), n, maxK, _robust ? 1 : 0, &params);
        if (result != 0)
        {
            _model = std::make_unique<Distribution::Mixture>();
            return;
        }

        if (!_robust)
        {
            std::vector<double> weights(params.k);
            std::vector<UniversalDistribution> comps;
            for (int i = 0; i < params.k; ++i)
            {
                weights[i] = params.weights[i];
                comps.push_back(UniversalDistribution(
                    Distribution::NormalDistribution(params.means[i], params.stddevs[i])));
            }
            _model = std::make_unique<Distribution::Mixture>(weights, comps);
        }
        else
        {
            // Робастный случай
            double unifWeight = params.uniformWeight;
            double normalWeight = 1.0 - unifWeight;

            // Строим смесь нормальных компонент
            std::vector<double> normWeights(params.k);
            std::vector<UniversalDistribution> normComps;
            for (int i = 0; i < params.k; ++i)
            {
                normWeights[i] = params.weights[i];
                normComps.push_back(UniversalDistribution(
                    Distribution::NormalDistribution(params.means[i], params.stddevs[i])));
            }
            Distribution::Mixture normalMix(normWeights, normComps);
            UniversalDistribution univNormalMix(normalMix);

            // Равномерная компонента
            double minVal = *std::min_element(data.begin(), data.end());
            double maxVal = *std::max_element(data.begin(), data.end());
            Distribution::UniformDistribution uniform(minVal, maxVal);
            UniversalDistribution univUniform(uniform);

            // Верхний уровень смеси
            std::vector<double> topWeights = {unifWeight, normalWeight};
            std::vector<UniversalDistribution> topComps = {univUniform, univNormalMix};
            _model = std::make_unique<Distribution::Mixture>(topWeights, topComps);
        }

        freeMixtureParams(&params);
    }

    void UniversalApproximator::update()
    {
        // При пакетном обновлении перестраиваем модель
        approximate();
    }

    void UniversalApproximator::notifyBatchUpdate()
    {
        update();
    }

} // namespace
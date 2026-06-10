#include "../include/UniversalApproximator.h"
#include "../include/NormalDistribution.h"
#include "../include/UniformDistribution.h"
#include "../include/mixture_builder.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <numeric>

namespace ModelingRandomValue::Approximation
{
    using Distribution::Mixture;
    using Distribution::NormalDistribution;
    using Distribution::UniformDistribution;
    using Distribution::UniversalDistribution;
    using namespace std;

    UniversalApproximator::UniversalApproximator(Data::DataSet &ds, bool robust, int maxK, int numTries)
        : _dataSet(ds), _robust(robust), _isBatchObserver(true), _bic(1e300)
    {
        _dataSet.attach(this, true);
        srand(static_cast<unsigned>(time(nullptr)));
        setMaxK(maxK);
        setNumTries(numTries);
    }

    UniversalApproximator::~UniversalApproximator()
    {
        _dataSet.detach(this, true);
    }

    void UniversalApproximator::approximate()
    {
        const vector<double> &data = _dataSet.getData();
        int n = static_cast<int>(data.size());
        if (n < 2)
        {
            _model = make_unique<Mixture>();
            _bic = 1e300;
            return;
        }

        MixtureParams params;
        int result = buildMixture(data.data(), n, _maxK, _robust ? 1 : 0, _numTries, &params);
        if (result != 0)
        {
            _model = make_unique<Mixture>();
            _bic = 1e300;
            return;
        }
        _bic = params.bic;

        if (!_robust)
        {
            // NOTE: Обычная смесь нормальных
            vector<double> weights(params.k);
            vector<UniversalDistribution> comps;
            for (int i = 0; i < params.k; ++i)
            {
                weights[i] = params.weights[i];
                comps.push_back(UniversalDistribution(NormalDistribution(params.means[i], params.stddevs[i])));
            }
            _model = make_unique<Distribution::Mixture>(weights, comps);
        }
        else
        {
            // NOTE: Робастная смесь: двухуровневая
            double unifWeight = params.uniformWeight;
            double normalWeight = 1.0 - unifWeight;

            // NOTE: Проверка на корректность
            if (normalWeight < 1e-10 || params.k == 0)
            {
                // NOTE: Все наблюдения — выбросы, создаём только равномерную
                double minVal = *min_element(data.begin(), data.end());
                double maxVal = *max_element(data.begin(), data.end());
                double center = (minVal + maxVal) / 2.0;
                double width = maxVal - minVal;
                if (width < 1e-10)
                {
                    width = 1.0;
                }

                UniformDistribution uniform(center, width);
                UniversalDistribution univUniform(uniform);

                vector<double> topWeights = {1.0};
                vector<UniversalDistribution> topComps = {univUniform};
                _model = make_unique<Distribution::Mixture>(topWeights, topComps);
                freeMixtureParams(&params);
                return;
            }

            // NOTE: Пересчитываем условные веса для нормальных компонент
            vector<double> normWeights(params.k);
            vector<UniversalDistribution> normComps;
            for (int i = 0; i < params.k; ++i)
            {
                normWeights[i] = params.weights[i] / normalWeight; // ← ИСПРАВЛЕНО
                normComps.push_back(UniversalDistribution(
                    NormalDistribution(params.means[i], params.stddevs[i])));
            }

            // NOTE: Нормализуем на всякий случай (защита от ошибок округления)
            double sum = accumulate(normWeights.begin(), normWeights.end(), 0.0);
            if (fabs(sum - 1.0) > 1e-6)
            {
                for (auto &w : normWeights)
                    w /= sum;
            }

            // NOTE: Смесь нормальных компонент
            Distribution::Mixture normalMix(normWeights, normComps);
            UniversalDistribution univNormalMix(normalMix);

            // NOTE: Равномерная компонента
            double minVal = *min_element(data.begin(), data.end());
            double maxVal = *max_element(data.begin(), data.end());
            double center = (minVal + maxVal) / 2.0;
            double width = maxVal - minVal;
            if (width < 1e-10)
            {
                width = 1.0;
            }

            UniformDistribution uniform(center, width);
            UniversalDistribution univUniform(uniform);

            // NOTE: Верхний уровень
            vector<double> topWeights = {unifWeight, normalWeight};
            vector<UniversalDistribution> topComps = {univUniform, univNormalMix};

            _model = make_unique<Distribution::Mixture>(topWeights, topComps);
        }

        freeMixtureParams(&params);
    }

    void UniversalApproximator::update()
    {
        approximate();
    }

    void UniversalApproximator::notifyBatchUpdate()
    {
        update();
    }
}
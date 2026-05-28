#pragma once
#include "Mixture.h"
#include "ShallowMixtureIterator.h"
#include "UniversalDistribution.h"
#include <functional>

namespace ModelingRandomValue::Iterators
{

    /// @brief Внутренний итератор для глубокого обхода (перебор листьев)
    class DeepMixtureTraverser
    {
    public:
        using ProcessFunc = std::function<bool(Distribution::UniversalDistribution &, double)>;

    private:
        Distribution::Mixture &_mix;
        ProcessFunc _process;
        bool _result;

        void execute(ShallowMixtureIterator it, double prob)
        {
            for (it.first(); !it.isDone(); it.next())
            {
                auto &comp = it.currentItem().first;
                double w = it.currentItem().second;
                // NOTE: проверяем, является ли компонента смесью

                Distribution::Mixture *subMix = dynamic_cast<Distribution::Mixture *>(&comp.component());
                if (subMix)
                {
                    ShallowMixtureIterator subIt(*subMix);
                    execute(subIt, prob * w);
                }
                else if (_process)
                {
                    _result = _process(comp, prob * w);
                    if (!_result)
                        return;
                }
            }
        }

    public:
        DeepMixtureTraverser(Distribution::Mixture &mix) : _mix(mix), _result(true) {}
        DeepMixtureTraverser(Distribution::Mixture &mix, const ProcessFunc &f)
            : _mix(mix), _process(f), _result(true) {}

        /// @brief Устанавливает новую функцию обработки и запускает обход
        /// @param f Пользовательская функция, которая будет применена к каждому листу
        /// @return false, если какая-то из обработок вернула false, иначе true
        bool traverse(const ProcessFunc &f)
        {
            _process = f;
            return traverse();
        }

        /// @brief Запускает обход, используя ранее установленную функцию
        /// @return false, если какая-то из обработок вернула false, иначе true
        bool traverse()
        {
            if (!_process)
                throw std::runtime_error("process function not set");
            _result = true;
            ShallowMixtureIterator it(_mix);
            execute(it, 1.0);
            return _result;
        }
    };
}
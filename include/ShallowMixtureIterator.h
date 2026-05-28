#pragma once
#include "Mixture.h"
#include <functional>

namespace ModelingRandomValue::Iterators
{

    /// @brief Внешний итератор для поверхностного обхода компонент смеси
    class ShallowMixtureIterator
    {
        Distribution::Mixture &_mix;
        size_t _index;

    public:
        ShallowMixtureIterator(Distribution::Mixture &mix) : _mix(mix), _index(0) {}
        
        /// @brief Сбросить индекс итератора
        void first() { _index = 0; }
        
        /// @brief Перейти к следующему индексу итератора
        void next() { ++_index; }
        
        /// @brief Достигли ли мы конца
        bool isDone() const { return _index >= _mix.size(); }
        
        /// @brief Получить текущее значение 
        std::pair<Distribution::UniversalDistribution &, double> currentItem()
        {
            return {_mix.getComponent(_index), _mix.getWeight(_index)};
        }
    };
}
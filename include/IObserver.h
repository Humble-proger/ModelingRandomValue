#pragma once
#include "Common.h"
#define interface struct

namespace ModelingRandomValue::Interfaces
{
    /// @brief Интерфейс наблюдателя
    interface IObserver
    {
    public:
        /// @brief Метод обновления объектов
        virtual void update() = 0;
    };
};
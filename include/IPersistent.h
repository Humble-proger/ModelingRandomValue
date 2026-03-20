#pragma once
#include "Common.h"
#define interface struct

namespace ModelingRandomValue::Interfaces
{
    /// @brief Интерфейс персистентного объекта
    interface IPersistent
    {
    public:
        /// @brief Сохранить объект в файл
        /// @param out Ссылка на файл сохранения
        virtual void save(std::ostream & out) const = 0;

        /// @brief Читает объект из файла
        /// @param in Ссылка на файл загрузки
        virtual void load(std::istream & in) = 0;
    };
};

#pragma once
#include "interfaces.h"
#include <map>
#include <functional>
#include <string>

namespace ModelingRandomValue::Factories
{

    /// @brief Фабрика-синглтон распределений
    class DistributionFactory
    {
    public:
        using Creator = std::function<Interfaces::IDistribution *()>;

        /// @brief Экземпляр класса
        static DistributionFactory &instance()
        {
            static DistributionFactory factory;
            return factory;
        }

        /// @brief Зарегистрировать тип создаваемого объекта
        /// @param name Имя создаваемого распределения
        /// @param creator Функция создающая распределение
        void registerType(const std::string &name, Creator creator)
        {
            _creators[name] = creator;
        }

        /// @brief Создать объект по имени
        /// @param name Имя создаваемого распределения / Идентификатор распределения
        /// @return Экземпляр распределения
        Interfaces::IDistribution *create(const std::string &name) const
        {
            auto it = _creators.find(name);
            if (it != _creators.end())
            {
                return it->second();
            }
            throw std::runtime_error("Unknown distribution type: " + name);
        }

    private:
        DistributionFactory() = default;
        std::map<std::string, Creator> _creators;
    };
}
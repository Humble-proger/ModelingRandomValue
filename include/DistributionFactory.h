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
        static DistributionFactory* instance();

        /// @brief Зарегистрировать распределение
        /// @param distId Идентификатор распределения
        /// @param creator Функция создающая распределение
        /// @return Получилось ли зарегистрировать распределение
        bool registerDistribution(const std::string &distId, Creator creator);

        /// @brief Удалить распределение
        /// @param distId Идентификатор распределения
        /// @return Получилось ли удалить распределение
        bool unregisterDistribution(const std::string& distId);

        /// @brief Создать объект по имени
        /// @param name Имя создаваемого распределения / Идентификатор распределения
        /// @return Экземпляр распределения
        Interfaces::IDistribution *createDist(const std::string &name);

    private:
        using CallbackMap = std::map<std::string, Creator>;
        CallbackMap callbacks;
    
        DistributionFactory() = default;
        DistributionFactory(const DistributionFactory&) = delete;
        DistributionFactory& operator=(const DistributionFactory&) = delete;
        ~DistributionFactory() = default;
    };
}
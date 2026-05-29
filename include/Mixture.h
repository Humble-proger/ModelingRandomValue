#pragma once
#include "interfaces.h"
#include "UniversalDistribution.h"
#include <vector>
#include <random>

namespace ModelingRandomValue::Distribution
{

    /**
     * @brief Многослойная смесь распределений (паттерн Компоновщик)
     * Поддерживает интерфейсы IDistribution и IPersistent.
     * Компоненты хранятся как объекты UniversalDistribution.
     */
    class Mixture : public Interfaces::IDistribution, public Interfaces::IPersistent
    {
    private:
        std::vector<double> _weights;
        std::vector<UniversalDistribution> _components;
        double _loc = 0.0;
        double _scale = 1.0;
        mutable std::discrete_distribution<int> _disc;
        mutable bool _discValid = false;

        void updateDiscrete() const;

    public:
        Mixture() = default;
        
        /// @throw Проверяем условие суммы весов, если не выполняется выводим ошибку
        Mixture(const std::vector<double> &weights,
                const std::vector<UniversalDistribution> &comps);
        ~Mixture() = default;

        /// @brief  Добавить распределение в виде конверта в смесь.
        /// @note Веса автоматически не выравниваются под сумму 1. Веса необходимо выравнивать в ручную
        /// @param comp Распределение в виде конверта
        /// @param weight Вес распределения
        void add(const UniversalDistribution &comp, double weight);

        /// @brief Удалить распределение из смеси.
        /// @param index  Индекс распределения, которого необходимо удалить
        /// @note Веса автоматически не выравниваются под сумму 1. Веса необходимо выравнивать в ручную
        void remove(size_t index);

        /// @brief Количество распределений в смеси
        size_t size() const { return _components.size(); }

        /// @brief Получить отдельное конверт распределения из смеси
        /// @param index Индекс указывающий на необходимое распределение
        /// @return Конверт распределения
        UniversalDistribution &getComponent(size_t index) { return _components[index]; }

        /// @brief Получить отдельное конверт распределения из смеси
        /// @param index Индекс указывающий на необходимое распределение
        /// @return Конверт распределения
        const UniversalDistribution &getComponent(size_t index) const { return _components[index]; }

        /// @brief Получить установленный вес распределения в смеси
        /// @param index Индекс распределения
        /// @return Вес распределения
        double getWeight(size_t index) const { return _weights[index]; }

        /// @brief Установить вес распределения в смеси
        /// @param index Индекс распределения
        /// @param w Новый вес распределения
        /// @note При установке нового веса распределения в смеси, условие об сумме весов не проверяется и не выравнивается автоматически
        void setWeight(size_t index, double w);

        // NOTE: Реализация интерфейса  IDistribution
#pragma region IDistribution
        double density(double x) const override;
        double random() override;
        double mean() const override;
        double variance() const override;
        double skewness() const override;
        double kurtosis() const override;
        double getLocation() const override { return _loc; }
        double getScale() const override { return _scale; }
        void setLocation(double loc) override { _loc = loc; }
        void setScale(double s) override
        {
            if (s <= 0.0)
                throw std::invalid_argument("Scale must be positive");
            _scale = s;
        }
        IDistribution *clone() const override { return new Mixture(*this); }
        std::string name() const override { return "Mixture"; }
#pragma endregion

        // NOTE: Реализация интерфеса IPersistent
#pragma region IPersistent
        void save(std::ostream &out) const override;
        void load(std::istream &in) override;
#pragma endregion
    };

}
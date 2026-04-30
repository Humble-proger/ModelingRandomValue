#pragma once
#include "interfaces.h"
#include "DistributionFactory.h"

namespace ModelingRandomValue::Distribution
{
    /// @brief Конверт распределения
    class UniversalDistribution : public Interfaces::IDistribution,
                                  public Interfaces::IPersistent
    {
    private:
        Interfaces::IDistribution *_letter = nullptr;

    public:
        UniversalDistribution() = default;

        /// @brief Конструктор из любого распределения (избегаем матрёшки)
        explicit UniversalDistribution(const Interfaces::IDistribution &other)
        {
            if (typeid(other) == typeid(UniversalDistribution))
            {
                const auto &uni = dynamic_cast<const UniversalDistribution &>(other);
                _letter = uni._letter->clone();
            }
            else
            {
                _letter = other.clone();
            }
        }

        /// @brief Конструктор копирования
        UniversalDistribution(const UniversalDistribution &other)
        {
            if (other._letter)
                _letter = other._letter->clone();
        }

        /// @brief Оператор присваивания
        UniversalDistribution &operator=(const UniversalDistribution &other)
        {
            if (this != &other)
            {
                delete _letter;
                _letter = other._letter ? other._letter->clone() : nullptr;
            }
            return *this;
        }

        /// @brief Конструктор из потока (использует фабрику)
        explicit UniversalDistribution(std::ifstream &in)
        {
            load(in);
        }

        ~UniversalDistribution()
        {
            delete _letter;
        }

        /// @brief Доступ к письму
        Interfaces::IDistribution &component() { return *_letter; }
        const Interfaces::IDistribution &component() const { return *_letter; }

// NOTE: Реализация IDistribution
#pragma region IDistribution
        double density(double x) const override
        {
            return _letter->density(x);
        }

        double random() override
        {
            return _letter->random();
        }

        double mean() const override
        {
            return _letter->mean();
        }

        double variance() const override
        {
            return _letter->variance();
        }

        double skewness() const override
        {
            return _letter->skewness();
        }

        double kurtosis() const override
        {
            return _letter->kurtosis();
        }

        double getLocation() const override
        {
            return _letter->getLocation();
        }

        double getScale() const override
        {
            return _letter->getScale();
        }

        void setLocation(double loc) override
        {
            _letter->setLocation(loc);
        }

        void setScale(double scale) override
        {
            _letter->setScale(scale);
        }

        IDistribution *clone() const override
        {
            return new UniversalDistribution(*this);
        }

        std::string name() const override
        {
            return "UniversalDistribution";
        }
#pragma endregion

// NOTE: Реализация IPersistent
#pragma region IPersistent
        void save(std::ostream &out) const override
        {
            out << _letter->name() << std::endl;
            dynamic_cast<const IPersistent *>(_letter)->save(out);
        }

        void load(std::istream &in) override
        {
            delete _letter;
            std::string typeName;
            in >> typeName;
            _letter = Factories::DistributionFactory::instance()->createDist(typeName);
            dynamic_cast<IPersistent *>(_letter)->load(in);
        }
#pragma endregion
    };
}
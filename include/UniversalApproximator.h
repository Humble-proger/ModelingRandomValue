#pragma once
#include "interfaces.h"
#include "DataSet.h"
#include "Mixture.h"
#include "UniversalDistribution.h"

extern "C" {
    #include "mixture_builder.h"
}

namespace ModelingRandomValue::Approximation {

/// @brief Класс универсального аппроксиматора (паттерн Адаптер)
/// @note Связывает объектно-ориентированную часть с C-модулем EM-алгоритма.
class UniversalApproximator : public Interfaces::IObserver {
private:
    Data::DataSet& _dataSet;
    bool _robust;
    std::unique_ptr<Distribution::Mixture> _model;
    bool _isBatchObserver;
    int _maxK;

public:
    UniversalApproximator(Data::DataSet& ds, bool robust = false, int maxK = 5);
    ~UniversalApproximator();

    // NOTE: Запрет копирования
    UniversalApproximator(const UniversalApproximator&) = delete;
    UniversalApproximator& operator=(const UniversalApproximator&) = delete;

    void setMaxK(int maxK) 
    {
        if (maxK < 1) {
            throw std::invalid_argument("maxK must be >= 1");
        }
        _maxK = maxK;
        approximate();
    }

    int getMaxK() const { return _maxK; }
    
    /// @brief Построить модель по текущим данным (вызывается из update() и пользователем)
    void approximate();

    /// @brief Получить построенную модель (смесь)
    Distribution::Mixture& getModel() { return *_model; }
    const Distribution::Mixture& getModel() const { return *_model; }

    // NOTE: Реализация IObserver
    void update() override;

    /// @brief Принудительное уведомление (batch update)
    void notifyBatchUpdate();
};

}
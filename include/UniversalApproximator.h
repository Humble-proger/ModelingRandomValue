#pragma once
#include "interfaces.h"
#include "DataSet.h"
#include "Mixture.h"
#include "UniversalDistribution.h"

namespace ModelingRandomValue::Approximation {

/// Класс универсального аппроксиматора (паттерн Адаптер)
/// Связывает объектно-ориентированную часть с C-модулем EM-алгоритма.
class UniversalApproximator : public Interfaces::IObserver {
private:
    Data::DataSet& _dataSet;
    bool _robust;                           // робастный режим
    std::unique_ptr<Distribution::Mixture> _model;   // построенная модель
    bool _isBatchObserver;                  // зарегистрирован для batch-уведомлений

public:
    UniversalApproximator(Data::DataSet& ds, bool robust = false);
    ~UniversalApproximator();

    // Запрет копирования
    UniversalApproximator(const UniversalApproximator&) = delete;
    UniversalApproximator& operator=(const UniversalApproximator&) = delete;

    /// Построить модель по текущим данным (вызывается из update() и пользователем)
    void approximate();

    /// Получить построенную модель (смесь)
    Distribution::Mixture& getModel() { return *_model; }
    const Distribution::Mixture& getModel() const { return *_model; }

    // IObserver
    void update() override;

    /// Принудительное уведомление (batch update)
    void notifyBatchUpdate();
};

} // namespace
#include "../include/Mixture.h"
#include "../include/DistributionFactory.h"
#include <cmath>
#include <stdexcept>
#include <numeric>

namespace ModelingRandomValue::Distribution
{

    Mixture::Mixture(const std::vector<double> &weights,
                     const std::vector<UniversalDistribution> &comps)
        : _weights(weights), _components(comps)
    {
        if (_weights.size() != _components.size())
            throw std::invalid_argument("weights and components size mismatch");
        // NOTE: Проверяем условие суммы весов
        double sum = std::accumulate(_weights.begin(), _weights.end(), 0.0);
        if (std::fabs(sum - 1.0) > 1e-9)
            throw std::invalid_argument("weights must sum to 1");
        updateDiscrete();
    }

    void Mixture::updateDiscrete() const
    {
        if (_weights.empty())
            return;
        std::vector<double> probs(_weights.begin(), _weights.end());
        _disc = std::discrete_distribution<int>(probs.begin(), probs.end());
        _discValid = true;
    }

    void Mixture::add(const UniversalDistribution &comp, double weight)
    {
        _components.push_back(comp);
        _weights.push_back(weight);
        
        // NOTE: Не нормализируем веса, пользователь сам следит за условием
        _discValid = false;
    }

    void Mixture::remove(size_t index)
    {
        if (index >= _components.size())
            throw std::out_of_range("invalid index");
        _components.erase(_components.begin() + index);
        _weights.erase(_weights.begin() + index);
        // NOTE: Не нормализируем веса, пользователь сам следит за условием
        _discValid = false;
    }

    void Mixture::setWeight(size_t index, double w)
    {
        if (index >= _weights.size())
            throw std::out_of_range("invalid index");
        _weights[index] = w;
        // NOTE: Не нормализируем веса, пользователь сам следит за условием
        _discValid = false;
    }

    double Mixture::density(double x) const
    {
        // NOTE: Применяем преобразование сдвига/масштаба
        double xx = (x - _loc) / _scale;
        double sum = 0.0;
        for (size_t i = 0; i < _components.size(); ++i)
            sum += _weights[i] * _components[i].density(xx);
        return sum / _scale;
    }

    double Mixture::random()
    {
        if (_components.empty())
            throw std::runtime_error("empty mixture");
        if (!_discValid)
            updateDiscrete();
        // NOTE: Используем второй алгоритм для моделирования смеси
        int idx = _disc(generator);
        double val = _components[idx].random();
        return _loc + _scale * val;
    }

    double Mixture::mean() const
    {
        double m = 0.0;
        for (size_t i = 0; i < _components.size(); ++i)
            m += _weights[i] * _components[i].mean();
        return _loc + _scale * m;
    }

    double Mixture::variance() const
    {
        double m_base = 0.0, e2_base = 0.0;
        for (size_t i = 0; i < _components.size(); ++i)
        {
            double m_i = _components[i].mean();
            double v_i = _components[i].variance();
            m_base += _weights[i] * m_i;
            e2_base += _weights[i] * (v_i + m_i * m_i);
        }
        double var_base = e2_base - m_base * m_base;
        return _scale * _scale * var_base;
    }

    double Mixture::skewness() const
    {
        // NOTE: Вычисляем центральный момент 3-го порядка базовой смеси
        double m_base = 0.0;
        double var_base = 0.0;
        double e2_base = 0.0;
        for (size_t i = 0; i < _components.size(); ++i)
        {
            double m_i = _components[i].mean();
            double v_i = _components[i].variance();
            m_base += _weights[i] * m_i;
            e2_base += _weights[i] * (v_i + m_i * m_i);
        }
        var_base = e2_base - m_base * m_base;
        double sigma_base = std::sqrt(var_base);
        double mu3 = 0.0;
        for (size_t i = 0; i < _components.size(); ++i)
        {
            double m_i = _components[i].mean();
            double v_i = _components[i].variance();
            double s_i = _components[i].skewness();
            double mu3_i = s_i * std::pow(_components[i].variance(), 1.5); // E[(Xi-μi)^3]
            double delta = m_i - m_base;
            mu3 += _weights[i] * (mu3_i + 3 * delta * v_i + delta * delta * delta);
        }
        double skew_base = mu3 / (sigma_base * sigma_base * sigma_base);
        // NOTE: skewness инвариантна к сдвигу/масштабу
        return skew_base;
    }

    double Mixture::kurtosis() const
    {
        // NOTE: Аналогично, центральный момент 4-го порядка
        double m_base = 0.0, e2_base = 0.0;
        for (size_t i = 0; i < _components.size(); ++i)
        {
            double m_i = _components[i].mean();
            double v_i = _components[i].variance();
            m_base += _weights[i] * m_i;
            e2_base += _weights[i] * (v_i + m_i * m_i);
        }
        double var_base = e2_base - m_base * m_base;
        double sigma2_base = var_base;
        double sigma_base = std::sqrt(sigma2_base);
        double mu4 = 0.0;
        for (size_t i = 0; i < _components.size(); ++i)
        {
            double m_i = _components[i].mean();
            double v_i = _components[i].variance();
            double s_i = _components[i].skewness();
            double k_i = _components[i].kurtosis();
            double mu3_i = s_i * std::pow(v_i, 1.5);
            double mu4_i = (k_i + 3.0) * v_i * v_i; // E[(Xi-μi)^4] = (kurt+3)*σ^4
            double delta = m_i - m_base;
            mu4 += _weights[i] * (mu4_i + 4 * delta * mu3_i + 6 * delta * delta * v_i + delta * delta * delta * delta);
        }
        double kurt_base = mu4 / (sigma2_base * sigma2_base) - 3.0;
        // NOTE:  Аналогично, kurtosis инвариантна к сдвигу/масштабу
        return kurt_base;
    }

    void Mixture::save(std::ostream &out) const
    {
        out << _loc << " " << _scale << " " << _components.size() << "\n";
        for (size_t i = 0; i < _components.size(); ++i)
        {
            out << _weights[i] << " ";
            _components[i].save(out);
            out << "\n";
        }
    }

    void Mixture::load(std::istream &in)
    {
        size_t n;
        in >> _loc >> _scale >> n;
        _weights.resize(n);
        _components.clear();
        for (size_t i = 0; i < n; ++i)
        {
            double w;
            in >> w;
            _weights.push_back(w);
            UniversalDistribution comp;
            comp.load(in);
            _components.push_back(comp);
        }
        _discValid = false;
    }
}

namespace {
    using namespace ModelingRandomValue;
    Interfaces::IDistribution* CreateMixture() {
        return new Distribution::Mixture();
    }
    const bool registeredMixture = Factories::DistributionFactory::instance()
        ->registerDistribution("Mixture", CreateMixture);
}
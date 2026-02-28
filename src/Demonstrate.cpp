#include "../include/Demonstrate.h"

namespace ModelingRandomValue::Demonstrate
{
    void demonstrateDistributions()
    {
        printHeader("1. ДЕМОНСТРАЦИЯ РАСПРЕДЕЛЕНИЙ");

        UniformDistribution _uniform(0.0, 1.0);
        printDist("Равномерное распределение U(0,1)", _uniform, 0.5);

        NormalDistribution _normal(0.0, 1.0);
        printDist("Нормальное распределение N(0,1)", _normal, 0.0);

        LogisticDistribution _logistic(0.0, 1.0);
        printDist("Логистическое распределение Logistic(0,1)", _logistic, 0.0);

        UniformLogisticDistribution _smooth(1.0);
        printDist("Сглаженное равномерное (вариант 13) s=1.0", _smooth, 0.0);
    }

    void demonstrateSampling()
    {
        printHeader("2. ГЕНЕРАЦИЯ ВЫБОРОК И ПРОВЕРКА СХОДИМОСТИ");

        UniformLogisticDistribution _dist(1.0);

        vector<int> _sizes = {10, 100, 1000, 10000, 100000};

        printSubHeader("Теоретические характеристики");
        printValue("Среднее", _dist.mean());
        printValue("Дисперсия", _dist.variance());
        printValue("Коэф. асимметрии", _dist.skewness());
        printValue("Коэф. эксцесса", _dist.kurtosis());

        printSeparator('-', 95);
        printStringTable<string>({{"n", 10}, {"Mean (dif)", 15}, {"Variance (dif)", 15}, {"Skewness (dif)", 20}, {"Kurtosis (dif)", 20}, {"Time (ms)", 15}});
        printSeparator('-', 95);

        for (int _n : _sizes)
        {
            DataSet _sample;

            double _time = measureTime([&]()
                                       {
                for (int _i = 0; _i < _n; _i++) {
                    _sample.add(_dist.random());
                } });
            printStringTable<double>(
                {{_n, 10}, {abs(_sample.mean() - _dist.mean()), 15}, {abs(_sample.variance() - _dist.variance()), 15}, {abs(_sample.skewness() - _dist.skewness()), 20}, {abs(_sample.kurtosis() - _dist.kurtosis()), 20}, {round(_time * 100) / 100, 15}});
        }
    }

    void demonstrateDataSet()
    {
        printHeader("3. ДЕМОНСТРАЦИЯ КЛАССА DataSet");

        DataSet _dataSet;

        printSubHeader("Добавление элементов");
        _dataSet.add(1.5);
        _dataSet.add(2.7);
        _dataSet.add(3.2);
        _dataSet.add(4.8);
        _dataSet.add(5.1);

        printValue("Размер выборки", _dataSet.size());

        printSubHeader("Доступ по индексу");
        for (size_t _i = 0; _i < _dataSet.size(); _i++)
        {
            cout << string(2, ' ') << "data[" << _i << "]: " << _dataSet.get(_i) << endl;
        }

        printSubHeader("Изменение элемента");
        printText("Изменяем data[1] с 2.7 на 9.9");
        _dataSet.set(1, 9.9);
        printValue("data[1]", _dataSet.get(1));

        printSubHeader("Удаление элемента");
        printText("Удаляем data[2]");
        _dataSet.remove(2);
        printValue("Новый размер", _dataSet.size());

        printSubHeader("Вставка элемента");
        printText("Вставляем 7.7 на позицию 1");
        _dataSet.insert(1, 7.7);

        for (size_t _i = 0; _i < _dataSet.size(); _i++)
        {
            cout << string(2, ' ') << "data[" << _i << "]: " << _dataSet.get(_i) << endl;
        }

        printSubHeader("Выборочные характеристики");
        printDataStatistic(_dataSet);
    }

    void demonstrateHistogram()
    {
        printHeader("4. ДЕМОНСТРАЦИЯ КЛАССА Histogram");

        NormalDistribution _normal(0.0, 1.0);
        DataSet _dataSet;

        printSubHeader("Генерация 1000 значений из N(0,1)");
        for (int _i = 0; _i < 1000; _i++)
        {
            _dataSet.add(_normal.random());
        }

        Histogram _hist(_dataSet, 15);

        printSubHeader("Информация о гистограмме");
        printHistStatistic(_hist);

        printSubHeader("Значения эмпирической плотности");
        vector<double> _testPoints = {-2.0, -1.0, 0.0, 1.0, 2.0};

        for (double _x : _testPoints)
        {
            cout << string(2, ' ') << "f(" << _x << "): " << _hist.getEmpiricalDensity(_x) << endl;
            cout << string(2, ' ') << "f_теор(" << _x << "): " << _normal.density(_x) << endl << endl;
        }

        printSubHeader("Первые 5 столбцов гистограммы");
        auto _bounds = _hist.getColBounds();
        auto _densities = _hist.getDensities();

        for (int _i = 0; _i < 5; _i++)
        {
            cout << endl << string(2, ' ') << "Столбец " << _i << ": [" << _bounds[_i].first << ", " << _bounds[_i].second << "]" << endl;
            printValue("Плотность", _densities[_i]);
        }

        _hist.saveToFile("normal_histogram");
        saveTheoreticalDensity("normal_theoretical_densities", _normal, { _hist.getMinBound(), _hist.getMaxBound() }, 100);
        printText("\nДанные сохранены в normal_histogram.csv");
    }

    void demonstrateDensityComparison()
    {
        printHeader("5. СРАВНЕНИЕ ТЕОРЕТИЧЕСКОЙ И ЭМПИРИЧЕСКОЙ ПЛОТНОСТИ");

        UniformLogisticDistribution _dist(1.0);

        vector<int> _sizes = {100, 1000, 10000};

        for (int _n : _sizes)
        {
            printSubHeader("Объем выборки n = " + to_string(_n));

            DataSet _dataSet;
            for (int _i = 0; _i < _n; _i++)
            {
                _dataSet.add(_dist.random());
            }

            int _cols = static_cast<int>(1 + 3.322 * log10(_n));
            Histogram _hist(_dataSet, _cols);

            double _minVal = _dataSet.min();
            double _maxVal = _dataSet.max();
            double _step = (_maxVal - _minVal) / 100.0;

            double _totalError = 0.0;
            int _points = 0;

            for (double _x = _minVal; _x <= _maxVal; _x += _step)
            {
                double _theoretical = _dist.density(_x);
                double _empirical = _hist.getEmpiricalDensity(_x);
                _totalError += abs(_theoretical - _empirical);
                _points++;
            }

            double _meanError = _totalError / _points;
            printValue("Среднее абсолютное отклонение", _meanError);

            string _filename = "comparison_n" + to_string(_n);
            _hist.saveToFile(_filename);
            saveTheoreticalDensity(_filename + "_theoretical", _dist, { _hist.getMinBound(), _hist.getMaxBound() }, 100);
            printText("Данные сохранены в " + _filename + ".csv");
        }
    }

    void demonstratePersistence()
    {
        printHeader("6. ТЕСТИРОВАНИЕ СОХРАНЕНИЯ И ЗАГРУЗКИ РАСПРЕДЕЛЕНИЙ");

        UniformDistribution _uniform(2.0, 5.0);
        NormalDistribution _normal(5.0, 2.0);
        LogisticDistribution _logistic(10.0, 3.0);
        UniformLogisticDistribution _smooth(1.5);

        printSubHeader("Сохранение распределений");

        {
            ofstream _file("output/uniform_dist.txt");
            _uniform.save(_file);
            printText("Равномерное распределение сохранено в uniform_dist.txt");
        }

        {
            ofstream _file("output/normal_dist.txt");
            _normal.save(_file);
            printText("Нормальное распределение сохранено в normal_dist.txt");
        }

        {
            ofstream _file("output/logistic_dist.txt");
            _logistic.save(_file);
            printText("Логистическое распределение сохранено в logistic_dist.txt");
        }

        {
            ofstream _file("output/smooth_dist.txt");
            _smooth.save(_file);
            printText("Сглаженное равномерное сохранено в smooth_dist.txt");
        }

        printSubHeader("Загрузка распределений");

        UniformDistribution _loadedUniform(0.0, 1.0);
        {
            ifstream _file("output/uniform_dist.txt");
            _loadedUniform.load(_file);
            cout << string(2, ' ') << "Загружено равномерное: [" << _loadedUniform.getLower() << ", " << _loadedUniform.getUpper() << "]" << endl;
        }

        NormalDistribution _loadedNormal(0.0, 1.0);
        {
            ifstream _file("output/normal_dist.txt");
            _loadedNormal.load(_file);
            cout << string(2, ' ') << "Загружено нормальное: [" << _loadedNormal.getMean() <<  ", " << _loadedNormal.getStddev() << "]" << endl;
        }

        LogisticDistribution _loadedLogistic(0.0, 1.0);
        {
            ifstream _file("output/logistic_dist.txt");
            _loadedLogistic.load(_file);
            cout << string(2, ' ') << "Загружено логистическое: [" << _loadedLogistic.getLocation() <<  ", " << _loadedLogistic.getScale() << "]" << endl;
        }

        UniformLogisticDistribution _loadedSmooth(1.0);
        {
            ifstream _file("output/smooth_dist.txt");
            _loadedSmooth.load(_file);
            cout << string(2, ' ') << "Загружено сглаженное: s=" << _loadedSmooth.getScale() << endl;
        }
    }

    void demonstratePerformance()
    {
        printHeader("7. ТЕСТИРОВАНИЕ ПРОИЗВОДИТЕЛЬНОСТИ");

        UniformLogisticDistribution _dist(1.0);
        vector<int> _sizes = {10000, 100000, 1000000};

        printSeparator('-', 47);
        printStringTable<string>({{"Size", 12}, {"Time (ms)", 15}, {"Element/s", 20}});
        printSeparator('-', 47);

        for (int _n : _sizes)
        {
            double _time = measureTime([&]()
                                       {
                for (int i = 0; i < _n; i++) {
                    double x = _dist.random();
                } });

            double _speed = _n / (_time / 1000.0); // элементов в секунду

            printStringTable<double>({{_n, 12}, {round(_time * 100) / 100, 15}, {round(_speed), 20}});
        }
    }

    void demonstrateHistogramObserver()
    {
        printHeader("8. ДЕМОНСТРАЦИЯ ПАТТЕРНА НАБЛЮДАТЕЛЬ НА HISTOGRAM");

        printText("\n1. Создаем набор данных (субъект) с начальными значениями:", 0);
        DataSet _dataSet;

        _dataSet.add(1.2);
        _dataSet.add(2.5);
        _dataSet.add(3.7);
        _dataSet.add(4.1);
        _dataSet.add(5.3);

        cout << string(2, ' ') << "Начальные данные: ";
        for (size_t i = 0; i < _dataSet.size(); i++)
        {
            cout << _dataSet.get(i) << " ";
        }
        cout << endl;

        printText("\n2. Создаем гистограмму с 3 столбцами (наблюдатель)", 0);
        Histogram _hist(_dataSet, 3);

        printText("Начальное состояние гистограммы:");
        printHistStatistic(_hist, true);

        printText("\n3. Добавляем новый элемент 7.8 в набор данных (гистограмма должна обновиться автоматически)", 0);

        _dataSet.add(7.8);

        printText("Состояние гистограммы ПОСЛЕ добавления:");
        printHistStatistic(_hist, true);

        printText("\n4. Изменяем элемент data[2] с 3.7 на 10.5 (гистограмма должна обновиться автоматически)", 0);

        _dataSet.set(2, 10.5);

        printText("Состояние гистограммы ПОСЛЕ добавления:");
        printHistStatistic(_hist, true);

        printText("\n5. Демонстрация разницы между автоматическим и ручным обновлением:", 0);

        DataSet _dataSet2;
        _dataSet2.add(1.0);
        _dataSet2.add(2.0);
        _dataSet2.add(3.0);

        Histogram _autoHist(_dataSet2, 3);

        Histogram _manualHist(_dataSet2, 3);
        _dataSet2.detach(&_manualHist);

        printText("Начальное состояние (обе гистограммы одинаковы):", 0);
        printText("\nAutoHist:");
        printHistStatistic(_autoHist);
        printText("\nManualHist:");
        printHistStatistic(_manualHist);

        printText("\nДобавляем элемент 10.0 в набор данных...");
        _dataSet2.add(10.0);

        printText("\nAutoHist:");
        printHistStatistic(_autoHist);
        printText("\nManualHist:");
        printHistStatistic(_manualHist);
        printText("\n(ManualHist не изменилась - не получила уведомление)");

        printText("\nВызываем refresh() для ManualHist");
        _manualHist.refresh();

        printText("\nПосле ручного обновления:");
        printText("\nAutoHist:");
        printHistStatistic(_autoHist);
        printText("\nManualHist:");
        printHistStatistic(_manualHist);
        printText("\n(Теперь гистограммы снова одинаковы)");
    }
}
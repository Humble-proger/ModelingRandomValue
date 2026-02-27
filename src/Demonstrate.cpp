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

        // 1.4 Сглаженное равномерное (вариант 13)
        std::cout << "\n--- Сглаженное равномерное (вариант 13) s=1.0 ---" << std::endl;
        UniformLogisticDistribution smooth(1.0);
        printDist("Сглаженное равномерное (вариант 13) s=1.0", smooth, 0.0);
    }

    void demonstrateSampling()
    {
        printHeader("2. ГЕНЕРАЦИЯ ВЫБОРОК И ПРОВЕРКА СХОДИМОСТИ");

        UniformLogisticDistribution _dist(1.0);

        std::vector<int> _sizes = {10, 100, 1000, 10000, 100000};

        printSubHeader("Теоретические характеристики");
        printValue("Среднее", _dist.mean());
        printValue("Дисперсия", _dist.variance());
        printValue("Коэфф. асимметрии", _dist.skewness());
        printValue("Коэфф. эксцесса", _dist.kurtosis());

        printSeparator('-');
        printStringTable<string>({{"n", 10}, {"Среднее", 15}, {"Дисперсия", 15}, {"Коэфф. асимметрии", 15}, {"Коэфф. эксцесса", 15}, {"Время (мс)", 15}});
        printSeparator('-');

        for (int n : _sizes)
        {
            DataSet _sample;

            double _time = measureTime([&]()
                                       {
                for (int i = 0; i < n; i++) {
                    _sample.add(_dist.random());
                } });
            printStringTable<double>(
                {{n, 10}, {abs(_sample.mean() - _dist.mean()), 15}, {abs(_sample.variance() - _dist.variance()), 15}, {abs(_sample.skewness() - _dist.skewness()), 15}, {abs(_sample.kurtosis() - _dist.kurtosis()), 15}, {round(_time * 100) / 100, 15}});
        }
    }

    void demonstrateDataSet()
    {
        printHeader("3. ДЕМОНСТРАЦИЯ КЛАССА DataSet");

        // Создаем набор данных
        DataSet _dataSet;

        // Добавляем элементы
        printSubHeader("Добавление элементов");
        _dataSet.add(1.5);
        _dataSet.add(2.7);
        _dataSet.add(3.2);
        _dataSet.add(4.8);
        _dataSet.add(5.1);

        printValue("Размер выборки", _dataSet.size());

        // Получаем элементы по индексу
        printSubHeader("Доступ по индексу");
        for (size_t i = 0; i < _dataSet.size(); i++)
        {
            printValue("data[" + to_string(i) + "]", _dataSet.get(i));
        }

        // Изменяем элемент
        printSubHeader("Изменение элемента");
        printText("Изменяем data[1] с 2.7 на 9.9");
        _dataSet.set(1, 9.9);
        printValue("data[1]", _dataSet.get(1));

        // Удаляем элемент
        printSubHeader("Удаление элемента");
        printText("Удаляем data[2]");
        _dataSet.remove(2);
        printValue("Новый размер", _dataSet.size());

        // Вставка элемента
        printSubHeader("Вставка элемента");
        printText("Вставляем 7.7 на позицию 1");
        _dataSet.insert(1, 7.7);

        for (size_t i = 0; i < _dataSet.size(); i++)
        {
            printValue("data[" + to_string(i) + "]", _dataSet.get(i));
        }

        // Вычисление характеристик
        printSubHeader("Выборочные характеристики");
        printDataStatistic(_dataSet);
    }

    void demonstrateHistogram()
    {
        printHeader("4. ДЕМОНСТРАЦИЯ КЛАССА Histogram");

        // Создаем набор данных с нормальным распределением
        NormalDistribution _normal(0.0, 1.0);
        DataSet _dataSet;

        // Генерируем 1000 значений
        printSubHeader("Генерация 1000 значений из N(0,1)");
        for (int i = 0; i < 1000; i++)
        {
            _dataSet.add(_normal.random());
        }

        // Создаем гистограмму с 15 столбцами
        Histogram _hist(_dataSet, 15);

        // Выводим информацию о гистограмме
        printSubHeader("Информация о гистограмме");
        printHistStatistic(_hist);

        // Проверяем эмпирическую плотность в разных точках
        printSubHeader("Значения эмпирической плотности");
        vector<double> testPoints = {-2.0, -1.0, 0.0, 1.0, 2.0};

        for (double x : testPoints)
        {
            printValue("f(" + to_string(x) + ")", _hist.getEmpiricalDensity(x));
            printValue("f_теор(" + to_string(x) + ")", _normal.density(x));
        }

        // Получаем границы столбцов
        printSubHeader("Первые 5 столбцов гистограммы");
        auto _bounds = _hist.getBinBounds();
        auto _densities = _hist.getDensities();

        for (int i = 0; i < 5; i++)
        {
            printText("Столбец " + to_string(i) + ": [" + to_string(_bounds[i].first) + ", " + to_string(_bounds[i].second) + "]");
            printValue("Плотность", _densities[i]);
        }

        // Сохраняем данные для построения графика
        _hist.saveToFile("normal_histogram.csv");
        printText("Данные сохранены в normal_histogram.csv");
    }

    void demonstrateDensityComparison()
    {
        printHeader("5. СРАВНЕНИЕ ТЕОРЕТИЧЕСКОЙ И ЭМПИРИЧЕСКОЙ ПЛОТНОСТИ");

        // Используем распределение из варианта 13
        UniformLogisticDistribution _dist(1.0);

        vector<int> _sizes = {100, 1000, 10000};

        for (int _n : _sizes)
        {
            printSubHeader("Объем выборки n = " + to_string(_n));

            // Генерируем выборку
            DataSet _dataSet;
            for (int i = 0; i < _n; i++)
            {
                _dataSet.add(_dist.random());
            }

            // Создаем гистограмму (по формуле Стёрджеса)
            int _cols = static_cast<int>(1 + 3.322 * log10(_n));
            Histogram _hist(_dataSet, _cols);

            // Вычисляем среднее абсолютное отклонение
            double _minVal = _dataSet.min();
            double _maxVal = _dataSet.max();
            double _step = (_maxVal - _minVal) / 100.0;

            double _totalError = 0.0;
            int _points = 0;

            for (double x = _minVal; x <= _maxVal; x += _step)
            {
                double _theoretical = _dist.density(x);
                double _empirical = _hist.getEmpiricalDensity(x);
                _totalError += abs(_theoretical - _empirical);
                _points++;
            }

            double _meanError = _totalError / _points;
            printValue("Среднее абсолютное отклонение", _meanError);

            // Сохраняем данные для графика
            string filename = "comparison_n" + to_string(_n);
            _hist.saveToFile(filename);
            printText("Данные сохранены в " + filename + ".csv");
        }
    }

    void demonstratePersistence()
    {
        printHeader("6. ТЕСТИРОВАНИЕ СОХРАНЕНИЯ И ЗАГРУЗКИ РАСПРЕДЕЛЕНИЙ");

        // Создаем распределения
        UniformDistribution _uniform(2.0, 5.0);
        NormalDistribution _normal(5.0, 2.0);
        LogisticDistribution _logistic(10.0, 3.0);
        UniformLogisticDistribution _smooth(1.5);

        // Сохраняем в файлы
        printSubHeader("Сохранение распределений");

        {
            ofstream file("uniform_dist.txt");
            _uniform.save(file);
            printText("Равномерное распределение сохранено в uniform_dist.txt");
        }

        {
            ofstream file("normal_dist.txt");
            _normal.save(file);
            printText("Нормальное распределение сохранено в normal_dist.txt");
        }

        {
            ofstream file("logistic_dist.txt");
            _logistic.save(file);
            printText("Логистическое распределение сохранено в logistic_dist.txt");
        }

        {
            ofstream file("smooth_dist.txt");
            _smooth.save(file);
            printText("Сглаженное равномерное сохранено в smooth_dist.txt");
        }

        // Загружаем из файлов
        printSubHeader("Загрузка распределений");

        UniformDistribution _loadedUniform(0.0, 1.0);
        {
            ifstream _file("uniform_dist.txt");
            _loadedUniform.load(_file);
            printText("Загружено равномерное: [" + to_string(_loadedUniform.getLower()) + ", " + to_string(_loadedUniform.getUpper()) + "]");
        }

        NormalDistribution _loadedNormal(0.0, 1.0);
        {
            std::ifstream _file("normal_dist.txt");
            _loadedNormal.load(_file);
            printText("Загружено нормальное: [" + to_string(_loadedNormal.getMean()) + ", " + to_string(_loadedNormal.getStddev()) + "]");
        }

        LogisticDistribution _loadedLogistic(0.0, 1.0);
        {
            std::ifstream _file("logistic_dist.txt");
            _loadedLogistic.load(_file);
            printText("Загружено логистическое: [" + to_string(_loadedLogistic.getLocation()) + ", " + to_string(_loadedLogistic.getScale()) + "]");
        }

        UniformLogisticDistribution _loadedSmooth(1.0);
        {
            std::ifstream _file("smooth_dist.txt");
            _loadedSmooth.load(_file);
            printText("Загружено сглаженное: s=" + to_string(_loadedSmooth.getScale()));
        }
    }

    void demonstratePerformance()
    {
        printHeader("7. ТЕСТИРОВАНИЕ ПРОИЗВОДИТЕЛЬНОСТИ");

        UniformLogisticDistribution _dist(1.0);
        vector<int> _sizes = {10000, 100000, 1000000};

        printSeparator('-');
        printStringTable<string>({{"Размер", 12}, {"Время (мс)", 15}, {"Элементов/с", 15}});

        for (int _n : _sizes)
        {
            double _time = measureTime([&]()
                                       {
                for (int i = 0; i < _n; i++) {
                    volatile double x = _dist.random();
                } });

            double _speed = _n / (_time / 1000.0); // элементов в секунду

            printStringTable<double>({{_n, 12}, {round(_time * 100) / 100, 15}, {round(_speed), 15}});
        }
    }

    void demonstrateHistogramObserver()
    {
        printHeader("8. ДЕМОНСТРАЦИЯ ПАТТЕРНА НАБЛЮДАТЕЛЬ НА HISTOGRAM");

        // 1. СОЗДАЕМ НАБОР ДАННЫХ (СУБЪЕКТ)
        printText("1. Создаем набор данных (субъект) с начальными значениями:", 0);
        DataSet _dataSet;

        // Добавляем начальные данные
        _dataSet.add(1.2);
        _dataSet.add(2.5);
        _dataSet.add(3.7);
        _dataSet.add(4.1);
        _dataSet.add(5.3);

        cout << setw(2) << "Начальные данные: ";
        for (size_t i = 0; i < _dataSet.size(); i++)
        {
            cout << _dataSet.get(i) << " ";
        }
        std::cout << std::endl;

        // 2. СОЗДАЕМ ГИСТОГРАММУ (НАБЛЮДАТЕЛЬ)
        printText("2. Создаем гистограмму с 3 столбцами (наблюдатель)", 0);
        Histogram _hist(_dataSet, 3);

        printText("Начальное состояние гистограммы:");
        printHistStatistic(_hist, true);

        // 3. ДОБАВЛЯЕМ НОВЫЙ ЭЛЕМЕНТ - ГИСТОГРАММА АВТОМАТИЧЕСКИ ОБНОВЛЯЕТСЯ
        printText("3. Добавляем новый элемент 7.8 в набор данных (гистограмма должна обновиться автоматически)", 0);

        _dataSet.add(7.8);

        printText("Состояние гистограммы ПОСЛЕ добавления:");
        printHistStatistic(_hist, true);

        // 4. ИЗМЕНЯЕМ СУЩЕСТВУЮЩИЙ ЭЛЕМЕНТ
        printText("4. Изменяем элемент data[2] с 3.7 на 10.5 (гистограмма должна обновиться автоматически)", 0);

        _dataSet.set(2, 10.5);

        printText("Состояние гистограммы ПОСЛЕ добавления:");
        printHistStatistic(_hist, true);

        // 5. ДЕМОНСТРАЦИЯ РАЗНИЦЫ МЕЖДУ АВТО И РУЧНЫМ РЕЖИМОМ
        printText("5. Демонстрация разницы между автоматическим и ручным обновлением:", 0);

        DataSet _dataSet2;
        _dataSet2.add(1.0);
        _dataSet2.add(2.0);
        _dataSet2.add(3.0);

        Histogram _autoHist(_dataSet2, 3);

        Histogram _manualHist(_dataSet2, 3);
        _dataSet2.detach(&_manualHist);

        printText("Начальное состояние (обе гистограммы одинаковы):", 0);
        printText("AutoHist:");
        printHistStatistic(_autoHist);
        printText("ManualHist:");
        printHistStatistic(_manualHist);

        // Добавляем новый элемент
        printText("Добавляем элемент 10.0 в набор данных...");
        _dataSet2.add(10.0);

        printText("AutoHist:");
        printHistStatistic(_autoHist);
        printText("ManualHist:");
        printHistStatistic(_manualHist);
        printText("(ManualHist не изменилась - не получила уведомление)");

        // Ручное обновление
        printText("Вызываем refresh() для ManualHist");
        _manualHist.refresh();

        printText("После ручного обновления:");
        printText("AutoHist:");
        printHistStatistic(_autoHist);
        printText("ManualHist:");
        printHistStatistic(_manualHist);
        printText("(Теперь гистограммы снова одинаковы)");
    }
}
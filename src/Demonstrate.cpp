#include "../include/Demonstrate.h"
#include "../include/DistributionFactory.h"
#include "../include/UniversalDistribution.h"

namespace ModelingRandomValue::Demonstrate
{
    using namespace AdditionalFunc;
    using namespace Data;
    using namespace Observers;
    using namespace Distribution;
    using namespace Factories;
    using namespace Interfaces;

    void demonstrateDistributions()
    {
        printHeader("1. ДЕМОНСТРАЦИЯ РАСПРЕДЕЛЕНИЙ");

        UniformDistribution _uniform(0.0, 1.0);
        printDist("Равномерное распределение U(0,1)", _uniform, 0.5);
        locScaleConverting(_uniform);
        valuesDistributionCharacteristics(_uniform);

        NormalDistribution _normal(0.0, 1.0);
        printDist("Нормальное распределение N(0,1)", _normal, 0.0);
        locScaleConverting(_normal);
        valuesDistributionCharacteristics(_normal);

        LogisticDistribution _logistic(0.0, 1.0);
        printDist("Логистическое распределение Logistic(0,1)", _logistic, 0.0);
        locScaleConverting(_logistic);
        valuesDistributionCharacteristics(_logistic);

        UniformLogisticDistribution _smooth(0.0, 1.0, 1.0);
        printDist("Сглаженное равномерное (вариант 13) loc=0.0, s=1.0", _smooth, 0.0);
        locScaleConverting(_smooth);
        valuesDistributionCharacteristicsShape(_smooth);
    }

    void demonstrateSampling()
    {
        printHeader("2. ГЕНЕРАЦИЯ ВЫБОРОК И ПРОВЕРКА СХОДИМОСТИ");

        UniformLogisticDistribution _dist(3.0, 2.0, 3.0);

        vector<int> _sizes = {10, 100, 1000, 10000, 100000};

        printSubHeader("Сглаженное равномерное (вариант 13) loc=3.0 s=2.0, shape=3.0");
        printText("Теоретические характеристики");
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

        LogisticDistribution _logical(0.0, 1.0);
        DataSet _dataSet;

        printSubHeader("Генерация 1000 значений из Logic(0,1)");
        for (int _i = 0; _i < 1000; _i++)
        {
            _dataSet.add(_logical.random());
        }

        Histogram _hist(_dataSet, 15);

        printSubHeader("Информация о гистограмме");
        printHistStatistic(_hist);

        printSubHeader("Значения эмпирической плотности");
        vector<double> _testPoints = {-2.0, -1.0, 0.0, 1.0, 2.0};

        for (double _x : _testPoints)
        {
            cout << string(2, ' ') << "f(" << _x << "): " << _hist.getEmpiricalDensity(_x) << endl;
            cout << string(2, ' ') << "f_теор(" << _x << "): " << _logical.density(_x) << endl
                 << endl;
        }

        printSubHeader("Первые 5 столбцов гистограммы");
        auto _bounds = _hist.getColBounds();
        auto _densities = _hist.getDensities();

        for (int _i = 0; _i < 5; _i++)
        {
            cout << endl
                 << string(2, ' ') << "Столбец " << _i << ": [" << _bounds[_i].first << ", " << _bounds[_i].second << "]" << endl;
            printValue("Плотность", _densities[_i]);
        }

        _hist.saveToFile("logical_n15");
        printText("\nДанные сохранены в logical_n15.csv");
        saveTheoreticalDensity("logical_n15_theoretical", _logical, {_hist.getMinBound(), _hist.getMaxBound()}, 100);
        printText("Данные о теоретической плотности сохранены в logical_theoretical_n15.csv");
    }

    void demonstrateDensityComparison()
    {
        printHeader("5. СРАВНЕНИЕ ТЕОРЕТИЧЕСКОЙ И ЭМПИРИЧЕСКОЙ ПЛОТНОСТИ");

        UniformLogisticDistribution _uniformLogical(1.0, 3.0, 2.0);
        UniformDistribution _uniform(-2.0, 5.0);
        NormalDistribution _normal(3.0, 4.0);

        vector<pair<size_t, size_t>> _sizes = {{100, 7}, {1000, 20}, {10000, 40}, {10000000, 100}};

        printSubHeader("Сглаженное равномерное (вариант 13) loc=1.0, s=3.0, shape=2.0");
        compareDensity("smooth", _uniformLogical, _sizes);

        printSubHeader("Нормальное распределение N(3,4)");
        compareDensity("norm", _normal, _sizes);

        printSubHeader("Равномерное распределение U(-2,5)");
        compareDensity("uniform", _uniform, _sizes);
    }

    void demonstratePersistence()
    {
        printHeader("6. ТЕСТИРОВАНИЕ СОХРАНЕНИЯ И ЗАГРУЗКИ РАСПРЕДЕЛЕНИЙ");

        UniformDistribution _uniform(2.0, 5.0);
        NormalDistribution _normal(5.0, 2.0);
        LogisticDistribution _logistic(10.0, 3.0);
        UniformLogisticDistribution _smooth(3.1, 1.5, 2.0);

        printSubHeader("Сохранение распределений");

        {
            ofstream _file("output/uniform_dist.txt");
            _uniform.save(_file);
            printText("Равномерное распределение [2, 5] сохранено в uniform_dist.txt");
        }

        {
            ofstream _file("output/normal_dist.txt");
            _normal.save(_file);
            printText("Нормальное распределение [5, 2] сохранено в normal_dist.txt");
        }

        {
            ofstream _file("output/logistic_dist.txt");
            _logistic.save(_file);
            printText("Логистическое распределение [10, 3] сохранено в logistic_dist.txt");
        }

        {
            ofstream _file("output/smooth_dist.txt");
            _smooth.save(_file);
            printText("Сглаженное равномерное [3.1, 1.5, 2.0] сохранено в smooth_dist.txt");
        }

        printSubHeader("Загрузка распределений");

        UniformDistribution _loadedUniform(0.0, 1.0);
        {
            ifstream _file("output/uniform_dist.txt");
            _loadedUniform.load(_file);
            cout << string(2, ' ') << "Загружено равномерное: [" << _loadedUniform.getLocation() << ", " << _loadedUniform.getScale() << "]" << endl;
        }

        NormalDistribution _loadedNormal(0.0, 1.0);
        {
            ifstream _file("output/normal_dist.txt");
            _loadedNormal.load(_file);
            cout << string(2, ' ') << "Загружено нормальное: [" << _loadedNormal.getLocation() << ", " << _loadedNormal.getScale() << "]" << endl;
        }

        LogisticDistribution _loadedLogistic(0.0, 1.0);
        {
            ifstream _file("output/logistic_dist.txt");
            _loadedLogistic.load(_file);
            cout << string(2, ' ') << "Загружено логистическое: [" << _loadedLogistic.getLocation() << ", " << _loadedLogistic.getScale() << "]" << endl;
        }

        UniformLogisticDistribution _loadedSmooth(0.0, 1.0);
        {
            ifstream _file("output/smooth_dist.txt");
            _loadedSmooth.load(_file);
            cout << string(2, ' ') << "Загружено сглаженное: [" << _loadedSmooth.getLocation() << ", " << _loadedSmooth.getScale() << ", " << _loadedSmooth.getShape() << "]" << endl;
        }
    }

    void demonstratePerformance()
    {
        printHeader("7. ТЕСТИРОВАНИЕ ПРОИЗВОДИТЕЛЬНОСТИ");

        UniformLogisticDistribution _dist(3.0, 5.0, 2.0);
        vector<int> _sizes = {10000, 100000, 1000000, 10000000};

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

    void demonstrateVirtualConstructorsAndEnvelope()
    {
        printHeader("ДЕМОНСТРАЦИЯ ВИРТУАЛЬНЫХ КОНСТРУКТОРОВ И ИДИОМЫ");

        DistributionFactory* factory = DistributionFactory::instance();
        
        printSubHeader("1. Демонстрация clone() и name() для каждого распределения");

        vector<unique_ptr<IDistribution>> originals;
        originals.push_back(make_unique<UniformDistribution>(0.0, 2.0));
        originals.push_back(make_unique<NormalDistribution>(5.0, 1.5));
        originals.push_back(make_unique<LogisticDistribution>(-2.0, 0.8));
        originals.push_back(make_unique<UniformLogisticDistribution>(3.0, 1.2));

        for (const auto &dist : originals)
        {
            printText("Оригинал: " + dist->name());
            printValue("location", dist->getLocation(), 6);
            printValue("scale", dist->getScale(), 6);

            printText("Клонируем...");
            printText("Результат:");
            unique_ptr<IDistribution> clone(dist->clone());
            printText("Клон: " + clone->name(), 6);
            printValue("location (клона)", clone->getLocation(), 6);
            printValue("scale (клона)", clone->getScale(), 6);

            printText("Изменяем клон...");
            printText("Результат:");
            clone->setLocation(clone->getLocation() + 10.0);
            printText("После изменения клона (location += 10):", 6);
            printValue("оригинал location", dist->getLocation(), 6);
            printValue("клон location", clone->getLocation(), 6);
            cout << endl;
        }

        printSubHeader("2. Создание объектов через фабрику по строковому имени");

        vector<string> typeNames = {
            "UniformDistribution",
            "NormalDistribution",
            "LogisticDistribution",
            "UniformLogisticDistribution"};

        for (const auto &name : typeNames)
        {
            printText("Создание объекта типа: " + name);
            unique_ptr<IDistribution> dist(factory->createDist(name));

            printText("Изменяем параметры объекта loc -> 1, scale -> 2");
            printText("Результат:");
            dist->setLocation(1.0);
            dist->setScale(2.0);

            printValue("location", dist->getLocation(), 6);
            printValue("scale", dist->getScale(), 6);
            printValue("mean", dist->mean(), 6);
            printValue("variance", dist->variance(), 6);
            cout << endl;
        }

        printSubHeader("3. Демонстрация класса UniversalDistribution (конверт/письмо)");

        UniformDistribution uniform(2.0, 3.0);
        UniversalDistribution envelope1(uniform);

        printText("Создан конверт с UniformDistribution(2.0, 3.0)");
        printText("name() конверта: " + envelope1.name(), 6);
        printText("name() письма: " + envelope1.component().name(), 6);
        printValue("location письма", envelope1.getLocation(), 6);
        printValue("scale письма", envelope1.getScale(), 6);
        printValue("mean()", envelope1.mean(), 6);
        printValue("random()", envelope1.random(), 6);

        printText("\nЗамена письма на NormalDistribution(0.0, 1.0)");
        NormalDistribution normal(0.0, 1.0);
        envelope1 = UniversalDistribution(normal);

        printValue("name() письма после замены: " + envelope1.component().name(), 6);
        printValue("location письма", envelope1.getLocation(), 6);
        printValue("mean()", envelope1.mean(), 6);

        printSubHeader("4. Предотвращение эффекта \"матрёшки\"");

        // NOTE: Создаем конверт из конверта
        UniversalDistribution envelope2(envelope1);
        printText("Создан конверт envelope2 из envelope1 (который сам является конвертом)");
        printText("name() envelope2: " + envelope2.name(), 6);
        printText("name() письма envelope2: " + envelope2.component().name(), 6);
        printText("(Если бы не было защиты, получился бы конверт внутри конверта)", 6);

        printSubHeader("5. Сохранение и загрузка UniversalDistribution");

        UniformLogisticDistribution smooth(3.1, 1.5);
        UniversalDistribution original(smooth);

        printText("Оригинальный конверт с UniformLogisticDistribution(3.1, 1.5)");
        printValue("location", original.getLocation(), 6);
        printValue("scale", original.getScale(), 6);

        {
            ofstream file("output/universal_dist_demo.txt");
            if (!file.is_open())
            {
                printText("Ошибка: не удалось открыть файл для записи");
            }
            else
            {
                original.save(file);
                printText("Объект сохранён в output/universal_dist_demo.txt");
            }
        }

        UniversalDistribution loaded;
        {
            ifstream file("output/universal_dist_demo.txt");
            if (!file.is_open())
            {
                printText("Ошибка: не удалось открыть файл для чтения");
            }
            else
            {
                loaded.load(file);
                printText("Объект загружен из файла");
            }
        }

        printText("\nСравнение оригинала и загруженного объекта:");
        printValue("Оригинал location", original.getLocation(), 6);
        printValue("Загруженный location", loaded.getLocation(), 6);
        printValue("Оригинал scale", original.getScale(), 6);
        printValue("Загруженный scale", loaded.getScale(), 6);
        printValue("Оригинал mean()", original.mean(), 6);
        printValue("Загруженный mean()", loaded.mean(), 6);
        printText("Тип письма оригинала: " + original.component().name(), 6);
        printText("Тип письма загруженного: " + loaded.component().name(), 6);

        printSubHeader("6. Сравнение производительности");

        const int N = 1000000;
        UniformLogisticDistribution directDist(0.0, 1.0);
        UniversalDistribution envelopeDist(directDist);

        double timeDirect = measureTime([&]()
                                        {
        double sum = 0.0;
        for (int i = 0; i < N; ++i) {
            sum += directDist.random();
        } });

        double timeEnvelope = measureTime([&]()
                                          {
        double sum = 0.0;
        for (int i = 0; i < N; ++i) {
            sum += envelopeDist.random();
        } });

        printText("Генерация " + to_string(N) + " случайных чисел:");
        printValue("Прямой вызов (мс)", timeDirect, 6);
        printValue("Через конверт (мс)", timeEnvelope, 6);
        printValue("Накладные расходы (%)", (timeEnvelope / timeDirect - 1.0) * 100.0, 6);

        printSubHeader("7. Полиморфный контейнер с UniversalDistribution");

        vector<UniversalDistribution> container;

        container.emplace_back(UniformDistribution(-1.0, 2.0));
        container.emplace_back(NormalDistribution(0.0, 1.0));
        container.emplace_back(LogisticDistribution(2.0, 0.5));
        container.emplace_back(UniformLogisticDistribution(1.0, 1.5));

        printText("Контейнер содержит объекты разных типов распределений:");
        printSeparator('-', 60);
        printStringTable<string>({{"Тип", 30}, {"location", 12}, {"scale", 10}, {"mean", 12}});
        printSeparator('-', 60);

        for (size_t i = 0; i < container.size(); ++i)
        {
            const auto &item = container[i];
            printStringTable<string>({{item.component().name(), 30},
                                      {to_string(item.getLocation()).substr(0, 8), 12},
                                      {to_string(item.getScale()).substr(0, 8), 10},
                                      {to_string(item.mean()).substr(0, 10), 12}});
        }
        printSeparator('-', 60);

        printText("\nСохранение всего контейнера в файл container_demo.txt");
        {
            ofstream file("output/container_demo.txt");
            file << container.size() << endl;
            for (const auto &item : container)
            {
                item.save(file);
            }
            printText("Контейнер сохранён");
        }

        printText("\nЗагрузка контейнера из файла:");
        {
            ifstream file("output/container_demo.txt");
            size_t count;
            file >> count;
            file.ignore(); // NOTE: Пропускаем перевод строки

            vector<UniversalDistribution> loadedContainer;
            for (size_t i = 0; i < count; ++i)
            {
                loadedContainer.emplace_back(file);
            }

            printText("Загруженные объекты:");
            printSeparator('-', 60);
            printStringTable<string>({{"Тип", 30}, {"location", 12}, {"scale", 10}});
            printSeparator('-', 60);
            for (const auto &item : loadedContainer)
            {
                printStringTable<string>({{item.component().name(), 30},
                                          {to_string(item.getLocation()).substr(0, 8), 12},
                                          {to_string(item.getScale()).substr(0, 8), 10}});
            }
            printSeparator('-', 60);
        }
    }
}
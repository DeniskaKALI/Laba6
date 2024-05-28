#include <iostream>
#include <list>
#include <string>
#include <chrono>
#include <iomanip>

// Структура для представления файла
struct File {
    std::string name;                           // Имя файла
    std::chrono::system_clock::time_point creationDate; // Дата создания файла
    int accessCount;                            // Количество обращений к файлу

    // Конструктор для удобства создания объектов
    File(const std::string& _name, const std::chrono::system_clock::time_point& _creationDate)
        : name(_name), creationDate(_creationDate), accessCount(0) {}

    // Перегрузка оператора вывода для удобного вывода информации о файле
    friend std::ostream& operator<<(std::ostream& os, const File& file) {
        auto time = std::chrono::system_clock::to_time_t(file.creationDate);
        std::tm tm;
#ifdef _WIN32
        localtime_s(&tm, &time);
#else
        localtime_r(&time, &tm); // Преобразуем время в tm struct в локальное время
#endif

        // Используем put_time для форматированного вывода даты
        os << "Имя: " << file.name << ", Дата создания: "
            << std::put_time(&tm, "%Y-%m-%d") << ", Количество обращений: " << file.accessCount;
        return os;
    }
};

// Функция для начального формирования каталога файлов
void initializeCatalog(std::list<File>& catalog);

// Функция для вывода каталога файлов
void printCatalog(const std::list<File>& catalog);

// Функция для удаления файлов, дата создания которых раньше заданной
void deleteFilesBeforeDate(std::list<File>& catalog, const std::chrono::system_clock::time_point& date);

// Функция для выборки файла с наибольшим количеством обращений
File& getFileWithMaxAccessCount(std::list<File>& catalog);

int main() {
    setlocale(LC_ALL, "ru");
    std::list<File> catalog; // Каталог файлов
    initializeCatalog(catalog); // Начальное формирование каталога

    int choice;
    do {
        std::cout << "\nМеню:" << std::endl;
        std::cout << "1. Вывести каталог файлов" << std::endl;
        std::cout << "2. Удалить файлы, созданные раньше заданной даты" << std::endl;
        std::cout << "3. Выбрать файл с наибольшим количеством обращений" << std::endl;
        std::cout << "4. Выход" << std::endl;
        std::cout << "Выберите действие (1-4): ";
        std::cin >> choice;

        switch (choice) {
        case 1:
            printCatalog(catalog);
            break;
        case 2:
        {
            std::cout << "Введите дату в формате ГГГГ-ММ-ДД: ";
            int year, month, day;
            std::cin >> year >> month >> day;
            std::tm inputDate = { 0 };
            inputDate.tm_year = year - 1900;  // год от 1900
            inputDate.tm_mon = month - 1;     // месяц от 0 до 11
            inputDate.tm_mday = day;          // день месяца
            std::chrono::system_clock::time_point deleteDate = std::chrono::system_clock::from_time_t(std::mktime(&inputDate));
            deleteFilesBeforeDate(catalog, deleteDate);
        }
        break;
        case 3:
        {
            File& file = getFileWithMaxAccessCount(catalog);
            std::cout << "Файл с наибольшим количеством обращений: " << file << std::endl;
        }
        break;
        case 4:
            std::cout << "Выход из программы." << std::endl;
            break;
        default:
            std::cout << "Некорректный выбор. Повторите ввод." << std::endl;
            break;
        }

    } while (choice != 4);

    return 0;
}

void initializeCatalog(std::list<File>& catalog) {
    // Получаем текущее время
    auto now = std::chrono::system_clock::now();

    // Добавляем несколько файлов в каталог
    catalog.emplace_back("file1.txt", now - std::chrono::hours(24 * 1)); // файл создан 1 день назад
    catalog.emplace_back("file2.doc", now - std::chrono::hours(24 * 2)); // файл создан 2 дня назад
    catalog.emplace_back("file3.jpg", now - std::chrono::hours(24 * 3)); // файл создан 3 дня назад
    catalog.emplace_back("file4.exe", now - std::chrono::hours(24 * 4)); // файл создан 4 дня назад
}

void printCatalog(const std::list<File>& catalog) {
    std::cout << "\nКаталог файлов:" << std::endl;
    for (const auto& file : catalog) {
        std::cout << file << std::endl;
    }
}

void deleteFilesBeforeDate(std::list<File>& catalog, const std::chrono::system_clock::time_point& date) {
    auto it = catalog.begin();
    while (it != catalog.end()) {
        if (it->creationDate < date) {
            std::cout << "Удален файл: " << it->name << std::endl;
            it = catalog.erase(it); // Удаляем файл из каталога
        }
        else {
            ++it;
        }
    }
}

File& getFileWithMaxAccessCount(std::list<File>& catalog) {
    if (catalog.empty()) {
        throw std::runtime_error("Каталог файлов пуст.");
    }

    auto maxAccessFile = catalog.begin();
    for (auto it = catalog.begin(); it != catalog.end(); ++it) {
        if (it->accessCount > maxAccessFile->accessCount) {
            maxAccessFile = it;
        }
    }

    return *maxAccessFile;
}

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

class Observer {
public:
    virtual ~Observer() = default;
    virtual void onWarning(const std::string& message) {}
    virtual void onError(const std::string& message) {}
    virtual void onFatalError(const std::string& message) {}
};

class ObservedSource {
private:
    mutable std::vector<std::weak_ptr<Observer>> observers;

    void notifyAll(void (Observer::* method)(const std::string&), const std::string& message) const {
        auto it = observers.begin();
        while (it != observers.end()) {
            if (auto pinned = it->lock()) {
                ((*pinned).*method)(message);
                ++it;
            }
            else {
                it = observers.erase(it);
            }
        }
    }

public:
    void addObserver(std::weak_ptr<Observer> obs) {
        observers.push_back(obs);
    }

    void warning(const std::string& message) const {
        notifyAll(&Observer::onWarning, message);
    }

    void error(const std::string& message) const {
        notifyAll(&Observer::onError, message);
    }

    void fatalError(const std::string& message) const {
        notifyAll(&Observer::onFatalError, message);
    }
};

class WarningObserver : public Observer {
public:
    void onWarning(const std::string& message) override {
        std::cout << "[WARNING]: " << message << std::endl;
    }
};

class ErrorObserver : public Observer {
private:
    std::string file_path;
public:
    explicit ErrorObserver(std::string path) : file_path(std::move(path)) {}

    void onError(const std::string& message) override {
        std::ofstream file(file_path, std::ios::app);
        if (file.is_open()) {
            file << "[ERROR]: " << message << std::endl;
        }
    }
};

class FatalErrorObserver : public Observer {
private:
    std::string file_path;
public:
    explicit FatalErrorObserver(std::string path) : file_path(std::move(path)) {}

    void onFatalError(const std::string& message) override {
        std::cout << "[FATAL ERROR]: " << message << std::endl;
        std::ofstream file(file_path, std::ios::app);
        if (file.is_open()) {
            file << "[FATAL ERROR]: " << message << std::endl;
        }
    }
};

int main() {
    std::setlocale(LC_ALL, "Russian");

    ObservedSource source;
    std::string log_file = "errors_log.txt";

    auto warningObs = std::make_shared<WarningObserver>();
    auto errorObs = std::make_shared<ErrorObserver>(log_file);

    source.addObserver(warningObs);
    source.addObserver(errorObs);

    {
        auto fatalObs = std::make_shared<FatalErrorObserver>(log_file);
        source.addObserver(fatalObs);

        std::cout << "--- Все наблюдатели активны ---" << std::endl;
        source.warning("Низкий уровень заряда батареи.");
        source.error("Не удалось подключиться к базе данных.");
        source.fatalError("Критический сбой памяти! Система останавливается.");

    }

    std::cout << "\n--- Наблюдатель FatalError удален из памяти ---" << std::endl;
    source.fatalError("Этого сообщения не должно быть ни в консоли, ни в файле.");

    source.warning("Повторное предупреждение.");

    return 0;
}

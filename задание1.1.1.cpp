#include <iostream>
#include <fstream>
#include <string>
#include <memory>

class LogCommand {
public:
    virtual ~LogCommand() = default;
    virtual void print(const std::string& message) = 0;
};

class ConsoleLogCommand : public LogCommand {
public:
    void print(const std::string& message) override {
        std::cout << "[Console Log]: " << message << std::endl;
    }
};

class FileLogCommand : public LogCommand {
private:
    std::string file_path;

public:
    explicit FileLogCommand(std::string path) : file_path(std::move(path)) {}

    void print(const std::string& message) override {
        std::ofstream file(file_path, std::ios::app);
        if (file.is_open()) {
            file << "[File Log]: " << message << std::endl;
        }
        else {
            std::cerr << "Ошибка: Не удалось открыть файл по пути: " << file_path << std::endl;
        }
    }
};

void print(LogCommand& command, const std::string& message) {
    command.print(message);
}

int main() {
    std::setlocale(LC_ALL, "Russian");

    ConsoleLogCommand consoleLog;

    FileLogCommand fileLog("log.txt");

    std::string msg1 = "Привет! Это первое тестовое сообщение.";
    std::string msg2 = "А это сообщение должно записаться в файл.";

    std::cout << "--- Запуск логирования ---" << std::endl;

    print(consoleLog, msg1);
    print(fileLog, msg2);

    std::cout << "--- Логирование завершено ---" << std::endl;

    return 0;
}

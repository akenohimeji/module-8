#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <stdexcept>

enum class Type {
    Warning,
    Error,
    FatalError,
    Unknown
};

class LogMessage {
private:
    Type msg_type;
    std::string msg_text;

public:
    LogMessage(Type type, std::string message)
        : msg_type(type), msg_text(std::move(message)) {
    }

    Type type() const { return msg_type; }
    const std::string& message() const { return msg_text; }
};

class LogHandler {
protected:
    std::shared_ptr<LogHandler> next_handler;

public:
    virtual ~LogHandler() = default;

    void setNext(std::shared_ptr<LogHandler> next) {
        next_handler = next;
    }

    virtual void handle(const LogMessage& msg) {
        if (next_handler) {
            next_handler->handle(msg);
        }
    }
};

class FatalErrorHandler : public LogHandler {
public:
    void handle(const LogMessage& msg) override {
        if (msg.type() == Type::FatalError) {
            throw std::runtime_error("[CRITICAL] Фатальная ошибка: " + msg.message());
        }
        LogHandler::handle(msg);
    }
};

class ErrorHandler : public LogHandler {
private:
    std::string file_path;

public:
    explicit ErrorHandler(std::string path) : file_path(std::move(path)) {}

    void handle(const LogMessage& msg) override {
        if (msg.type() == Type::Error) {
            std::ofstream file(file_path, std::ios::app);
            if (file.is_open()) {
                file << "[FILE LOG] Ошибка: " << msg.message() << std::endl;
                std::cout << "[Chain]: Ошибка успешно записана в файл." << std::endl;
            }
            else {
                std::cerr << "Не удалось открыть файл для записи ошибки!" << std::endl;
            }
            return; 
        }
        LogHandler::handle(msg);
    }
};

class WarningHandler : public LogHandler {
public:
    void handle(const LogMessage& msg) override {
        if (msg.type() == Type::Warning) {
            std::cout << "[CONSOLE LOG] Предупреждение: " << msg.message() << std::endl;
            return; 
        }
        LogHandler::handle(msg);
    }
};

class UnknownMessageHandler : public LogHandler {
public:
    void handle(const LogMessage& msg) override {
        if (msg.type() == Type::Unknown) {
            throw std::runtime_error("[UNKNOWN] Необработанное сообщение: " + msg.message());
        }
        LogHandler::handle(msg);
    }
};

int main() {
    std::setlocale(LC_ALL, "Russian");

    auto fatalHandler = std::make_shared<FatalErrorHandler>();
    auto errorHandler = std::make_shared<ErrorHandler>("chain_errors.txt");
    auto warningHandler = std::make_shared<WarningHandler>();
    auto unknownHandler = std::make_shared<UnknownMessageHandler>();

    fatalHandler->setNext(errorHandler);
    errorHandler->setNext(warningHandler);
    warningHandler->setNext(unknownHandler);

    LogMessage msg1(Type::Warning, "Низкий заряд батареи.");
    LogMessage msg2(Type::Error, "Потеря соединения с сервером.");
    LogMessage msg3(Type::Unknown, "Какая-то странная сетевая активность.");
    LogMessage msg4(Type::FatalError, "Процессор перегрелся!");

    std::cout << "--- Накатываем сообщения на цепочку ---" << std::endl;

    fatalHandler->handle(msg1);

    fatalHandler->handle(msg2);

    try {
        fatalHandler->handle(msg3);
    }
    catch (const std::runtime_error& e) {
        std::cout << "Перехвачено исключение: " << e.what() << std::endl;
    }

    try {
        fatalHandler->handle(msg4);
    }
    catch (const std::runtime_error& e) {
        std::cout << "Перехвачено исключение: " << e.what() << std::endl;
    }

    return 0;
}

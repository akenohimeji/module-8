#include <fstream>
#include <string>
#include <stdexcept>
#include <memory>

// ============================================================================
// ИСПРАВЛЕННЫЕ ИНТЕРФЕЙСЫ (Решение ISP)
// ОШИБКА: Раньше интерфейс Printable требовал от одного класса уметь всё сразу.
// РЕШЕНИЕ: Разделили один "толстый" интерфейс на три независимых.
// ============================================================================

class HTMLPrintable {
public:
    virtual ~HTMLPrintable() = default;
    virtual std::string printAsHTML() const = 0;
};

class TextPrintable {
public:
    virtual ~TextPrintable() = default;
    virtual std::string printAsText() const = 0;
};

class JSONPrintable {
public:
    virtual ~JSONPrintable() = default;
    virtual std::string printAsJSON() const = 0;
};


// ============================================================================
// КЛАСС ДАННЫХ (Решение LSP)
// ОШИБКА: Раньше Data хранил формат внутри и падал с runtime_error при "чужом" методе.
// Это нарушало контракт базового класса (LSP).
// РЕШЕНИЕ: Класс Data теперь просто хранит чистые данные. Он не привязан к формату.
// ============================================================================

class Data {
public:
    explicit Data(std::string data) : data_(std::move(data)) {}

    const std::string& getData() const {
        return data_;
    }

private:
    std::string data_;
};


// ============================================================================
// КЛАССЫ-ФОРМАТТЕРЫ (Решение OCP)
// ОШИБКА: Раньше для добавления формата нужно было менять интерфейс Printable и enum.
// РЕШЕНИЕ: Применили паттерн "Стратегия". Новый формат добавляется созданием нового класса,
// не затрагивая старый код. Классы реализуют только нужные им интерфейсы.
// ============================================================================

class TextDataFormatter : public TextPrintable {
public:
    explicit TextDataFormatter(const Data& data) : data_(data) {}

    std::string printAsText() const override {
        // ОШИБКА ИСПРАВЛЕНА: Больше никаких проверок флага и исключений runtime_error!
        return data_.getData();
    }
private:
    const Data& data_;
};

class HTMLDataFormatter : public HTMLPrintable {
public:
    explicit HTMLDataFormatter(const Data& data) : data_(data) {}

    std::string printAsHTML() const override {
        return "<html>" + data_.getData() + "</html>"; // Исправлен также неверный тег <html/>
    }
private:
    const Data& data_;
};

class JSONDataFormatter : public JSONPrintable {
public:
    explicit JSONDataFormatter(const Data& data) : data_(data) {}

    std::string printAsJSON() const override {
        return "{ \"data\": \"" + data_.getData() + "\"}";
    }
private:
    const Data& data_;
};


// ============================================================================
// ФУНКЦИИ СОХРАНЕНИЯ (Решение OCP и LSP)
// ОШИБКА: Раньше функция saveTo содержала switch-case. При добавлении формата её
// пришлось бы переписывать (нарушение OCP).
// РЕШЕНИЕ: Функции теперь принимают строго целевой интерфейс. Полная безопасность типов.
// ============================================================================

void saveToAsHTML(std::ofstream& file, const HTMLPrintable& printable) {
    file << printable.printAsHTML();
}

void saveToAsJSON(std::ofstream& file, const JSONPrintable& printable) {
    file << printable.printAsJSON();
}

void saveToAsText(std::ofstream& file, const TextPrintable& printable) {
    file << printable.printAsText();
}

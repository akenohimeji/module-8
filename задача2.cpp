#include <fstream>
#include <string>
#include <utility>
#include <iostream>


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


class Data {
public:
    explicit Data(std::string data) : data_(std::move(data)) {}
    virtual ~Data() = default;

protected:
    const std::string& raw_data() const { return data_; }

private:
    std::string data_;
};

class TextData : public Data, public TextPrintable {
public:
    using Data::Data;

    std::string printAsText() const override {
        return raw_data();
    }
};

class HTMLData : public Data, public HTMLPrintable {
public:
    using Data::Data;

    std::string printAsHTML() const override {
        return "<html>" + raw_data() + "</html>";
    }
};

class JSONData : public Data, public JSONPrintable {
public:
    using Data::Data;

    std::string printAsJSON() const override {
        return "{ \"data\": \"" + raw_data() + "\"}";
    }
};


void saveToAsHTML(std::ofstream& file, const HTMLPrintable& printable) {
    file << printable.printAsHTML();
}

void saveToAsJSON(std::ofstream& file, const JSONPrintable& printable) {
    file << printable.printAsJSON();
}

void saveToAsText(std::ofstream& file, const TextPrintable& printable) {
    file << printable.printAsText();
}


int main() {
    setlocale(LC_ALL, "Russian");
    HTMLData htmlContent("Hello from HTML!");
    TextData textContent("Hello from Plain Text!");
    JSONData jsonContent("Hello from JSON!");

    std::ofstream htmlFile("output.html");
    std::ofstream textFile("output.txt");
    std::ofstream jsonFile("output.json");

    if (htmlFile.is_open() && textFile.is_open() && jsonFile.is_open()) {

        saveToAsHTML(htmlFile, htmlContent);
        saveToAsText(textFile, textContent);
        saveToAsJSON(jsonFile, jsonContent);

        std::cout << "Все файлы успешно созданы и записаны!" << std::endl;
    }
    else {
        std::cerr << "Ошибка: Не удалось открыть один или несколько файлов для записи." << std::endl;
    }

    return 0;
}

#include <iostream>
#include <string>
#include <algorithm>

class Printable {
public:
    virtual ~Printable() = default;
    virtual void render(const std::string& text) const = 0;
};

class Text : public Printable {
public:
    void render(const std::string& text) const override {
        std::cout << text;
    }
};

class Decorator : public Printable {
protected:
    Printable* component;
public:
    Decorator(Printable* comp) : component(comp) {}
    virtual ~Decorator() { delete component; }
};

class Paragraph : public Decorator {
public:
    Paragraph(Printable* comp) : Decorator(comp) {}

    void render(const std::string& text) const override {
        std::cout << "<p>";
        component->render(text);
        std::cout << "</p>";
    }
};

class Reversed : public Decorator {
public:
    Reversed(Printable* comp) : Decorator(comp) {}

    void render(const std::string& text) const override {
        std::string reversed_text = text;
        std::reverse(reversed_text.begin(), reversed_text.end());
        component->render(reversed_text);
    }
};

class Link : public Decorator {
public:
    Link(Printable* comp) : Decorator(comp) {}

    void render(const std::string& text) const override {
        std::cout << "<a href=\x22\x22>";
        component->render(text);
        std::cout << "</a>";
    }

    void render(const std::string& href, const std::string& text) const {
        std::cout << "<a href=" << href << ">";
        component->render(text);
        std::cout << "</a>";
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    auto text_block_p = new Paragraph(new Text());
    text_block_p->render("Hello world");
    std::cout << "\n\n";

    auto text_block_r = new Reversed(new Text());
    text_block_r->render("Hello world");
    std::cout << "\n\n";

    // Тест 3: Link
    auto text_block_l = new Link(new Text());
    text_block_l->render("netology.ru", "Hello world");
    std::cout << "\n";

    return 0;
}

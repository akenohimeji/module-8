#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <cassert>

class SqlSelectQueryBuilder {
public:
    SqlSelectQueryBuilder() = default;

    SqlSelectQueryBuilder& AddColumn(const std::string& column) {
        columns_.push_back(column);
        return *this;
    }

    SqlSelectQueryBuilder& AddFrom(const std::string& table) {
        table_ = table;
        return *this;
    }

    SqlSelectQueryBuilder& AddWhere(const std::string& column, const std::string& value) {
        where_conditions_.push_back({ column, value });
        return *this;
    }

    std::string BuildQuery() const {
        std::string query = "SELECT ";

        if (columns_.empty()) {
            query += "*";
        }
        else {
            for (size_t i = 0; i < columns_.size(); ++i) {
                query += columns_[i];
                if (i + 1 < columns_.size()) {
                    query += ", ";
                }
            }
        }

        query += " FROM " + table_;

        if (!where_conditions_.empty()) {
            query += " WHERE ";
            for (size_t i = 0; i < where_conditions_.size(); ++i) {
                query += where_conditions_[i].first + "=" + where_conditions_[i].second;
                if (i + 1 < where_conditions_.size()) {
                    query += " AND ";
                }
            }
        }

        query += ";";
        return query;
    }

private:
    std::vector<std::string> columns_;
    std::string table_;
    std::vector<std::pair<std::string, std::string>> where_conditions_;
};

int main() {
    setlocale(LC_ALL, "Russian");

    SqlSelectQueryBuilder query_builder;
    query_builder.AddColumn("name").AddColumn("phone");
    query_builder.AddFrom("students");
    query_builder.AddWhere("id", "42").AddWhere("name", "John");

    std::string expected_1 = "SELECT name, phone FROM students WHERE id=42 AND name=John;";
    assert(query_builder.BuildQuery() == expected_1);
    std::cout << "Тест 1 пройден: " << query_builder.BuildQuery() << std::endl;

    SqlSelectQueryBuilder empty_cols_builder;
    empty_cols_builder.AddFrom("users");
    empty_cols_builder.AddWhere("active", "1");

    std::string expected_2 = "SELECT * FROM users WHERE active=1;";
    assert(empty_cols_builder.BuildQuery() == expected_2);
    std::cout << "Тест 2 пройден: " << empty_cols_builder.BuildQuery() << std::endl;

    std::cout << "\nВсе тесты успешно выполнены!" << std::endl;
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <map>
#include <cassert>

class SqlSelectQueryBuilder {
public:
    SqlSelectQueryBuilder() = default;

    SqlSelectQueryBuilder& AddColumn(const std::string& column) {
        columns_.push_back(column);
        return *this;
    }

    SqlSelectQueryBuilder& AddColumns(const std::vector<std::string>& columns) noexcept {
        for (size_t i = 0; i < columns.size(); ++i) {
            columns_.push_back(columns[i]);
        }
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

    SqlSelectQueryBuilder& AddWhere(const std::map<std::string, std::string>& kv) noexcept {
        for (auto it = kv.begin(); it != kv.end(); ++it) {
            where_conditions_.push_back({ it->first, it->second });
        }
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

    // Передаем вектор колонок
    std::vector<std::string> cols;
    cols.push_back("name");
    cols.push_back("phone");
    cols.push_back("age");
    query_builder.AddColumns(cols);

    query_builder.AddFrom("students");

    std::map<std::string, std::string> internal_where;
    internal_where["id"] = "42";
    internal_where["name"] = "John";
    query_builder.AddWhere(internal_where);

    std::string expected_1 = "SELECT name, phone, age FROM students WHERE id=42 AND name=John;";
    assert(query_builder.BuildQuery() == expected_1);
    std::cout << "Тест 1 (Новые методы) пройден: " << query_builder.BuildQuery() << std::endl;

    SqlSelectQueryBuilder mixed_builder;
    mixed_builder.AddColumn("id");

    std::vector<std::string> mixed_cols;
    mixed_cols.push_back("email");
    mixed_cols.push_back("status");
    mixed_builder.AddColumns(mixed_cols);

    mixed_builder.AddFrom("users");
    mixed_builder.AddWhere("role", "admin");

    std::map<std::string, std::string> mixed_where;
    mixed_where["active"] = "1";
    mixed_builder.AddWhere(mixed_where);

    std::string expected_2 = "SELECT id, email, status FROM users WHERE role=admin AND active=1;";
    assert(mixed_builder.BuildQuery() == expected_2);
    std::cout << "Тест 2 (Смешанный вызов) пройден: " << mixed_builder.BuildQuery() << std::endl;

    return 0;
}

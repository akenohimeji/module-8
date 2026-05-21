#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <map>
#include <cassert>

struct WhereCondition {
    std::string column;
    std::string op; 
    std::string value;
};

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
        where_conditions_.push_back({ column, "=", value });
        return *this;
    }

    SqlSelectQueryBuilder& AddWhere(const std::map<std::string, std::string>& kv) noexcept {
        for (auto it = kv.begin(); it != kv.end(); ++it) {
            where_conditions_.push_back({ it->first, "=", it->second });
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
                if (i + 1 < columns_.size()) query += ", ";
            }
        }

        query += " FROM " + table_;

        if (!where_conditions_.empty()) {
            query += " WHERE ";
            for (size_t i = 0; i < where_conditions_.size(); ++i) {
                query += where_conditions_[i].column + where_conditions_[i].op + where_conditions_[i].value;
                if (i + 1 < where_conditions_.size()) query += " AND ";
            }
        }

        query += ";";
        return query;
    }

protected:
    std::vector<std::string> columns_;
    std::string table_;
    std::vector<WhereCondition> where_conditions_;
};


class AdvancedSqlSelectQueryBuilder : public SqlSelectQueryBuilder {
public:
    AdvancedSqlSelectQueryBuilder() = default;

    AdvancedSqlSelectQueryBuilder& AddWhereGreater(const std::string& column, const std::string& value) {
        where_conditions_.push_back({ column, ">", value });
        return *this;
    }

    AdvancedSqlSelectQueryBuilder& AddWhereLess(const std::string& column, const std::string& value) {
        where_conditions_.push_back({ column, "<", value });
        return *this;
    }
};


int main() {
    setlocale(LC_ALL, "Russian");

    AdvancedSqlSelectQueryBuilder query_builder;

    std::vector<std::string> cols;
    cols.push_back("name");
    cols.push_back("phone");
    query_builder.AddColumns(cols);

    query_builder.AddFrom("students");

    query_builder.AddWhereGreater("id", "42");

    std::string expected = "SELECT name, phone FROM students WHERE id>42;";
    assert(query_builder.BuildQuery() == expected);
    std::cout << "Тест пройден: " << query_builder.BuildQuery() << std::endl;

    AdvancedSqlSelectQueryBuilder complex_builder;
    complex_builder.AddColumn("name").AddFrom("workers");
    complex_builder.AddWhere("status", "active"); 
    complex_builder.AddWhereLess("age", "30");     

    std::string expected_complex = "SELECT name FROM workers WHERE status=active AND age<30;";
    assert(complex_builder.BuildQuery() == expected_complex);
    std::cout << "Комплексный тест пройден: " << complex_builder.BuildQuery() << std::endl;

    std::cout << "\nВсе тесты успешно выполнены!" << std::endl;
    return 0;
}

#include <iostream>
#include <string>
#include <memory>

class VeryHeavyDatabase {
public:
    virtual ~VeryHeavyDatabase() = default;

    virtual std::string GetData(const std::string& key) const {
        return "value"; 
    }
};

class OneShotDB : public VeryHeavyDatabase {
private:
    VeryHeavyDatabase* real_object; 
    mutable size_t shots;           

public:
    explicit OneShotDB(VeryHeavyDatabase* real_object, size_t shots = 1)
        : real_object(real_object), shots(shots) {
    }


    std::string GetData(const std::string& key) const override {
        if (shots > 0) {
            --shots; 
            return real_object->GetData(key); 
        }

        return "error"; 
    }
};

int main() {
    auto real_db = VeryHeavyDatabase();
    auto limit_db = OneShotDB(std::addressof(real_db), 2);

    std::cout << limit_db.GetData("key") << std::endl; 
    std::cout << limit_db.GetData("key") << std::endl; 
    std::cout << limit_db.GetData("key") << std::endl; 

    return 0;
}

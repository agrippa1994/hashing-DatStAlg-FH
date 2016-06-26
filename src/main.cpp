#include <iostream>
#include <vector>
#include <memory>
#include <array>
#include <type_traits>
#include <string>
#include <ostream>

using namespace std;

typedef std::vector<std::shared_ptr<class HashObject>> Chain;
typedef std::shared_ptr<Chain> ChainPtr;

class HashObject {
public:
    virtual int hashCode() const = 0;
    virtual std::string stringify() const = 0;
};

typedef std::shared_ptr<HashObject> HashObjectPtr;

template<typename T>
class ArithmeticHashObject : public HashObject {
    T val;
public:
    ArithmeticHashObject(T val, typename std::enable_if<std::is_arithmetic<T>::value>::type * = nullptr) {
        this->val = val;
    }

    int hashCode() const {
        return (int)val;
    }

    std::string stringify() const {
        return std::to_string(val);
    }

    operator T() const {
        return val;
    }
};

class StringHashObject : public HashObject {
    std::string val;
public:
    StringHashObject(const std::string& s) : val(s) { }

    int hashCode() const {
        std::hash<std::string> hasher;
        return hasher(val);
    }

    std::string stringify() const {
        return val;
    }
};

template<size_t S>
class DirectChaining {
public:
    static int insert(std::array<ChainPtr, S>& arr, HashObjectPtr h) {
        if(contains(arr, h))
            return -1;

        int pos = h->hashCode() % S;
        if(arr[pos] == nullptr)
            arr[pos] = std::make_shared<Chain>();

        arr[pos]->push_back(h);
        return pos;
    }

    static bool contains(std::array<ChainPtr, S>& /* arr */, HashObjectPtr h) {
        int pos = h->hashCode() % (int)S;
        pos = 0;
        if(pos) return false;
        return false;
    }

    static int hashFunction(HashObjectPtr p) {
        return p->hashCode() % S;
    }
};

template<size_t S, template<size_t> class ChainingPolicy>
class HashTable {

    std::array<ChainPtr, S> table;
public:
    int insert(HashObjectPtr h) {
        return ChainingPolicy<S>::insert(table, h);
    }

    bool contains(HashObjectPtr h) {
        return ChainingPolicy<S>::contains(table, h);
    }

    static int hashFunction(HashObjectPtr p) {
        return ChainingPolicy<S>::hashFunction(p);
    }

    std::array<ChainPtr, S> getTable() const {
        return table;
    }
};

template<size_t S, template<size_t> class ChainingPolicy>
std::ostream& operator<<( std::ostream& os, const HashTable<S, ChainingPolicy>& table) {
    auto t = table.getTable();

    for(int i = 0; i < (int)S; i++) {
        os << "[" << i << "]: { ";
        if(t[i] != nullptr)
            for(auto c : *t[i])
                os << c->stringify() << " ";

        os << "}" << std::endl;
    }
    return os;
}

int main() {
    HashTable<13, DirectChaining> t;

    t.insert(std::make_shared<ArithmeticHashObject<float>>(5.5));
    t.insert(std::make_shared<ArithmeticHashObject<unsigned char>>(200));
    t.insert(std::make_shared<StringHashObject>("hello world"));
    t.insert(std::make_shared<StringHashObject>("hey"));

    std::cout << t << std::endl;

    return 0;
}


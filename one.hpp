#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cmath>
#include <cstring>

namespace me {
class Json {
private:
    enum NodeType {
        N_OBJECT,
        N_LIST,
        N_STRING,
        N_NUMBER,
        N_BOOL,
    };

public:
    enum Type {
        BOOL=N_BOOL,
        NUMBER=N_NUMBER,
        STRING=N_STRING,
        LIST=N_LIST,
        OBJECT=N_OBJECT,
    };

    class iterator {
    private:
        class iteratorGetClass;

    public:
        iterator(unsigned long id, unsigned long index);

        bool operator==(iterator& other);

        bool operator!=(iterator& other);

        void operator++();

        void operator--();

        iteratorGetClass operator*();

        template<typename type>
        type get() {
            if (_index >= size) {
                std::cerr << "cannot get value from iterator\n";
                exit(1);
            }
            Node& node = s_data[id];
            switch (node.type) {
                case N_STRING:
                    return node.value.str[_index];
                break;
                case N_LIST: {
                    getClass tmp(node.value.list[_index]);
                    return tmp;
                }
                break;
                case N_OBJECT: {
                    getClass tmp(node.value.object[_key[_index]]);
                    return tmp;
                }
                break;
            }
            std::cerr << "Unreachable: me::Json::iterator::get\n";
            exit(1);
        }

        unsigned long index();

        std::string key();

    private:
        class iteratorGetClass {
        public:
            iteratorGetClass(unsigned long id, std::vector<std::string>* key, unsigned long size, unsigned long index);

            operator char();

            operator Json();

            template<typename type>
            operator type() {
                Node& node = s_data[id];
                if (node.type != N_LIST) {
                    std::cerr << "cannot get value from non list iterator\n";
                    exit(1);
                }
                if (index >= size) {
                    std::cerr << "cannot get value from end iterator\n";
                    exit(1);
                }
                getClass tmp(node.value.list[index]);
                return tmp;
            }

            operator std::pair<std::string, Json>();

            template<typename type>
            operator std::pair<std::string, type>() {
                Node& node = s_data[id];
                if (node.type != N_OBJECT) {
                    std::cerr << "cannot get pair from non object iterator\n";
                    exit(1);
                }
                if (index >= size) {
                    std::cerr << "cannot get pair from end iterator\n";
                    exit(1);
                }
                getClass tmp(node.value.object[(*key)[index]]);
                return {(*key)[index], tmp};
            }

        private:
            unsigned long id;
            std::vector<std::string>* key;
            unsigned int size;
            unsigned int index;
        };

        unsigned long id;
        std::vector<std::string> _key;
        unsigned long size;
        unsigned long _index;
    };

    class JsonValue {
    public:
        JsonValue();

        JsonValue(unsigned long id);

        void set(unsigned long id);

        std::string str();

        template<typename type>
        type get() {
            getClass tmp(id);
            return tmp;
        }

        Type type();

        unsigned long size();

        std::vector<std::string> keys();

        iterator begin();

        iterator end();

        JsonValue operator[](unsigned long index);

        JsonValue operator[](std::string key);

        bool operator==(Json other);

        void operator=(Json json);

        template<typename type>
        void operator=(type value) {
            Node& node = s_data[id];
            node.destroy();
            createClass tmp(value);
            node.save = true;
            node.type = s_data[tmp.id].type;
            node.value = s_data[tmp.id].value;
            s_data.erase(tmp.id);
        }

        void operator+=(double v);

        void operator-=(double v);

        void operator*=(double v);

        void operator/=(double v);

        void push_back(char c);

        void push_back(std::string str);

        void push_back(Json json);

        template<typename type>
        void push_back(type v) {
            Node& node = s_data[id];
            if (node.type != N_LIST) {
                std::cerr << "cannot append item to non list\n";
                exit(1);
            }
            createClass tmp(v);
            node.value.list.push_back(tmp.id);
        }

        void insert(unsigned long index, char c);

        void insert(unsigned long index, std::string str);

        void insert(unsigned long index, Json json);

        template<typename type>
        void insert(unsigned long index, type value) {
            Node& node = s_data[id];
            if (node.type != N_LIST) {
                std::cerr << "cannot insert value\n";
                exit(1);
            }
            createClass tmp(value);
            std::vector<unsigned long>::iterator it(&node.value.list[index]);
            node.value.list.insert(it, tmp.id);
        }

        bool insert(std::string key, Json json);

        template<typename type>
        bool insert(std::string key, type value) {
            Node& node = s_data[id];
            if (node.type != N_OBJECT) {
                std::cerr << "cannot insert value into object\n";
                exit(1);
            }
            createClass tmp(value);
            return node.value.object.insert({key, tmp.id}).second;
        }

        void erase(unsigned long index);

        void erase(unsigned long first, unsigned long last);

        void erase(std::string key);

        void clear();

        bool empty();

        unsigned long find(char c);

        template<typename type>
        iterator find(type value) {
            Node& node = s_data[id];
            createClass tmp(value);
            switch (node.type) {
                case N_LIST:
                    for (unsigned long i = 0; i < node.value.list.size(); i++) {
                        if (cmp(tmp.id, node.value.list[i])) {
                            return {id, i};
                        }
                    }
                    return {id, node.value.list.size()};
                break;
                case N_OBJECT:
                    for (unsigned long i = 0; i < keys().size(); i++) {
                        if (cmp(tmp.id, node.value.object[keys()[i]])) {
                            return {id, i};
                        }
                    }
                    return {id, keys().size()};
                break;
            }
            std::cerr << "cannot find value in string\n";
            exit(1);
        }

        operator Json();

    private:
        unsigned long id;
    };

    Json(std::string str);

    Json(Json& other);

    template<typename type>
    static Json create(type value) {
        createClass tmp(value);
        return tmp.id;
    }

    std::string str();

    template<typename type>
    type get() {
        return value.get<type>();
    }

    Type type();

    unsigned long size();

    std::vector<std::string> keys();

    iterator begin();

    iterator end();

    JsonValue operator[](unsigned long index);

    JsonValue operator[](std::string key);

    bool operator==(Json other);

    void operator=(Json json);

    void operator+=(double v);

    void operator-=(double v);

    void operator*=(double v);

    void operator/=(double v);

    void push_back(char c);

    void push_back(std::string str);

    void push_back(Json json);

    template<typename type>
    void push_back(type v) {
        value.push_back(v);
    }

    void insert(unsigned long index, char c);

    void insert(unsigned long index, std::string str);

    void insert(unsigned long index, Json json);

    template<typename type>
    void insert(unsigned long index, type value) {
        value.insert(index, value);
    }

    bool insert(std::string key, Json json);

    template<typename type>
    bool insert(std::string key, type value) {
        return value.insert(key, value);
    }

    void erase(unsigned long index);

    void erase(unsigned long first, unsigned long last);

    void erase(std::string key);

    void clear();

    bool empty();

    unsigned long find(char c);

    template<typename type>
    iterator find(type value) {
        return value.find(value);
    }

    ~Json();

private:
    enum TokenType {
        T_COPEN,
        T_CCLOSE,
        T_BOPEN,
        T_BCLOSE,
        T_STRING,
        T_NUMBER,
        T_COMMA,
        T_BOOL,
        T_COLON,
    };

    struct TokenValue {
        TokenValue();
        TokenValue(bool b);
        TokenValue(double number);
        TokenValue(std::string str);

        bool b;
        double number;
        std::string str;
    };

    struct Token {
        TokenType type;
        TokenValue value;
    };

    struct NodeValue {
        NodeValue();
        NodeValue(std::map<std::string, unsigned long> object);
        NodeValue(std::vector<unsigned long> list);
        NodeValue(std::string str);
        NodeValue(double number);
        NodeValue(bool b);

        std::map<std::string, unsigned long> object;
        std::vector<unsigned long> list;
        std::string str;
        double number;
        bool b;
    };

    struct Node {
        NodeType type;
        NodeValue value;
        bool save = false;

        void destroy();
    };

    class getClass {
    public:
        getClass(unsigned long node);

        operator bool();

        operator double();

        operator long();

        operator int();

        operator short();

        operator std::string();

        template<typename type>
        operator std::vector<type>() {
            Node& node = s_data[id];
            if (node.type != N_LIST) {
                std::cerr << "Cannot get vector from non list type\n";
                exit(1);
            }
            std::vector<type> ret;
            for (unsigned long n : node.value.list) {
                getClass tmp(n);
                ret.push_back(tmp);
            }
            return ret;
        }

        template<typename type>
        operator std::map<std::string, type>() {
            Node& node = s_data[id];
            if (node.type != N_OBJECT) {
                std::cerr << "Cannot get map from non object type\n";
                exit(1);
            }
            std::map<std::string, type> ret;
            for (std::pair<std::string, unsigned long> n : node.value.object) {
                getClass tmp(n.second);
                ret.insert({n.first, tmp});
            }
            return ret;
        }

    private:
        unsigned long id;
    };

    class createClass {
    public:
        unsigned long id;

        createClass(bool b);

        createClass(long number);

        createClass(int number);

        createClass(short number);

        createClass(double number);

        createClass(std::string str);

        template<typename type>
        createClass(std::vector<type> vector) {
            std::vector<unsigned long> list;
            for (type v : vector) {
                createClass tmp(v);
                list.push_back(tmp.id);
            }
            id = s_id;
            s_data.insert({s_id++, {N_LIST, {list}}});
        }

        template<typename type>
        createClass(std::map<std::string, type> map) {
            std::map<std::string, unsigned long> object;
            for (std::pair<std::string, type> v : map) {
                createClass tmp(v.second);
                object.insert({v.first, tmp.id});
            }
            id = s_id;
            s_data.insert({s_id++, {N_OBJECT, {object}}});
        }
    };

    static std::map<unsigned long, Node> s_data;
    static unsigned long s_id;
    static char hexChars[23];
    JsonValue value;
    unsigned long id;
    std::vector<Token> tokens;
    unsigned long i = 0;

    char get_char(std::string str, int base);

    double get_number(std::string str, int base);

    void tokenize(std::string str);

    TokenValue match(TokenType type, std::string str);

    unsigned long parse();

    static std::string hex(unsigned char c);

    static std::string prep(std::string str);

    static std::string str(unsigned long id, std::string indent="");

    static unsigned long copy(unsigned long id);

    static std::vector<std::string> keys(unsigned long id);

    static bool cmp(unsigned long id1, unsigned long id2);

    Json(unsigned long id);
};
}

unsigned long me::Json::s_id = 0;
char me::Json::hexChars[23] = "0123456789abcdefABCDEF";
std::map<unsigned long, me::Json::Node> me::Json::s_data;

me::Json::iterator::iterator(unsigned long id, unsigned long index) : id(id), _index(index) {
    Node& node = s_data[id];
    bool b = false;
    if (node.type == N_OBJECT) {
        b = true;
        for (std::pair<std::string, unsigned long> n : node.value.object) {
            _key.push_back(n.first);
            size = _key.size();
        }
    } else if (node.type == N_LIST) {
        b = true;
        size = node.value.list.size();
    } else if (node.type == N_STRING) {
        b = true;
        size = node.value.str.size();
    }
    if (!b) {
        std::cerr << "cannot create iterator, invalid type\n";
        exit(1);
    }
    if (index > size) {
        std::cerr << "invalid index " << index << " for Json::iterator with size " << size << "\n";
        exit(1);
    }
}

bool me::Json::iterator::operator==(iterator& other) {
    return (id==other.id && _index==other._index);
}

bool me::Json::iterator::operator!=(iterator& other) {
    return !(operator==(other));
}

void me::Json::iterator::operator++() {
    if (_index >= size) {
        std::cerr << "cannot increment end iterator\n";
        exit(1);
    }
    _index++;
}

void me::Json::iterator::operator--() {
    if (_index == 0) {
        std::cerr << "cannot decrement begin iterator\n";
        exit(1);
    }
    _index--;
}

me::Json::iterator::iteratorGetClass me::Json::iterator::operator*() {
    return {id, &_key, size, _index};
}

unsigned long me::Json::iterator::index() {
    if (_index >= size) {
        std::cerr << "cannot get index from iterator\n";
        exit(1);
    }
    Node& node = s_data[id];
    if (node.type != N_STRING && node.type != N_LIST) {
        std::cerr << "cannot get index from iterator\n";
        exit(1);
    }
    return _index;
}

std::string me::Json::iterator::key() {
    if (_index >= size) {
        std::cerr << "cannot get key from iterator\n";
        exit(1);
    }
    if (s_data[id].type != N_OBJECT) {
        std::cerr << "cannot get key from iterator\n";
        exit(1);
    }
    return _key[_index];
}

me::Json::iterator::iteratorGetClass::iteratorGetClass(unsigned long id, std::vector<std::string>* key, unsigned long size, unsigned long index) : id(id), key(key), size(size), index(index) {}

me::Json::iterator::iteratorGetClass::operator char() {
    Node& node = s_data[id];
    if (node.type != N_STRING) {
        std::cerr << "cannot get char from non string iterator\n";
        exit(1);
    }
    if (index >= size) {
        std::cerr << "cannot get char from end iterator\n";
        exit(1);
    }
    return node.value.str[index];
}

me::Json::iterator::iteratorGetClass::operator Json() {
    Node& node = s_data[id];
    if (node.type != N_LIST) {
        std::cerr << "cannot get Json from non list iterator\n";
        exit(1);
    }
    if (index >= size) {
        std::cerr << "cannot get Json from end iterator\n";
        exit(1);
    }
    return node.value.list[index];
}

me::Json::iterator::iteratorGetClass::operator std::pair<std::string, Json>() {
    Node& node = s_data[id];
    if (node.type != N_OBJECT) {
        std::cerr << "cannot get pair from non object iterator\n";
        exit(1);
    }
    if (index >= size) {
        std::cerr << "cannot get pair from end iterator\n";
        exit(1);
    }
    Json tmp(node.value.object[(*key)[index]]);
    return {(*key)[index], tmp};
}

me::Json::JsonValue::JsonValue() {}

me::Json::JsonValue::JsonValue(unsigned long id) : id(id) {}

void me::Json::JsonValue::set(unsigned long id) {
    this->id = id;
}

std::string me::Json::JsonValue::str() {
    return Json::str(id);
}

me::Json::Type me::Json::JsonValue::type() {
    return (Type)s_data[id].type;
}

unsigned long me::Json::JsonValue::size() {
    Node& node = s_data[id];
    switch (node.type) {
        case N_STRING:
            return node.value.str.size();
        break;
        case N_LIST:
            return node.value.list.size();
        break;
        case N_OBJECT:
            return node.value.object.size();
        break;
        default:
            std::cerr << "size is not available for bool and number\n";
            exit(1);
    }
}

std::vector<std::string> me::Json::JsonValue::keys() {
    return Json::keys(id);
}

me::Json::iterator me::Json::JsonValue::begin() {
    return {id, 0};
}

me::Json::iterator me::Json::JsonValue::end() {
    return {id, size()};
}

me::Json::JsonValue me::Json::JsonValue::operator[](unsigned long index) {
    Node& node = s_data[id];
    if (node.type != N_LIST) {
        std::cerr << "operator[] with int is only available for lists\n";
        exit(1);
    }
    return {node.value.list[index]};
}

me::Json::JsonValue me::Json::JsonValue::operator[](std::string key) {
    Node& node = s_data[id];
    if (node.type != N_OBJECT) {
        std::cerr << "operator[] with std::string is only available for objects\n";
        exit(1);
    }
    return {node.value.object[key]};
}

bool me::Json::JsonValue::operator==(Json other) {
    return cmp(id, other.id);
}

void me::Json::JsonValue::operator=(Json json) {
    Node& node = s_data[id];
    node.destroy();
    unsigned long tmp = copy(json.id);
    node.save = true;
    node.type = s_data[tmp].type;
    node.value = s_data[tmp].value;
    s_data.erase(tmp);
}

void me::Json::JsonValue::operator+=(double v) {
    Node& node = s_data[id];
    if (node.type != N_NUMBER) {
        std::cerr << "cannot add to non number\n";
        exit(1);
    }
    node.value.number += v;
}

void me::Json::JsonValue::operator-=(double v) {
    Node& node = s_data[id];
    if (node.type != N_NUMBER) {
        std::cerr << "cannot subtract from non number\n";
        exit(1);
    }
    node.value.number -= v;
}

void me::Json::JsonValue::operator*=(double v) {
    Node& node = s_data[id];
    if (node.type != N_NUMBER) {
        std::cerr << "cannot multiply non number\n";
        exit(1);
    }
    node.value.number *= v;
}

void me::Json::JsonValue::operator/=(double v) {
    Node& node = s_data[id];
    if (node.type != N_NUMBER) {
        std::cerr << "cannot divide non number\n";
        exit(1);
    }
    node.value.number /= v;
}

void me::Json::JsonValue::push_back(char c) {
    Node& node = s_data[id];
    if (node.type != N_STRING) {
        std::cerr << "cannot append char to non string\n";
        exit(1);
    }
    node.value.str.push_back(c);
}

void me::Json::JsonValue::push_back(std::string str) {
    Node& node = s_data[id];
    if (node.type != N_STRING) {
        std::cerr << "cannot append string to non string\n";
        exit(1);
    }
    for (char c : str) {
        node.value.str.push_back(c);
    }
}

void me::Json::JsonValue::push_back(Json json) {
    Node& node = s_data[id];
    if (node.type != N_LIST) {
        std::cerr << "cannot append item to non list\n";
        exit(1);
    }
    node.value.list.push_back(copy(json.id));
}

void me::Json::JsonValue::insert(unsigned long index, char c) {
    Node& node = s_data[id];
    if (node.type != N_STRING) {
        std::cerr << "cannot insert char into non string\n";
        exit(1);
    }
    std::string tmp;
    tmp.push_back(c);
    node.value.str.insert(index, tmp);
}

void me::Json::JsonValue::insert(unsigned long index, std::string str) {
    Node& node = s_data[id];
    switch (node.type) {
        case N_STRING:
            node.value.str.insert(index, str);
        break;
        case N_LIST: {
            createClass tmp(str);
            std::vector<unsigned long>::iterator it(&node.value.list[index]);
            node.value.list.insert(it, tmp.id);
        }
        break;
        default:
            std::cerr << "cannot insert string\n";
            exit(1);
    }
}

void me::Json::JsonValue::insert(unsigned long index, Json json) {
    Node& node = s_data[id];
    switch (node.type) {
        case N_LIST: {
            std::vector<unsigned long>::iterator tmp(&node.value.list[index]);
            node.value.list.insert(tmp, copy(json.id));
        }
        break;
        default:
            std::cerr << "cannot inset json\n";
            exit(1);
    }
}

bool me::Json::JsonValue::insert(std::string key, Json json) {
    Node& node = s_data[id];
    if (node.type != N_OBJECT) {
        std::cerr << "cannot insert Json into object\n";
        exit(1);
    }
    return node.value.object.insert({key, copy(json.id)}).second;
}

void me::Json::JsonValue::erase(unsigned long index) {
    Node& node = s_data[id];
    switch (node.type) {
        case N_STRING:
            if (index >= node.value.str.size()) {
                std::cerr << "cannot erase char, index out of bounds\n";
                exit(1);
            }
            node.value.str.erase(index, 1);
        break;
        case N_LIST: {
            if (index >= node.value.list.size()) {
                std::cerr << "cannot erase value, index out of bounds\n";
                exit(1);
            }
            s_data[node.value.list[index]].destroy();
            s_data.erase(node.value.list[index]);
            std::vector<unsigned long>::iterator tmp(&node.value.list[index]);
            node.value.list.erase(tmp);
        }
        break;
        default:
            std::cerr << "cannot erase value, invalid type\n";
            exit(1);
    }
}

void me::Json::JsonValue::erase(unsigned long first, unsigned long last) {
    Node& node = s_data[id];
    switch (node.type) {
        case N_STRING:
            if (first >= node.value.str.size() || last > node.value.str.size() || first >= last) {
                std::cerr << "cannot erase chars, indecies invalid\n";
                exit(1);
            }
            node.value.str.erase(first, last-first);
        break;
        case N_LIST: {
            if (first >= node.value.list.size() || last > node.value.list.size() || first >= last) {
                std::cerr << "cannot erase value, indecies invalid\n";
                exit(1);
            }
            s_data[node.value.list[first]].destroy();
            s_data.erase(node.value.list[first]);
            std::vector<unsigned long>::iterator tmp(&node.value.list[first]);
            node.value.list.erase(tmp);
        }
        break;
        default:
            std::cerr << "cannot erase value, invalid type\n";
            exit(1);
    }
}

void me::Json::JsonValue::erase(std::string key) {
    Node& node = s_data[id];
    if (node.type != N_OBJECT) {
        std::cerr << "cannot erase value, from non object\n";
        exit(1);
    }
    if (node.value.object.find(key) == node.value.object.end()) {
        std::cerr << "cannot erase value, invalid key\n";
        exit(1);
    }
    s_data[node.value.object[key]].destroy();
    s_data.erase(node.value.object[key]);
    node.value.object.erase(key);
}

void me::Json::JsonValue::clear() {
    Node& node = s_data[id];
    switch (node.type) {
        case N_STRING:
            node.value.str.clear();
        break;
        case N_LIST:
            for (unsigned long n : node.value.list) {
                s_data[n].destroy();
                s_data.erase(n);
            }
            node.value.list.clear();
        break;
        case N_OBJECT:
            for (std::pair<std::string, unsigned long> n : node.value.object) {
                s_data[n.second].destroy();
                s_data.erase(n.second);
            }
            node.value.object.clear();
        break;
        default:
            std::cerr << "cannot clear value, invalid type\n";
            exit(1);
    }
}

bool me::Json::JsonValue::empty() {
    return size()==0;
}

unsigned long me::Json::JsonValue::find(char c) {
    Node& node = s_data[id];
    if (node.type != N_STRING) {
        std::cerr << "cannot find char in non string\n";
        exit(1);
    }
    return node.value.str.find(c);
}

me::Json::JsonValue::operator Json() {
    return id;
}

me::Json::Json(std::string str) {
    tokenize(str);
    id = parse();
    value.set(id);
    s_data[id].save = true;
    tokens.clear();
}

me::Json::Json(Json& other) {
    id = copy(other.id);
    s_data[id].save = true;
    value.set(id);
}

std::string me::Json::str() {
    return value.str();
}

me::Json::Type me::Json::type() {
    return value.type();
}

unsigned long me::Json::size() {
    return value.size();
}

std::vector<std::string> me::Json::keys() {
    return value.keys();
}

me::Json::iterator me::Json::begin() {
    return value.begin();
}

me::Json::iterator me::Json::end() {
    return value.end();
}

unsigned long find(char c);

me::Json::~Json() {
    s_data[id].save = false;
    s_data[id].destroy();
    s_data.erase(id);
}

me::Json::JsonValue me::Json::operator[](unsigned long index) {
    return value[index];
}

me::Json::JsonValue me::Json::operator[](std::string key) {
    return value[key];
}

bool me::Json::operator==(Json other) {
    return value.operator==(other);
}

void me::Json::operator=(Json json) {
    value.operator=(json);
}

void me::Json::operator+=(double v) {
    value.operator+=(v);
}

void me::Json::operator-=(double v) {
    value.operator-=(v);
}

void me::Json::operator*=(double v) {
    value.operator*=(v);
}

void me::Json::operator/=(double v) {
    value.operator/=(v);
}

void me::Json::push_back(char c) {
    value.push_back(c);
}

void me::Json::push_back(std::string str) {
    value.push_back(str);
}

void me::Json::push_back(Json json) {
    value.push_back(json);
}

void me::Json::insert(unsigned long index, char c) {
    value.insert(index, c);
}

void me::Json::insert(unsigned long index, std::string str) {
    value.insert(index, str);
}

void me::Json::insert(unsigned long index, Json json) {
    value.insert(index, json);
}

bool me::Json::insert(std::string key, Json json) {
    return value.insert(key, json);
}

void me::Json::erase(unsigned long index) {
    value.erase(index);
}

void me::Json::erase(unsigned long first, unsigned long last) {
    value.erase(first, last);
}

void me::Json::erase(std::string key) {
    value.erase(key);
}

void me::Json::clear() {
    value.clear();
}

bool me::Json::empty() {
    return value.empty();
}

unsigned long me::Json::find(char c) {
    return value.find(c);
}





me::Json::TokenValue::TokenValue() {}

me::Json::TokenValue::TokenValue(bool b) : b(b) {}

me::Json::TokenValue::TokenValue(double number) : number(number) {}

me::Json::TokenValue::TokenValue(std::string str) : str(str) {}

me::Json::NodeValue::NodeValue() {}

me::Json::NodeValue::NodeValue(std::map<std::string, unsigned long> object) : object(object) {}

me::Json::NodeValue::NodeValue(std::vector<unsigned long> list) : list(list) {}

me::Json::NodeValue::NodeValue(std::string str) : str(str) {}

me::Json::NodeValue::NodeValue(double number) : number(number) {}

me::Json::NodeValue::NodeValue(bool b) : b(b) {}

void me::Json::Node::destroy() {
    if (save) {
        return;
    }
    if (type == N_LIST) {
        for (unsigned long n : value.list) {
            s_data[n].destroy();
            s_data.erase(n);
        }
        value.list.clear();
    }
    if (type == N_OBJECT) {
        for (std::pair<std::string, unsigned long> n : value.object) {
            s_data[n.second].destroy();
            s_data.erase(n.second);
        }
        value.object.clear();
    }
}

me::Json::getClass::getClass(unsigned long id) : id(id) {}

me::Json::getClass::operator bool() {
    Node& node = s_data[id];
    if (node.type != N_BOOL) {
        std::cerr << "Cannot get bool from non bool type\n";
        exit(1);
    }
    return node.value.b;
}

me::Json::getClass::operator double() {
    Node& node = s_data[id];
    if (node.type != N_NUMBER) {
        std::cerr << "Cannot get double from non number type\n";
        exit(1);
    }
    return node.value.number;
}

me::Json::getClass::operator long() {
    Node& node = s_data[id];
    if (node.type != N_NUMBER) {
        std::cerr << "Cannot get integer from non number type\n";
        exit(1);
    }
    return node.value.number;
}

me::Json::getClass::operator int() {
    return operator long();
}

me::Json::getClass::operator short() {
    return operator long();
}

me::Json::getClass::operator std::string() {
    Node& node = s_data[id];
    if (node.type != N_STRING) {
        std::cerr << "Cannot get string from non string type\n";
        exit(1);
    }
    return node.value.str;
}

me::Json::createClass::createClass(bool b) {
    id = s_id;
    s_data.insert({s_id++, {N_BOOL, {b}}});
}

me::Json::createClass::createClass(long number) {
    id = s_id;
    s_data.insert({s_id++, {N_NUMBER, {(double)number}}});
}

me::Json::createClass::createClass(int number) {
    id = s_id;
    s_data.insert({s_id++, {N_NUMBER, {(double)number}}});
}

me::Json::createClass::createClass(short number) {
    id = s_id;
    s_data.insert({s_id++, {N_NUMBER, {(double)number}}});
}

me::Json::createClass::createClass(double number) {
    id = s_id;
    s_data.insert({s_id++, {N_NUMBER, {number}}});
}

me::Json::createClass::createClass(std::string str) {
    id = s_id;
    s_data.insert({s_id++, {N_STRING, {str}}});
}

char me::Json::get_char(std::string str, int base) {
    char ret = 0;
    for (char c : str) {
        ret *= base;
        if (isdigit(c)) {
            if (c-'0' >= base) {
                std::cerr << "Invalid escape sequence: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'0';
        } else if (isupper(c)) {
            if (c-'A'+10 >= base) {
                std::cerr << "Invalid escape sequence: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'A'+10;
        } else if (islower(c)) {
            if (c-'a'+10 >= base) {
                std::cerr << "Invalid escape sequence: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'a'+10;
        } else {
            std::cerr << "Invalid escape sequence: not a number '" << c << "'\n";
            exit(1);
        }
    }
    return ret;
}

double me::Json::get_number(std::string str, int base) {
    double ret = 0;
    unsigned long i = 0;
    for (; i < str.size() && str[i] != '.'; i++) {
        char c = str[i];
        ret *= base;
        if (isdigit(c)) {
            if (c-'0' >= base) {
                std::cerr << "Invalid number: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'0';
        } else if (isupper(c)) {
            if (c-'A'+10 >= base) {
                std::cerr << "Invalid number: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'A'+10;
        } else if (islower(c)) {
            if (c-'a'+10 >= base) {
                std::cerr << "Invalid number: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'a'+10;
        } else {
            std::cerr << "Invalid number: not a number '" << c << "'\n";
            exit(1);
        }
    }
    unsigned long j = 1;
    if (i < str.size()) {
        if (base != 10) {
            std::cerr << "Wrong base for decimal number " << base << "\n";
            exit(1);
        }
        i++;
        for (; i < str.size(); i++) {
            if (isdigit(str[i])) {
                if (str[i]-'0' >= 10) {
                    std::cerr << "Invalid decimal digit: number exceeds 9 " << str[i] << " \n";
                    exit(1);
                }
                ret += ((double)(str[i]-'0'))/pow(10, j++);
            } else {
                std::cerr << "Invalid decimal digit " << str[i] << "\n";
                exit(1);
            }
        }
    }
    return ret;
}

void me::Json::tokenize(std::string str) {
    std::string buf;
    unsigned long i = 0;
    while (i < str.size()) {
        switch (str[i]) {
            case '{':
                i++;
                tokens.push_back({T_COPEN});
            break;
            case '}':
                i++;
                tokens.push_back({T_CCLOSE});
            break;
            case '[':
                i++;
                tokens.push_back({T_BOPEN});
            break;
            case ']':
                i++;
                tokens.push_back({T_BCLOSE});
            break;
            case ',':
                i++;
                tokens.push_back({T_COMMA});
            break;
            case ':':
                i++;
                tokens.push_back({T_COLON});
            break;
            case '"':
                i++;
                while (str[i] != '"') {
                    if (str[i] == '\\') {
                        i++;
                        switch (str[i]) {
                            case 'a':
                                i++;
                                buf.push_back('\a');
                            break;
                            case 'b':
                                i++;
                                buf.push_back('\b');
                            break;
                            case 'e':
                                i++;
                                buf.push_back('\e');
                            break;
                            case 'f':
                                i++;
                                buf.push_back('\f');
                            break;
                            case 'n':
                                i++;
                                buf.push_back('\n');
                            break;
                            case 'r':
                                i++;
                                buf.push_back('\r');
                            break;
                            case 't':
                                i++;
                                buf.push_back('\t');
                            break;
                            case 'v':
                                i++;
                                buf.push_back('\v');
                            break;
                            case 'x':
                                i++;
                                buf.push_back(get_char(str.substr(i, 2), 16));
                                i += 2;
                            break;
                            case '"':
                                i++;
                                buf.push_back('"');
                            break;
                            default:
                                if (isdigit(str[i]) && str[i] <= '8') {
                                    std::string b;
                                    b.push_back(str[i++]);
                                    while (isdigit(str[i]) && str[i] <= '8' && b.size() < 3) {
                                        b.push_back(str[i++]);
                                    }
                                    buf.push_back(get_char(b, 8));
                                    break;
                                }
                                std::cerr << "Invalid escape sequence '" << str[i] << "'\n";
                                exit(1);
                        }
                    } else {
                        buf.push_back(str[i]);
                        i++;
                    }
                }
                i++;
                tokens.push_back({T_STRING, {buf}});
                buf.clear();
            break;
            default:
                if (isspace(str[i])) {
                    while (isspace(str[i])) {
                        i++;
                    }
                } else if (isdigit(str[i])) {
                    std::string b = "0";
                    int base = 10;
                    if (str[i] == '0') {
                        i++;
                        switch (str[i]) {
                            case 'b':
                                i++;
                                base = 2;
                            break;
                            case 'o':
                                i++;
                                base = 8;
                            break;
                            case 'x':
                                i++;
                                base = 16;
                            break;
                            default:
                                base = 10;
                        }
                    }
                    if (base == 16) {
                        while (strchr(hexChars, str[i]) && str[i] != '\0') {
                            b.push_back(str[i++]);
                        }
                    } else {
                        while (isdigit(str[i]) && str[i] <= base+'0') {
                            b.push_back(str[i++]);
                        }
                        if (base == 10 && str[i] == '.') {
                            i++;
                            b.push_back('.');
                            while (isdigit(str[i])) {
                                b.push_back(str[i++]);
                            }
                        }
                    }
                    tokens.push_back({T_NUMBER, {get_number(b, base)}});
                } else if (i+3 < str.size() && str.substr(i, 4) == "true") {
                    i += 4;
                    tokens.push_back({T_BOOL, {true}});
                } else if (i+4 < str.size() && str.substr(i, 5) == "false") {
                    i += 5;
                    tokens.push_back({T_BOOL, {false}});
                } else {
                    std::cerr << "Invalid JSON character '" << str[i] << "'\n";
                    exit(1);
                }
        }
    }
}

me::Json::TokenValue me::Json::match(TokenType type, std::string str) {
    if (tokens[i].type != type) {
        std::cerr << "expected \"" << str << "\"\n";
        exit(1);
    }
    return tokens[i++].value;
}

unsigned long me::Json::parse() {
    switch (tokens[i].type) {
        case T_STRING:
            s_data.insert({s_id, {N_STRING, {tokens[i++].value.str}}});
            return s_id++;
        break;
        case T_NUMBER:
            s_data.insert({s_id, {N_NUMBER, {tokens[i++].value.number}}});
            return s_id++;
        break;
        case T_BOOL:
            s_data.insert({s_id, {N_BOOL, {tokens[i++].value.b}}});
            return s_id++;
        break;
        case T_BOPEN: {
            i++;
            std::vector<unsigned long> list;
            if (tokens[i].type == T_BCLOSE) {
                i++;
                s_data.insert({s_id, {N_LIST, {list}}});
                return s_id++;
            }
            list.push_back(parse());
            while (tokens[i].type == T_COMMA) {
                i++;
                list.push_back(parse());
            }
            match(T_BCLOSE, "]");
            s_data.insert({s_id, {N_LIST, {list}}});
            return s_id++;
        }
        break;
        case T_COPEN: {
            i++;
            std::map<std::string, unsigned long> object;
            if (tokens[i].type == T_CCLOSE) {
                i++;
                s_data.insert({s_id,{ N_OBJECT, {object}}});
                return s_id++;
            }
            std::string str = match(T_STRING, "string").str;
            match(T_COLON, ":");
            if (!object.insert({str, parse()}).second) {
                std::cerr << "Duplicate key \"" << str << "\"\n";
                exit(1);
            }
            while (tokens[i].type == T_COMMA) {
                i++;
                str = match(T_STRING, "string").str;
                match(T_COLON, ":");
                if (!object.insert({str, parse()}).second) {
                    std::cerr << "Duplicate key \"" << str << "\"\n";
                    exit(1);
                }
            }
            match(T_CCLOSE, "}");
            s_data.insert({s_id, {N_OBJECT, {object}}});
            return s_id++;
        }
        break;
        default:
            std::cerr << "Unexpected token " << tokens[i].type << "\n";
            exit(1);
    }
}

std::string me::Json::hex(unsigned char c) {
    std::stringstream ss;
    int i1 = c/16, i2 = c%16;
    ss << hexChars[i1] << hexChars[i2];
    return ss.str();
}

std::string me::Json::prep(std::string str) {
    std::stringstream ss;
    ss << '"';
    for (char c : str) {
        switch (c) {
            case '\0':
                ss << "\\0";
            break;
            case '\a':
                ss << "\\a";
            break;
            case '\b':
                ss << "\\b";
            break;
            case '\e':
                ss << "\\e";
            break;
            case '\f':
                ss << "\\f";
            break;
            case '\n':
                ss << "\\n";
            break;
            case '\r':
                ss << "\\r";
            break;
            case '\t':
                ss << "\\t";
            break;
            case '\v':
                ss << "\\v";
            break;
            case '"':
                ss << "\\\"";
            break;
            default:
                if (!isprint(c)) {
                    ss << "\\x" << hex(c);
                } else {
                    ss << c;
                }
        }
    }
    ss << '"';
    return ss.str();
}

std::string me::Json::str(unsigned long id, std::string indent) {
    std::stringstream ss;
    Node& node = s_data[id];
    switch (node.type) {
        case N_BOOL:
            if (node.value.b) {
                return "true";
            }
            return "false";
        break;
        case N_NUMBER:
            ss << node.value.number;
            return ss.str();
        break;
        case N_STRING:
            return prep(node.value.str);
        break;
        case N_LIST:
            if (node.value.list.empty()) {
                return "[]";
            }
            ss << "[\n";
            for (unsigned long i = 0; i < node.value.list.size(); i++) {
                unsigned long n = node.value.list[i];
                ss << indent << "    " << str(n, indent+"    ");
                if (i+1 < node.value.list.size()) {
                    ss << ',';
                }
                ss << '\n';
            }
            ss << indent << "]";
            return ss.str();
        break;
        case N_OBJECT:
            if (node.value.object.empty()) {
                return "{}";
            }
            ss << "{\n";
            for (std::map<std::string, unsigned long>::iterator it = node.value.object.begin(); it != node.value.object.end(); it++) {
                ss << indent << "    " << prep(it->first) << ": " << str(it->second, indent+"    ");
                if (++it != node.value.object.end()) {
                    ss << ',';
                }
                it--;
                ss << '\n';
            }
            ss << indent << "}";
            return ss.str();
        break;
    }
    std::cerr << "Unreachable: me::Json::str\n";
    exit(1);
}

me::Json::Json(unsigned long id) : value(id) {
    s_data[id].save = true;
}

unsigned long me::Json::copy(unsigned long id) {
    Node& node = s_data[id];
    switch (node.type) {
        case N_BOOL:
            s_data.insert({s_id, {N_BOOL, {node.value.b}}});
            return s_id++;
        break;
        case N_NUMBER:
            s_data.insert({s_id, {N_NUMBER, {node.value.number}}});
            return s_id++;
        break;
        case N_STRING:
            s_data.insert({s_id, {N_STRING, {node.value.str}}});
            return s_id++;
        break;
        case N_LIST: {
            std::vector<unsigned long> list;
            for (unsigned long n : node.value.list) {
                list.push_back(copy(n));
            }
            s_data.insert({s_id, {N_LIST, {list}}});
            return s_id++;
        }
        break;
        case N_OBJECT: {
            std::map<std::string, unsigned long> object;
            for (std::pair<std::string, unsigned long> n : node.value.object) {
                object.insert({n.first, copy(n.second)});
            }
            s_data.insert({s_id, {N_OBJECT, {object}}});
            return s_id++;
        }
        break;
    }
    std::cerr << "Unreachable: me::Json::copy\n";
    exit(1);
}

std::vector<std::string> me::Json::keys(unsigned long id) {
    Node& node = s_data[id];
    if (node.type != N_OBJECT) {
        std::cerr << "keys is only available for objects\n";
        exit(1);
    }
    std::vector<std::string> ret;
    for (std::pair<std::string, unsigned long> n : node.value.object) {
        ret.push_back(n.first);
    }
    return ret;
}

bool me::Json::cmp(unsigned long id1, unsigned long id2) {
    if (id1 == id2) {
        return true;
    }
    Node& n1 = s_data[id1];
    Node& n2 = s_data[id2];
    if (n1.type != n2.type) {
        return false;
    }
    switch (n1.type) {
        case N_BOOL:
            return n1.value.b == n2.value.b;
        break;
        case N_NUMBER:
            return n1.value.number == n2.value.number;
        break;
        case N_STRING:
            return n1.value.str == n2.value.str;
        break;
        case N_LIST:
            if (n1.value.list.size() != n2.value.list.size()) {
                return false;
            }
            for (unsigned long i = 0; i < n1.value.list.size(); i++) {
                if (!cmp(n1.value.list[i], n2.value.list[i])) {
                    return false;
                }
            }
            return true;
        break;
        case N_OBJECT:
            if (keys(id1) != keys(id2)) {
                return false;
            }
            for (std::string key : keys(id1)) {
                if (!cmp(n1.value.object[key], n2.value.object[key])) {
                    return false;
                }
            }
            return true;
        break;
    }
    std::cerr << "Unreachable: me::Json::cmp\n";
    exit(1);
}

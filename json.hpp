#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

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

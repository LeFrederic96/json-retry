#include <iostream>
#include <string>
#include <vector>
#include <map>

#define INDENT "    "

namespace me {
class Json {
private:
    enum NodeType {
        N_BOOL,
        N_NUMBER,
        N_STRING,
        N_LIST,
        N_OBJECT,
    };

public:
    class JsonValue;

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
        friend JsonValue;

        bool operator==(iterator other);

        bool operator!=(iterator other);

        iterator operator++();

        iterator operator--();

        iterator operator++(int);

        iterator operator--(int);

        iterator operator+(unsigned long step);

        iterator operator-(unsigned long step);

        iterator operator+=(unsigned long step);

        iterator operator-=(unsigned long step);

        iteratorGetClass operator*();

        unsigned long index();

        std::string key();

    private:
        std::vector<std::string> keys;
        unsigned long size;
        unsigned long idx;
        NodeType type;
        unsigned long id;

        iterator(unsigned long id, unsigned long index);

        class iteratorGetClass {
        public:
            iteratorGetClass(unsigned long id, unsigned long idx, std::vector<std::string>* keys, NodeType type);

            operator JsonValue();

            template<typename type>
            operator type() {
                if (this->type != N_LIST) {
                    std::cerr << "cannot get non pair from object iterator\n";
                    exit(1);
                }
                Node& node = data[id];
                getClass tmp(node.value.list[idx]);
                return tmp;
            }

            template<typename type>
            operator std::pair<std::string, type>() {
                if (this->type != N_OBJECT) {
                    std::cerr << "cannot get pair from non object iterator\n";
                    exit(1);
                }
                Node& node = data[id];
                getClass tmp(node.value.object[(*keys)[idx]]);
                return {(*keys)[idx], tmp};
            }

        private:
            unsigned long id;
            unsigned long idx;
            std::vector<std::string>* keys;
            NodeType type;
        };
    };

    class JsonValue {
    public:
        friend class Json;

        JsonValue();
        JsonValue(unsigned long id);

        void set(unsigned long id);

        operator Json();

        Type type();

        std::string str();

        JsonValue operator[](unsigned long i);

        JsonValue operator[](std::string str);

        void operator=(Json other);

        template<typename type>
        void operator=(type other) {
            Json::destroy(id);
            createClass tmp(other);
            id = tmp.id;
        }

        unsigned long size();

        std::vector<std::string> keys();

        iterator begin();

        iterator end();

        template<typename type>
        type get() {
            getClass tmp(id);
            return tmp;
        }

        void push_back(Json value);

        template<typename type>
        void push_back(type value) {
            Node& node = data[id];
            if (node.type != N_LIST) {
                std::cerr << "cannot push_back a value onto a non list\n";
                exit(1);
            }
            createClass tmp(value);
            node.value.list.push_back(tmp.id);
        }

        void insert(unsigned long index, Json value);

        template<typename type>
        void insert(unsigned long index, type value) {
            Node& node = data[id];
            if (node.type != N_LIST) {
                std::cerr << "cannot insert a value by unsigned long into a non list\n";
                exit(1);
            }
            createClass tmp(value);
            node.value.list.insert(node.value.list.begin()+index, tmp.id);
        }

        bool insert(std::string key, Json value);

        template<typename type>
        bool insert(std::string key, type value) {
            Node& node = data[id];
            if (node.type != N_OBJECT) {
                std::cerr << "cannot inser a value by string into non object\n";
                exit(1);
            }
            createClass tmp(value);
            return node.value.object.insert({key, tmp.id}).second;
        }

        void erase(unsigned long index);

        void erase(std::string key);

        void erase(iterator pos);

        bool empty();

        void clear();

        iterator find(std::string key);

        iterator find(const char* key);

        template<typename type>
        iterator find(type value) {
            Node& node = data[id];
            if (node.type != N_OBJECT) {
                std::cerr << "cannot find value in non list\n";
                exit(1);
            }
            iterator it = begin();
            while (it != end() && *it != value) {
                it++;
            }
            return it;
        }

        bool operator==(Json other);

        bool operator==(JsonValue other);

        template<typename type>
        bool operator==(type other) {
            createClass tmp(other);
            bool ret = cmp(id, tmp.id);
            Json::destroy(tmp.id);
            return ret;
        }

        bool operator!=(Json other);

        bool operator!=(JsonValue other);

        template<typename type>
        bool operator!=(type other) {
            return !operator==(other);
        }

    private:
        unsigned long id;

        void destroy();
    };

    Json(std::string str);

    Json(Json& other);

    void operator=(Json other);

    template<typename type>
    void operator=(type other) {
        value.operator=(other);
    }

    Type type();

    std::string str();

    JsonValue operator[](unsigned long i);

    JsonValue operator[](std::string str);

    unsigned long size();

    std::vector<std::string> keys();

    iterator begin();

    iterator end();

    template<typename type>
    type get() {
        return value.get<type>();
    }

    template<typename type>
    static Json create(type v) {
        createClass tmp(v);
        return tmp.id;
    }

    void push_back(Json value);

    template<typename type>
    void push_back(type value) {
        this->value.push_back(value);
    }

    void insert(unsigned long index, Json value);

    template<typename type>
    void insert(unsigned long index, type value) {
        this->value.insert(index, value);
    }

    bool insert(std::string key, Json value);

    template<typename type>
    bool insert(std::string key, type value) {
        return this->value.insert(key, value);
    }

    void erase(unsigned long index);

    void erase(std::string key);

    void erase(iterator pos);

    bool empty();

    void clear();

    iterator find(std::string key);

    template<typename type>
    iterator find(type value) {
        return this->value.find(value);
    }

    bool operator==(Json other);

    bool operator==(JsonValue other);

    template<typename type>
    bool operator==(type other) {
        return value.operator==(other);
    }

    bool operator!=(Json other);

    bool operator!=(JsonValue other);

    template<typename type>
    bool operator!=(type other) {
        return value.operator!=(other);
    }

    ~Json();

private:
    enum TokenType {
        T_COPEN,
        T_CCLOSE,
        T_BOPEN,
        T_BCLOSE,
        T_COMMA,
        T_COLON,
        T_BOOL,
        T_NUMBER,
        T_STRING,
    };

    struct TokenValue {
        bool b;
        double number;
        std::string str;
    };

    struct Token {
        Token(TokenType type);
        Token(bool b);
        Token(double number);
        Token(std::string str);

        TokenType type;
        TokenValue value;
    };

    struct NodeValue {
        bool b;
        double number;
        std::string str;
        std::vector<unsigned long> list;
        std::map<std::string, unsigned long> object;
    };

    struct Node {
        Node();
        Node(bool b);
        Node(double number);
        Node(std::string str);
        Node(std::vector<unsigned long> list);
        Node(std::map<std::string, unsigned long> object);

        NodeType type;
        NodeValue value;
    };

    class getClass {
    public:
        getClass(unsigned long id);

        operator Json();

        operator bool();

        operator double();

        operator std::string();

        template<typename type>
        operator std::vector<type>() {
            Node& node = data[id];
            if (node.type != N_LIST) {
                std::cerr << "cannot get vector from non list\n";
                exit(1);
            }
            std::vector<type> list;
            for (unsigned long n : node.value.list) {
                getClass tmp(n);
                list.push_back(tmp);
            }
            return list;
        }

        template<typename type>
        operator std::map<std::string, type>() {
            Node& node = data[id];
            if (node.type != N_OBJECT) {
                std::cerr << "cannot get map from non object\n";
                exit(1);
            }
            std::map<std::string, type> object;
            for (std::pair<std::string, unsigned long> n : node.value.object) {
                getClass tmp(n.second);
                object.insert({n.first, tmp});
            }
            return object;
        }

        operator char();
        operator short();
        operator int();
        operator long();
        operator unsigned char();
        operator unsigned short();
        operator unsigned int();
        operator unsigned long();
        operator float();

    private:
        unsigned long id;
    };

    class createClass {
    public:
        unsigned long id;

        createClass(bool b);
        createClass(double number);
        createClass(std::string str);

        template<typename type>
        createClass(std::vector<type> vector) {
            std::vector<unsigned long> list;
            for (type v : vector) {
                createClass tmp(v);
                list.push_back(tmp.id);
            }
            data.insert({index, list});
            id = index++;
        }

        template<typename type>
        createClass(std::map<std::string, type> map) {
            std::map<std::string, unsigned long> object;
            for (std::pair<std::string, type> v : map) {
                createClass tmp(v.second);
                object.insert({v.first, tmp.id});
            }
            data.insert({index, object});
            id = index++;
        }

        createClass(char number);
        createClass(short number);
        createClass(int number);
        createClass(long number);
        createClass(unsigned char number);
        createClass(unsigned short number);
        createClass(unsigned int number);
        createClass(unsigned long number);
        createClass(float number);
    };

    static char hexChars[23];
    static std::map<unsigned long, Node> data;
    static unsigned long index;

    std::vector<Token> tokens;
    JsonValue value;
    unsigned long i = 0;

    Json(unsigned long id);

    char get_char(std::string str, int base);

    double get_number(std::string str, int base);

    unsigned long tokenize(std::string str);

    TokenValue match(TokenType type, std::string str);

    unsigned long parse();

    static std::string hex(char c);

    static std::string prep(std::string str);

    static std::string str(unsigned long id, std::string indent);

    static unsigned long copy(unsigned long id);

    static void destroy(unsigned long id);

    static bool cmp(unsigned long id1, unsigned long id2);
};
}

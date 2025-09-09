#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace me {
class Json {
private:
    struct Node;

    enum NodeType {
        N_OBJECT,
        N_LIST,
        N_STRING,
        N_NUMBER,
        N_BOOL,
    };

public:
    enum Type {
        JSON_OBJECT=N_OBJECT,
        JSON_LIST=N_LIST,
        JSON_STRING=N_STRING,
        JSON_NUMBER=N_NUMBER,
        JSON_BOOL=N_BOOL,
    };

    class iterator {
    private:
        class iteratorGetClass;
    public:
        iterator(Node* node, unsigned int index);

        bool operator==(iterator& other);

        bool operator!=(iterator& other);

        void operator++();

        void operator--();

        iteratorGetClass operator*();

    private:
        Node* node;
        std::vector<std::string> key;
        unsigned int size;
        unsigned int index;

        class iteratorGetClass {
        public:
            iteratorGetClass(Node* node, std::vector<std::string>* key, unsigned int size, unsigned int index);

            operator char();

            operator Json();

            operator std::pair<std::string, Json>();

        private:
            Node* node;
            std::vector<std::string>* key;
            unsigned int size;
            unsigned int index;
        };
    };

    class JsonValue {
    public:
        JsonValue(Node* n);

        JsonValue operator[](unsigned int index);

        JsonValue operator[](std::string key);

        std::string str();

        template<typename type>
        type get() {
            getClass tmp(node);
            return tmp;
        }

        void operator=(Json json);

        template<typename type>
        void operator=(type value) {
            createClass tmp(value);
            node->destroy();
            node->save = true;
            node->type = tmp.node->type;
            node->value = tmp.node->value;
        }

        Type type();

        unsigned int size();

        std::vector<std::string> keys();

        iterator begin();

        iterator end();

        void push_back(char c);

        void push_back(std::string str);

        void push_back(Json json);

        template<typename type>
        void push_back(type v) {
            if (node->type != N_LIST) {
                std::cerr << "cannot append item to non list\n";
                exit(1);
            }
            createClass tmp(v);
            node->value->list.push_back(tmp.node);
        }

        void insert(unsigned int index, char c);

        void insert(unsigned int index, std::string str);

        void insert(unsigned int index, Json json);

        template<typename type>
        void insert(unsigned int index, type value) {
            switch (node->type) {
                case N_LIST: {
                    createClass tmp(value);
                    std::vector<Node*>::iterator it(&node->value->list[index]);
                    node->value->list.insert(it, tmp.node);
                }
                break;
                default:
                    std::cerr << "cannot insert value\n";
                    exit(1);
            }
        }

        bool insert(std::string key, Json json);

        template<typename type>
        bool insert(std::string key, type value) {
            if (node->type != N_OBJECT) {
                std::cerr << "cannot insert value into object\n";
                exit(1);
            }
            createClass tmp(value);
            return node->value->object.insert({key, tmp.node}).second;
        }

        void erase(unsigned int index);

        void erase(unsigned int first, unsigned int last);

        void erase(std::string key);

        operator Json();

    private:
        Node* node;
    };

    Json(std::string str);

    Json(Json& other);

    template<typename type>
    static Json create(type value) {
        createClass tmp(value);
        return tmp.node;
    }

    std::string str();

    JsonValue operator[](unsigned int index);

    JsonValue operator[](std::string key);

    template<typename type>
    type get() {
        getClass tmp(node);
        return tmp;
    }

    void operator=(Json json);

    Type type();

    unsigned int size();

    std::vector<std::string> keys();

    iterator begin();

    iterator end();

    void push_back(char c);

    void push_back(std::string str);

    void push_back(Json json);

    template<typename type>
    void push_back(type v) {
        if (node->type != N_LIST) {
            std::cerr << "cannot append item to non list\n";
            exit(1);
        }
        createClass tmp(v);
        node->value->list.push_back(tmp.node);
    }

    void insert(unsigned int index, char c);

    void insert(unsigned int index, std::string str);

    void insert(unsigned int index, Json json);

    template<typename type>
    void insert(unsigned int index, type value) {
        switch (node->type) {
            case N_LIST: {
                createClass tmp(value);
                std::vector<Node*>::iterator it(&node->value->list[index]);
                node->value->list.insert(it, tmp.node);
            }
            break;
            default:
                std::cerr << "cannot insert value\n";
                exit(1);
        }
    }

    bool insert(std::string key, Json json);

    template<typename type>
    bool insert(std::string key, type value) {
        if (node->type != N_OBJECT) {
            std::cerr << "cannot insert value into object\n";
            exit(1);
        }
        createClass tmp(value);
        return node->value->object.insert({key, tmp.node}).second;
    }

    void erase(unsigned int index);

    void erase(unsigned int first, unsigned int last);

    void erase(std::string key);

    ~Json();

private:
    struct Token;

    static char hexChars[23];
    std::vector<Token>* tokens;
    unsigned int i = 0;
    static unsigned int g_id;
    Node* node;

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
        TokenValue(std::string& str);

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
        NodeValue(std::map<std::string, Node*>& object);
        NodeValue(std::vector<Node*>& list);
        NodeValue(std::string& str);
        NodeValue(double number);
        NodeValue(bool b);

        std::map<std::string, Node*> object;
        std::vector<Node*> list;
        std::string str;
        double number;
        bool b;
        bool save = false;
    };

    struct Node {
        void destroy();

        ~Node();

        NodeType type;
        NodeValue* value;
        bool destroyed;
        bool save;
        int id;
    };

    class getClass {
    public:
        getClass(Node* node);

        operator bool();

        operator double();

        operator int();

        operator std::string();

        template<typename type>
        operator std::vector<type>() {
            if (node->type != N_LIST) {
                std::cerr << "Cannot get vector from non list type\n";
                exit(1);
            }
            std::vector<type> ret;
            for (Node* n : node->value->list) {
                getClass tmp(n);
                ret.push_back(tmp);
            }
            return ret;
        }

        template<typename type>
        operator std::map<std::string, type>() {
            if (node->type != N_OBJECT) {
                std::cerr << "Cannot get map from non object type\n";
                exit(1);
            }
            std::map<std::string, type> ret;
            for (std::pair<std::string, Node*> n : node->value->object) {
                getClass tmp(n.second);
                ret.insert({n.first, tmp});
            }
            return ret;
        }

    private:
        Node* node;
    };

    class createClass {
    public:
        Node* node;

        createClass(bool b);

        createClass(int number);

        createClass(double number);

        createClass(std::string str);

        template<typename type>
        createClass(std::vector<type> vector) {
            std::vector<Node*> ret;
            for (type v : vector) {
                createClass tmp(v);
                ret.push_back(tmp.node);
            }
            node = makeNode(N_LIST, {ret});
        }

        template<typename type>
        createClass(std::map<std::string, type> map) {
            std::map<std::string, Node*> ret;
            for (std::pair<std::string, type> v : map) {
                createClass tmp(v.second);
                ret.insert({v.first, tmp.node});
            }
            node = makeNode(N_OBJECT, {ret});
        }
    };

    char get_char(std::string str, int base);

    double get_number(std::string& str, int base);

    void tokenize(std::string& str, std::vector<Token>& tokens);

    static Node* makeNode(NodeType type, NodeValue value);

    TokenValue match(TokenType type, std::string str);

    Node* parse();

    static std::string hex(unsigned char c);

    static std::string prep(std::string str);

    static std::string str(Node* node, std::string indent);

    static Node* copy(Node* node);

    Json(Node* node);
};
}

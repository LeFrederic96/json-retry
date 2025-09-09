#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cmath>
#include <map>
#include <sstream>

namespace me {
class Json {
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
        TokenValue() {}
        TokenValue(bool b) : b(b) {}
        TokenValue(double number) : number(number) {}
        TokenValue(std::string& str) : str(str) {}

        bool b;
        double number;
        std::string str;
    };

    struct Token {
        TokenType type;
        TokenValue value;
    };

    char get_char(std::string str, int base) {
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

    double get_number(std::string& str, int base) {
        double ret = 0;
        unsigned int i = 0;
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
        unsigned int j = 1;
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

    static char hexChars[23];
    void tokenize(std::string& str, std::vector<Token>& tokens) {
        std::string buf;
        unsigned int i = 0;
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

    enum NodeType {
        N_OBJECT,
        N_LIST,
        N_STRING,
        N_NUMBER,
        N_BOOL,
    };

    struct Node;
    struct NodeValue {
        NodeValue() {}
        NodeValue(std::map<std::string, Node*>& object) : object(object) {}
        NodeValue(std::vector<Node*>& list) : list(list) {}
        NodeValue(std::string& str) : str(str) {}
        NodeValue(double number) : number(number) {}
        NodeValue(bool b) : b(b) {}

        std::map<std::string, Node*> object;
        std::vector<Node*> list;
        std::string str;
        double number;
        bool b;
        bool save = false;
    };

    struct Node {
        NodeType type;
        NodeValue* value;
        bool destroyed;
        bool save;

        int id;

        void destroy() {
            if (destroyed) {
                return;
            }
            if (type == N_LIST) {
                for (Node* n : value->list) {
                    n->~Node();
                    free(n);
                }
            }
            if (type == N_OBJECT) {
                for (Node* n : value->list) {
                    n->~Node();
                    free(n);
                }
            }
            if (!value->save) {
                delete value;
            } else {
                value->save = false;
            }
            destroyed = true;
        }

        ~Node() {
            if (save) {
                return;
            }
            destroy();
        }
    };

    std::vector<Token>* tokens;
    unsigned int i = 0;
    static unsigned int g_id;

    static Node* makeNode(NodeType type, NodeValue value) {
        Node* ret = (Node*)malloc(sizeof(Node));
        ret->type = type;
        ret->value = new NodeValue(value);
        ret->destroyed = false;
        ret->save = false;

        ret->id = g_id++;

        return ret;
    }

    TokenValue match(TokenType type, std::string str) {
        if ((*tokens)[i].type != type) {
            std::cerr << "Expected \"" << str << "\"\n";
            exit(1);
        }
        return (*tokens)[i++].value;
    }

    Node* parse() {
        switch (tokens->operator[](i).type) {
            case T_STRING:
                return makeNode(N_STRING, {(*tokens)[i++].value.str});
            break;
            case T_NUMBER:
                return makeNode(N_NUMBER, {(*tokens)[i++].value.number});
            break;
            case T_BOOL:
                return makeNode(N_BOOL, {(*tokens)[i++].value.b});
            break;
            case T_BOPEN: {
                i++;
                std::vector<Node*> list;
                if ((*tokens)[i].type == T_BCLOSE) {
                    i++;
                    return makeNode(N_LIST, {list});
                }
                list.push_back(parse());
                while ((*tokens)[i].type == T_COMMA) {
                    i++;
                    list.push_back(parse());
                }
                match(T_BCLOSE, "]");
                return makeNode(N_LIST, {list});
            }
            break;
            case T_COPEN: {
                i++;
                std::map<std::string, Node*> object;
                if ((*tokens)[i].type == T_CCLOSE) {
                    i++;
                    return makeNode(N_OBJECT, {object});
                }
                std::string str = match(T_STRING, "string").str;
                match(T_COLON, ":");
                if (!object.insert({str, parse()}).second) {
                    std::cerr << "Duplicate key \"" << str << "\"\n";
                    exit(1);
                }
                while ((*tokens)[i].type == T_COMMA) {
                    i++;
                    str = match(T_STRING, "string").str;
                    match(T_COLON, ":");
                    if (!object.insert({str, parse()}).second) {
                        std::cerr << "Duplicate key \"" << str << "\"\n";
                        exit(1);
                    }
                }
                match(T_CCLOSE, "}");
                return makeNode(N_OBJECT, {object});
            }
            break;
            default:
                std::cerr << "Unexpected token " << (*tokens)[i].type << "\n";
                exit(1);
        }
    }

    Node* node;

    Json(Node* node) : node(node) {
        node->save = true;
    }

    static std::string hex(unsigned char c) {
        std::stringstream ss;
        int i1 = c/16, i2 = c%16;
        ss << hexChars[i1] << hexChars[i2];
        return ss.str();
    }

    static std::string prep(std::string str) {
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

    static std::string str(Node* node, std::string indent) {
        std::stringstream ss;
        switch (node->type) {
            case N_BOOL:
                if (node->value->b) {
                    return "true";
                }
                return "false";
            break;
            case N_NUMBER:
                ss << node->value->number;
                return ss.str();
            break;
            case N_STRING:
                return prep(node->value->str);
            break;
            case N_LIST:
                if (node->value->list.empty()) {
                    return "[]";
                }
                ss << "[\n";
                for (unsigned int i = 0; i < node->value->list.size(); i++) {
                    Node* n = node->value->list[i];
                    ss << indent << "    " << str(n, indent+"    ");
                    if (i+1 < node->value->list.size()) {
                        ss << ',';
                    }
                    ss << '\n';
                }
                ss << indent << "]";
                return ss.str();
            break;
            case N_OBJECT:
                if (node->value->object.empty()) {
                    return "{}";
                }
                ss << "{\n";
                for (std::map<std::string, Node*>::iterator it = node->value->object.begin(); it != node->value->object.end(); it++) {
                    ss << indent << "    " << prep(it->first) << " : " << str(it->second, indent+"    ");
                    if (++it != node->value->object.end()) {
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

    static Node* copy(Node* node) {
        switch (node->type) {
            case N_BOOL:
                return makeNode(N_BOOL, {node->value->b});
            break;
            case N_NUMBER:
                return makeNode(N_NUMBER, {node->value->number});
            break;
            case N_STRING:
                return makeNode(N_STRING, {node->value->str});
            break;
            case N_LIST: {
                std::vector<Node*> ret;
                for (Node* n : node->value->list) {
                    ret.push_back(copy(n));
                }
                return makeNode(N_LIST, {ret});
            }
            break;
            case N_OBJECT: {
                std::map<std::string, Node*> ret;
                for (std::pair<std::string, Node*> n : node->value->object) {
                    ret.insert({n.first, copy(n.second)});
                }
                return makeNode(N_OBJECT, {ret});
            }
        }
        std::cerr << "Unreachable: me::Json::copy\n";
        exit(1);
    }

    class getClass {
    public:
        getClass(Node* node) : node(node) {}

        operator bool() {
            if (node->type != N_BOOL) {
                std::cerr << "Cannot get bool from non bool type\n";
                exit(1);
            }
            return node->value->b;
        }

        operator double() {
            if (node->type != N_NUMBER) {
                std::cerr << "Cannot get double from non number type\n";
                exit(1);
            }
            return node->value->number;
        }

        operator int() {
            if (node->type != N_NUMBER) {
                std::cerr << "Cannot get integer from non number type\n";
                exit(1);
            }
            return node->value->number;
        }

        operator std::string() {
            if (node->type != N_STRING) {
                std::cerr << "Cannot get string from non string type\n";
                exit(1);
            }
            return node->value->str;
        }

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

        createClass(bool b) {
            node = makeNode(N_BOOL, {b});
        }

        createClass(int number) {
            node = makeNode(N_NUMBER, {(double)number});
        }

        createClass(double number) {
            node = makeNode(N_NUMBER, {number});
        }

        createClass(std::string str) {
            node = makeNode(N_STRING, {str});
        }

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
        Node* node;
        std::vector<std::string> key;
        unsigned int size;
        unsigned int index;

        class getClass {
        public:
            getClass(Node* node, std::vector<std::string>* key, unsigned int size, unsigned int index) : node(node), key(key), size(size), index(index) {}

            operator char() {
                if (node->type != N_STRING) {
                    std::cerr << "cannot get char from non string iterator\n";
                    exit(1);
                }
                if (index == size) {
                    std::cerr << "cannot get char from end iterator\n";
                    exit(1);
                }
                return node->value->str[index];
            }

            operator Json() {
                if (node->type != N_LIST) {
                    std::cerr << "cannot get Json from non list iterator\n";
                    exit(1);
                }
                if (index == size) {
                    std::cerr << "cannot get Json from end iterator\n";
                    exit(1);
                }
                return node->value->list[index];
            }

            operator std::pair<std::string, Json>() {
                if (node->type != N_OBJECT) {
                    std::cerr << "cannot get pair from non object iterator\n";
                    exit(1);
                }
                if (index == size) {
                    std::cerr << "cannot get pair from end iterator\n";
                    exit(1);
                }
                Json tmp(node->value->object[(*key)[index]]);
                return {(*key)[index], tmp};
            }

        private:
            Node* node;
            std::vector<std::string>* key;
            unsigned int size;
            unsigned int index;
        };

    public:
        iterator(Node* node, unsigned int index) : node(node), index(index) {
            bool b = false;
            if (node->type == N_OBJECT) {
                b = true;
                for (std::pair<std::string, Node*> n : node->value->object) {
                    key.push_back(n.first);
                    size = key.size();
                }
            } else if (node->type == N_LIST) {
                b = true;
                size = node->value->list.size();
            } else if (node->type == N_STRING) {
                b = true;
                size = node->value->str.size();
            }
            if (!b) {
                std::cerr << "cannot create iterator, invalid type\n";
                exit(1);
            }
            if (b && index > size) {
                std::cerr << "invalid index " << index << " for Json::iterator with size " << size << "\n";
                exit(1);
            }
        }

        bool operator==(iterator& other) {
            return (node==other.node && index==other.index);
        }

        bool operator!=(iterator& other) {
            return !(operator==(other));
        }

        void operator++() {
            if (index >= size) {
                std::cerr << "cannot increment end iterator\n";
                exit(1);
            }
            index++;
        }

        void operator--() {
            if (index == 0) {
                std::cerr << "cannot decrement begin iterator\n";
                exit(1);
            }
            index--;
        }

        getClass operator*() {
            return {node, &key, size, index};
        }
    };

    class JsonValue {
    public:
        JsonValue(Node* n) {
            node = n;
        }

        JsonValue operator[](unsigned int index) {
            if (node->type != N_LIST) {
                std::cerr << "operator[] with int is only available for lists\n";
                exit(1);
            }
            return {node->value->list[index]};
        }

        JsonValue operator[](std::string key) {
            if (node->type != N_OBJECT) {
                std::cerr << "operator[] with std::string is only available for objects\n";
                exit(1);
            }
            return {node->value->object[key]};
        }

        std::string str() {
            return me::Json::str(node, "");
        }

        template<typename type>
        type get() {
            getClass tmp(node);
            return tmp;
        }

        void operator=(Json json) {
            Node* tmp = copy(json.node);
            node->destroy();
            node->destroyed = false;
            node->save = true;
            node->type = tmp->type;
            node->value = tmp->value;
            free(tmp);
        }

        template<typename type>
        void operator=(type value) {
            createClass tmp(value);
            node->destroy();
            node->save = true;
            node->type = tmp.node->type;
            node->value = tmp.node->value;
        }

        Type type() {
            return (Type)node->type;
        }

        unsigned int size() {
            switch (node->type) {
                case N_STRING:
                    return node->value->str.size();
                break;
                case N_LIST:
                    return node->value->list.size();
                break;
                case N_OBJECT:
                    return node->value->object.size();
                break;
                default:
                    std::cerr << "size is not available for bool and number\n";
                    exit(1);
            }
        }

        std::vector<std::string> keys() {
            if (node->type != N_OBJECT) {
                std::cerr << "keys is only available for objects\n";
                exit(1);
            }
            std::vector<std::string> ret;
            for (std::pair<std::string, Node*> n : node->value->object) {
                ret.push_back(n.first);
            }
            return ret;
        }

        iterator begin() {
            return {node, 0};
        }

        iterator end() {
            return {node, size()};
        }

        void push_back(char c) {
            if (node->type != N_STRING) {
                std::cerr << "cannot append char to non string\n";
                exit(1);
            }
            node->value->str.push_back(c);
        }

        void push_back(std::string str) {
            if (node->type != N_STRING) {
                std::cerr << "cannot append string to non string\n";
                exit(1);
            }
            for (char c : str) {
                node->value->str.push_back(c);
            }
        }

        void push_back(Json json) {
            if (node->type != N_LIST) {
                std::cerr << "cannot append item to non list\n";
                exit(1);
            }
            node->value->list.push_back(copy(json.node));
        }

        template<typename type>
        void push_back(type v) {
            if (node->type != N_LIST) {
                std::cerr << "cannot append item to non list\n";
                exit(1);
            }
            createClass tmp(v);
            node->value->list.push_back(tmp.node);
        }

        void insert(unsigned int index, char c) {
            if (node->type != N_STRING) {
                std::cerr << "cannot insert char into non string\n";
                exit(1);
            }
            std::string tmp;
            tmp.push_back(c);
            node->value->str.insert(index, tmp);
        }

        void insert(unsigned int index, std::string str) {
            switch (node->type) {
                case N_STRING:
                    node->value->str.insert(index, str);
                break;
                case N_LIST: {
                    createClass tmp(str);
                    std::vector<Node*>::iterator it(&node->value->list[index]);
                    node->value->list.insert(it, tmp.node);
                }
                break;
                default:
                    std::cerr << "cannot insert string\n";
                    exit(1);
            }
        }

        void insert(unsigned int index, Json json) {
            switch (node->type) {
                case N_LIST: {
                    std::vector<Node*>::iterator tmp(&node->value->list[index]);
                    node->value->list.insert(tmp, copy(json.node));
                }
                break;
                default:
                    std::cerr << "cannot inset json\n";
                    exit(1);
            }
        }

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

        bool insert(std::string key, Json json) {
            if (node->type != N_OBJECT) {
                std::cerr << "cannot insert Json into object\n";
                exit(1);
            }
            return node->value->object.insert({key, copy(json.node)}).second;
        }

        template<typename type>
        bool insert(std::string key, type value) {
            if (node->type != N_OBJECT) {
                std::cerr << "cannot insert value into object\n";
                exit(1);
            }
            createClass tmp(value);
            return node->value->object.insert({key, tmp.node}).second;
        }

        void erase(unsigned int index) {
            switch (node->type) {
                case N_STRING:
                    if (index >= node->value->str.size()) {
                        std::cerr << "cannot erase char, index out of bounds\n";
                        exit(1);
                    }
                    node->value->str.erase(index, 1);
                break;
                case N_LIST: {
                    if (index >= node->value->list.size()) {
                        std::cerr << "cannot erase value, index out of bounds\n";
                        exit(1);
                    }
                    node->value->list[index]->~Node();
                    std::vector<Node*>::iterator tmp(&node->value->list[index]);
                    node->value->list.erase(tmp);
                }
                break;
                default:
                    std::cerr << "cannot erase value, invalid type\n";
                    exit(1);
            }
        }

        void erase(unsigned int first, unsigned int last) {
            switch (node->type) {
                case N_STRING:
                    if (first >= node->value->str.size() || last > node->value->str.size() || first >= last) {
                        std::cerr << "cannot erase chars, indecies invalid\n";
                        exit(1);
                    }
                    node->value->str.erase(first, last-first);
                break;
                case N_LIST: {
                    if (first >= node->value->list.size() || last > node->value->list.size() || first >= last) {
                        std::cerr << "cannot erase value, indecies invalid\n";
                        exit(1);
                    }
                    delete node->value->list[first];
                    std::vector<Node*>::iterator tmp(&node->value->list[first]);
                    node->value->list.erase(tmp);
                }
                break;
                default:
                    std::cerr << "cannot erase value, invalid type\n";
                    exit(1);
            }
        }

        void erase(std::string key) {
            if (node->type != N_OBJECT) {
                std::cerr << "cannot erase value, from non object\n";
                exit(1);
            }
            if (node->value->object.find(key) == node->value->object.end()) {
                std::cerr << "cannot erase value, invalid key\n";
                exit(1);
            }
            delete node->value->object[key];
            node->value->object.erase(key);
        }

        operator Json() {
            return node;
        }
    private:
        Node* node;
    };

    Json(std::string str) {
        std::vector<Token> tokens;
        tokenize(str, tokens);
        this->tokens = &tokens;
        node = parse();
        node->save = true;
    }

    Json(Json& other) {
        node = copy(other.node);
        node->save = true;
    }

    template<typename type>
    static Json create(type value) {
        createClass tmp(value);
        return tmp.node;
    }

    std::string str() {
        return str(node, "");
    }

    JsonValue operator[](unsigned int index) {
        if (node->type != N_LIST) {
            std::cerr << "operator[] with int is only available for lists\n";
            exit(1);
        }
        return {node->value->list[index]};
    }

    JsonValue operator[](std::string key) {
        if (node->type != N_OBJECT) {
            std::cerr << "operator[] with std::string is only available for objects\n";
            exit(1);
        }
        return {node->value->object[key]};
    }

    template<typename type>
    type get() {
        getClass tmp(node);
        return tmp;
    }

    void operator=(Json json) {
        Node* tmp = copy(json.node);
        node->destroy();
        node->destroyed = false;
        node->save = true;
        node->type = tmp->type;
        node->value = tmp->value;
        free(tmp);
    }

    Type type() {
        return (Type)node->type;
    }

    unsigned int size() {
        switch (node->type) {
            case N_STRING:
                return node->value->str.size();
            break;
            case N_LIST:
                return node->value->list.size();
            break;
            case N_OBJECT:
                return node->value->object.size();
            break;
            default:
                std::cerr << "size is not available for bool and number\n";
                exit(1);
        }
    }

    std::vector<std::string> keys() {
        if (node->type != N_OBJECT) {
            std::cerr << "keys is only available for objects\n";
            exit(1);
        }
        std::vector<std::string> ret;
        for (std::pair<std::string, Node*> n : node->value->object) {
            ret.push_back(n.first);
        }
        return ret;
    }

    iterator begin() {
        return {node, 0};
    }

    iterator end() {
        return {node, size()};
    }

    void push_back(char c) {
        if (node->type != N_STRING) {
            std::cerr << "cannot append char to non string\n";
            exit(1);
        }
        node->value->str.push_back(c);
    }

    void push_back(std::string str) {
        if (node->type != N_STRING) {
            std::cerr << "cannot append string to non string\n";
            exit(1);
        }
        for (char c : str) {
            node->value->str.push_back(c);
        }
    }

    void push_back(Json json) {
        if (node->type != N_LIST) {
            std::cerr << "cannot append item to non list\n";
            exit(1);
        }
        node->value->list.push_back(copy(json.node));
    }

    template<typename type>
    void push_back(type v) {
        if (node->type != N_LIST) {
            std::cerr << "cannot append item to non list\n";
            exit(1);
        }
        createClass tmp(v);
        node->value->list.push_back(tmp.node);
    }

    void insert(unsigned int index, char c) {
        if (node->type != N_STRING) {
            std::cerr << "cannot insert char into non string\n";
            exit(1);
        }
        std::string tmp;
        tmp.push_back(c);
        node->value->str.insert(index, tmp);
    }

    void insert(unsigned int index, std::string str) {
        switch (node->type) {
            case N_STRING:
                node->value->str.insert(index, str);
            break;
            case N_LIST: {
                createClass tmp(str);
                std::vector<Node*>::iterator it(&node->value->list[index]);
                node->value->list.insert(it, tmp.node);
            }
            break;
            default:
                std::cerr << "cannot insert string\n";
                exit(1);
        }
    }

    void insert(unsigned int index, Json json) {
        switch (node->type) {
            case N_LIST: {
                std::vector<Node*>::iterator tmp(&node->value->list[index]);
                node->value->list.insert(tmp, copy(json.node));
            }
            break;
            default:
                std::cerr << "cannot inset json\n";
                exit(1);
        }
    }

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

    bool insert(std::string key, Json json) {
        if (node->type != N_OBJECT) {
            std::cerr << "cannot insert Json into object\n";
            exit(1);
        }
        return node->value->object.insert({key, copy(json.node)}).second;
    }

    template<typename type>
    bool insert(std::string key, type value) {
        if (node->type != N_OBJECT) {
            std::cerr << "cannot insert value into object\n";
            exit(1);
        }
        createClass tmp(value);
        return node->value->object.insert({key, tmp.node}).second;
    }

    void erase(unsigned int index) {
        switch (node->type) {
            case N_STRING:
                if (index >= node->value->str.size()) {
                    std::cerr << "cannot erase char, index out of bounds\n";
                    exit(1);
                }
                node->value->str.erase(index, 1);
            break;
            case N_LIST: {
                if (index >= node->value->list.size()) {
                    std::cerr << "cannot erase value, index out of bounds\n";
                    exit(1);
                }
                node->value->list[index]->~Node();
                std::vector<Node*>::iterator tmp(&node->value->list[index]);
                node->value->list.erase(tmp);
            }
            break;
            default:
                std::cerr << "cannot erase value, invalid type\n";
                exit(1);
        }
    }

    void erase(unsigned int first, unsigned int last) {
        switch (node->type) {
            case N_STRING:
                if (first >= node->value->str.size() || last > node->value->str.size() || first >= last) {
                    std::cerr << "cannot erase chars, indecies invalid\n";
                    exit(1);
                }
                node->value->str.erase(first, last-first);
            break;
            case N_LIST: {
                if (first >= node->value->list.size() || last > node->value->list.size() || first >= last) {
                    std::cerr << "cannot erase value, indecies invalid\n";
                    exit(1);
                }
                delete node->value->list[first];
                std::vector<Node*>::iterator tmp(&node->value->list[first]);
                node->value->list.erase(tmp);
            }
            break;
            default:
                std::cerr << "cannot erase value, invalid type\n";
                exit(1);
        }
    }

    void erase(std::string key) {
        if (node->type != N_OBJECT) {
            std::cerr << "cannot erase value, from non object\n";
            exit(1);
        }
        if (node->value->object.find(key) == node->value->object.end()) {
            std::cerr << "cannot erase value, invalid key\n";
            exit(1);
        }
        delete node->value->object[key];
        node->value->object.erase(key);
    }

    ~Json() {
        node->save = false;
        node->~Node();
    }
};
}

char me::Json::hexChars[23] = "0123456789abcdefABCDEF";
unsigned int me::Json::g_id = 0;

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace me {
    class Json {
    private:
        struct Node;

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

        char get_char(std::string str, int base);

        double get_number(std::string& str, int base);

        static char hexChars[23];

        void tokenize(std::string& str, std::vector<Token>& tokens);

        enum NodeType {
            N_OBJECT,
            N_LIST,
            N_STRING,
            N_NUMBER,
            N_BOOL,
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
            NodeType type;
            NodeValue* value;
            bool destroyed;
            bool save;
            int id;

            void destroy();

            ~Node();
        };

        std::vector<Token>* tokens;
        unsigned int i = 0;
        static unsigned int g_id;

        static Node* makeNode(NodeType type, NodeValue value);

        TokenValue match(TokenType type, std::string str);

        Node* parse();

        Node* node;

        Json(Node* node);

        static std::string hex(unsigned char c);

        static std::string prep(std::string str);

        static std::string str(Node* node, std::string indent);

        static Node* copy(Node* node);

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

    public:
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

        enum Type {
            JSON_OBJECT=N_OBJECT,
            JSON_LIST=N_LIST,
            JSON_STRING=N_STRING,
            JSON_NUMBER=N_NUMBER,
            JSON_BOOL=N_BOOL,
        };

        ~Json();
    };
}

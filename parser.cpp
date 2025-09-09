#include "json-dev.hpp"

unsigned int me::Json::g_id = 0;

me::Json::NodeValue::NodeValue() {}

me::Json::NodeValue::NodeValue(std::map<std::string, Node*>& object) : object(object) {}

me::Json::NodeValue::NodeValue(std::vector<Node*>& list) : list(list) {}

me::Json::NodeValue::NodeValue(std::string& str) : str(str) {}

me::Json::NodeValue::NodeValue(double number) : number(number) {}

me::Json::NodeValue::NodeValue(bool b) : b(b) {}

void me::Json::Node::destroy() {
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

me::Json::Node::~Node() {
    if (save) {
        return;
    }
    destroy();
}

me::Json::Node* me::Json::makeNode(NodeType type, NodeValue value) {
    Node* ret = (Node*)malloc(sizeof(Node));
    ret->type = type;
    ret->value = new NodeValue(value);
    ret->destroyed = false;
    ret->save = false;
    ret->id = g_id++;
    return ret;
}

me::Json::TokenValue me::Json::match(TokenType type, std::string str) {
    if ((*tokens)[i].type != type) {
        std::cerr << "Expected \"" << str << "\"\n";
        exit(1);
    }
    return (*tokens)[i++].value;
}

me::Json::Node* me::Json::parse() {
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

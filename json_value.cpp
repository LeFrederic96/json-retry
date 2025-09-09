#include "json-dev.hpp"

me::Json::JsonValue::JsonValue() {}

me::Json::JsonValue::JsonValue(Node* n) {
    node = n;
}

void me::Json::JsonValue::set(Node* n) {
    node = n;
}

me::Json::JsonValue me::Json::JsonValue::operator[](unsigned int index) {
    if (node->type != N_LIST) {
        std::cerr << "operator[] with int is only available for lists\n";
        exit(1);
    }
    return {node->value->list[index]};
}

me::Json::JsonValue me::Json::JsonValue::operator[](std::string key) {
    if (node->type != N_OBJECT) {
        std::cerr << "operator[] with std::string is only available for objects\n";
        exit(1);
    }
    return {node->value->object[key]};
}

std::string me::Json::JsonValue::str() {
    return me::Json::str(node, "");
}

void me::Json::JsonValue::operator=(Json json) {
    Node* tmp = copy(json.node);
    node->destroy();
    node->destroyed = false;
    node->save = true;
    node->type = tmp->type;
    node->value = tmp->value;
    free(tmp);
}

me::Json::Type me::Json::JsonValue::type() {
    return (Type)node->type;
}

unsigned int me::Json::JsonValue::size() {
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

std::vector<std::string> me::Json::JsonValue::keys() {
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

me::Json::iterator me::Json::JsonValue::begin() {
    return {node, 0};
}

me::Json::iterator me::Json::JsonValue::end() {
    return {node, size()};
}

void me::Json::JsonValue::push_back(char c) {
    if (node->type != N_STRING) {
        std::cerr << "cannot append char to non string\n";
        exit(1);
    }
    node->value->str.push_back(c);
}

void me::Json::JsonValue::push_back(std::string str) {
    if (node->type != N_STRING) {
        std::cerr << "cannot append string to non string\n";
        exit(1);
    }
    for (char c : str) {
        node->value->str.push_back(c);
    }
}

void me::Json::JsonValue::push_back(Json json) {
    if (node->type != N_LIST) {
        std::cerr << "cannot append item to non list\n";
        exit(1);
    }
    node->value->list.push_back(copy(json.node));
}

void me::Json::JsonValue::insert(unsigned int index, char c) {
    if (node->type != N_STRING) {
        std::cerr << "cannot insert char into non string\n";
        exit(1);
    }
    std::string tmp;
    tmp.push_back(c);
    node->value->str.insert(index, tmp);
}

void me::Json::JsonValue::insert(unsigned int index, std::string str) {
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

void me::Json::JsonValue::insert(unsigned int index, Json json) {
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

bool me::Json::JsonValue::insert(std::string key, Json json) {
    if (node->type != N_OBJECT) {
        std::cerr << "cannot insert Json into object\n";
        exit(1);
    }
    return node->value->object.insert({key, copy(json.node)}).second;
}

void me::Json::JsonValue::erase(unsigned int index) {
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

void me::Json::JsonValue::erase(unsigned int first, unsigned int last) {
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

void me::Json::JsonValue::erase(std::string key) {
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

me::Json::JsonValue::operator Json() {
    return node;
}

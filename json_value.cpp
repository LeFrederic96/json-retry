#include "json.hpp"

me::Json::JsonValue::JsonValue(Node* n) {
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

me::Json::JsonValue::operator Json() {
    return node;
}

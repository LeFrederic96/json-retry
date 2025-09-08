#include "json.hpp"

me::Json::Json(std::string str) {
    std::vector<Token> tokens;
    tokenize(str, tokens);
    this->tokens = &tokens;
    node = parse();
    node->save = true;
}

me::Json::Json(Json& other) {
    node = copy(other.node);
    node->save = true;
}

me::Json::Json(Node* node) : node(node) {
    node->save = true;
}

std::string me::Json::str() {
    return str(node, "");
}

me::Json::JsonValue me::Json::operator[](unsigned int index) {
    if (node->type != N_LIST) {
        std::cerr << "operator[] with int is only available for lists\n";
        exit(1);
    }
    return {node->value->list[index]};
}

me::Json::JsonValue me::Json::operator[](std::string key) {
    if (node->type != N_OBJECT) {
        std::cerr << "operator[] with std::string is only available for objects\n";
        exit(1);
    }
    return {node->value->object[key]};
}

void me::Json::operator=(Json json) {
    Node* tmp = copy(json.node);
    node->destroy();
    node->destroyed = false;
    node->save = true;
    node->type = tmp->type;
    node->value = tmp->value;
    free(tmp);
}

me::Json::~Json() {
    node->save = false;
    node->~Node();
}


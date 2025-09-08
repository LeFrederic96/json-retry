#include "json.hpp"

me::Json::getClass::getClass(Node* node) : node(node) {}

me::Json::getClass::operator bool() {
    if (node->type != N_BOOL) {
        std::cerr << "Cannot get bool from non bool type\n";
        exit(1);
    }
    return node->value->b;
}

me::Json::getClass::operator double() {
    if (node->type != N_NUMBER) {
        std::cerr << "Cannot get double from non number type\n";
        exit(1);
    }
    return node->value->number;
}

me::Json::getClass::operator int() {
    if (node->type != N_NUMBER) {
        std::cerr << "Cannot get integer from non number type\n";
        exit(1);
    }
    return node->value->number;
}

me::Json::getClass::operator std::string() {
    if (node->type != N_STRING) {
        std::cerr << "Cannot get string from non string type\n";
        exit(1);
    }
    return node->value->str;
}

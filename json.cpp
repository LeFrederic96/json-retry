#include "json-dev.hpp"

me::Json::Json(Node* node) : node(node), value(node) {
    node->save = true;
}

me::Json::Json(std::string str) {
    std::vector<Token> tokens;
    tokenize(str, tokens);
    this->tokens = &tokens;
    node = parse();
    node->save = true;
    value.set(node);
}

me::Json::Json(Json& other) {
    node = copy(other.node);
    node->save = true;
    value.set(node);
}

std::string me::Json::str() {
    return value.str();
}

me::Json::JsonValue me::Json::operator[](unsigned int index) {
    return value[index];
}

me::Json::JsonValue me::Json::operator[](std::string key) {
    return value[key];
}

void me::Json::operator=(Json json) {
    return value.operator=(json);
}

me::Json::Type me::Json::type() {
    return value.type();
}

unsigned int me::Json::size() {
    return value.size();
}

std::vector<std::string> me::Json::keys() {
    return value.keys();
}

me::Json::iterator me::Json::begin() {
    return value.begin();
}

me::Json::iterator me::Json::end() {
    return value.end();
}

void me::Json::push_back(char c) {
    return value.push_back(c);
}

void me::Json::push_back(std::string str) {
    return value.push_back(str);
}

void me::Json::push_back(Json json) {
    return value.push_back(json);
}

void me::Json::insert(unsigned int index, char c) {
    return value.insert(index, c);
}

void me::Json::insert(unsigned int index, std::string str) {
    return value.insert(index, str);
}

void me::Json::insert(unsigned int index, Json json) {
    return value.insert(index, json);
}

bool me::Json::insert(std::string key, Json json) {
    return value.insert(key, json);
}

void me::Json::erase(unsigned int index) {
    return value.erase(index);
}

void me::Json::erase(unsigned int first, unsigned int last) {
    return value.erase(first, last);
}

void me::Json::erase(std::string key) {
    return value.erase(key);
}

me::Json::~Json() {
    node->save = false;
    node->~Node();
}

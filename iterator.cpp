#include "json-dev.hpp"

me::Json::iterator::iteratorGetClass::iteratorGetClass(Node* node, std::vector<std::string>* key, unsigned int size, unsigned int index) : node(node), key(key), size(size), index(index) {}

me::Json::iterator::iteratorGetClass::operator char() {
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

me::Json::iterator::iteratorGetClass::operator Json() {
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

me::Json::iterator::iteratorGetClass::operator std::pair<std::string, Json>() {
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

me::Json::iterator::iterator(Node* node, unsigned int index) : node(node), index(index) {
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

bool me::Json::iterator::operator==(iterator& other) {
    return (node==other.node && index==other.index);
}

bool me::Json::iterator::operator!=(iterator& other) {
    return !(operator==(other));
}

void me::Json::iterator::operator++() {
    if (index >= size) {
        std::cerr << "cannot increment end iterator\n";
        exit(1);
    }
    index++;
}

void me::Json::iterator::operator--() {
    if (index == 0) {
        std::cerr << "cannot decrement begin iterator\n";
        exit(1);
    }
    index--;
}

me::Json::iterator::iteratorGetClass me::Json::iterator::operator*() {
    return {node, &key, size, index};
}

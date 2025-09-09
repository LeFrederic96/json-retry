#include "json-dev.hpp"

me::Json::Node* me::Json::copy(Node* node) {
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

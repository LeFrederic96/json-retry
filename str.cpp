#include "json.hpp"

#include <sstream>

std::string me::Json::hex(unsigned char c) {
    std::stringstream ss;
    int i1 = c/16, i2 = c%16;
    ss << hexChars[i1] << hexChars[i2];
    return ss.str();
}

std::string me::Json::prep(std::string str) {
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

std::string me::Json::str(Node* node, std::string indent) {
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

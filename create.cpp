#include "json.hpp"

me::Json::createClass::createClass(bool b) {
    node = makeNode(N_BOOL, {b});
}

me::Json::createClass::createClass(int number) {
    node = makeNode(N_NUMBER, {(double)number});
}

me::Json::createClass::createClass(double number) {
    node = makeNode(N_NUMBER, {number});
}

me::Json::createClass::createClass(std::string str) {
    node = makeNode(N_STRING, {str});
}

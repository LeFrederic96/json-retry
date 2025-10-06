#include "json.hpp"

#include <cstring>
#include <cmath>
#include <sstream>

me::Json::iterator::iterator(unsigned long id, unsigned long index) : id(id), idx(index), type(data[id].type) {
    Node& node = data[id];
    switch (node.type) {
        case N_LIST:
            size = node.value.list.size();
        break;
        case N_OBJECT:
            for (std::pair<std::string, unsigned long> n : node.value.object) {
                keys.push_back(n.first);
            }
            size = keys.size();
        break;
        default:
            std::cerr << "cannot create iterator from non object or list\n";
            exit(1);
    }
    if (index > size) {
        std::cerr << "iterator index out of range\n";
        exit(1);
    }
}

bool me::Json::iterator::operator==(iterator other) {
    return id==other.id && idx==other.idx;
}

bool me::Json::iterator::operator!=(iterator other) {
    return !operator==(other);
}

me::Json::iterator me::Json::iterator::operator++() {
    if (idx == size) {
        std::cerr << "cannot increment end iterator\n";
        exit(1);
    }
    idx++;
    return {id, idx-1};
}

me::Json::iterator me::Json::iterator::operator--() {
    if (idx == 0) {
        std::cerr << "cannot decrement begin iterator\n";
        exit(1);
    }
    idx--;
    return {id, idx+1};
}

me::Json::iterator me::Json::iterator::operator++(int) {
    if (idx == size) {
        std::cerr << "cannot increment end iterator\n";
        exit(1);
    }
    idx++;
    return {id, idx};
}

me::Json::iterator me::Json::iterator::operator--(int) {
    if (idx == 0) {
        std::cerr << "cannot decrement begin iterator\n";
        exit(1);
    }
    idx--;
    return {id, idx};
}

me::Json::iterator me::Json::iterator::operator+(unsigned long step) {
    if (idx+step > size) {
        std::cerr << "cannot increment iterator past end\n";
        exit(1);
    }
    return {id, idx+step};
}

me::Json::iterator me::Json::iterator::operator-(unsigned long step) {
    if (idx < step) {
        std::cerr << "cannot decrement iterator past 0\n";
        exit(1);
    }
    return {id, idx-step};
}

me::Json::iterator me::Json::iterator::operator+=(unsigned long step) {
    if (idx+step > size) {
        std::cerr << "cannot increment iterator past end\n";
        exit(1);
    }
    idx += step;
    return {id, idx};
}

me::Json::iterator me::Json::iterator::operator-=(unsigned long step) {
    if (idx < step) {
        std::cerr << "cannot decrement iterator past 0\n";
        exit(1);
    }
    idx -= step;
    return {id, idx};
}

me::Json::iterator::iteratorGetClass me::Json::iterator::operator*() {
    if (idx == size) {
        std::cerr << "cannot get value from end iterator\n";
        exit(1);
    }
    return {id, idx, &keys, type};
}

unsigned long me::Json::iterator::index() {
    if (type != N_LIST) {
        std::cerr << "cannot get index from non list iterator\n";
        exit(1);
    }
    return idx;
}

std::string me::Json::iterator::key() {
    if (type != N_OBJECT) {
        std::cerr << "cannot get key from non object iterator\n";
        exit(1);
    }
    return keys[idx];
}

me::Json::iterator::iteratorGetClass::iteratorGetClass(unsigned long id, unsigned long idx, std::vector<std::string>* keys, NodeType type) : id(id), idx(idx), keys(keys), type(type) {}

me::Json::iterator::iteratorGetClass::operator me::Json::JsonValue() {
    Node& node = data[id];
    switch (type) {
        case N_LIST:
            return node.value.list[idx];

        case N_OBJECT:
            return node.value.object[(*keys)[idx]];
    }
    std::cerr << "unreachable me::Json::iterator::iteratorGetClass::operator me::Json::JsonValue\n";
    exit(1);
}

me::Json::JsonValue::JsonValue() : id(-1) {}
me::Json::JsonValue::JsonValue(unsigned long id) : id(id) {}

void me::Json::JsonValue::set(unsigned long id) {
    this->id = id;
}

me::Json::JsonValue::operator me::Json() {
    return id;
}

me::Json::Type me::Json::JsonValue::type() {
    return (Type)data[id].type;
}

std::string me::Json::JsonValue::str() {
    return Json::str(id, "");
}

me::Json::JsonValue me::Json::JsonValue::operator[](unsigned long i) {
    Node& node = data[id];
    if (node.type != N_LIST) {
        std::cerr << "cannot use [] with unsigned long for non list\n";
        exit(1);
    }
    return node.value.list[i];
}

me::Json::JsonValue me::Json::JsonValue::operator[](std::string str) {
    Node& node = data[id];
    if (node.type != N_OBJECT) {
        std::cerr << "cannot use [] with string for non object\n";
        exit(1);
    }
    return node.value.object[str];
}

void me::Json::JsonValue::operator=(Json other) {
    Json::destroy(id);
    id = copy(other.value.id);
}

unsigned long me::Json::JsonValue::size() {
    Node& node = data[id];
    switch (node.type) {
        case N_LIST:
            return node.value.list.size();

        case N_OBJECT:
            return node.value.object.size();
    }
    std::cerr << "cannot get size from non list or object\n";
    exit(1);
}

std::vector<std::string> me::Json::JsonValue::keys() {
    Node& node = data[id];
    if (node.type != N_OBJECT) {
        std::cerr << "cannot get keys from non object\n";
        exit(1);
    }
    std::vector<std::string> ret;
    for (std::pair<std::string, unsigned long> n : node.value.object) {
        ret.push_back(n.first);
    }
    return ret;
}

me::Json::iterator me::Json::JsonValue::begin() {
    return {id, 0};
}

me::Json::iterator me::Json::JsonValue::end() {
    return {id, size()};
}

void me::Json::JsonValue::push_back(Json value) {
    Node& node = data[id];
    if (node.type != N_LIST) {
        std::cerr << "cannot push_back a value onto a non list\n";
        exit(1);
    }
    node.value.list.push_back(copy(value.value.id));
}

void me::Json::JsonValue::insert(unsigned long index, Json value) {
    Node& node = data[id];
    if (node.type != N_LIST) {
        std::cerr << "cannot insert a value by unsigned long into a non list\n";
        exit(1);
    }
    node.value.list.insert(node.value.list.begin()+index, copy(value.value.id));
}

bool me::Json::JsonValue::insert(std::string key, Json value) {
    Node& node = data[id];
    if (node.type != N_OBJECT) {
        std::cerr << "cannot insert a value by string into non object\n";
        exit(1);
    }
    return node.value.object.insert({key, copy(value.value.id)}).second;
}

void me::Json::JsonValue::erase(unsigned long index) {
    Node& node = data[id];
    if (node.type != N_LIST) {
        std::cerr << "cannot erase value by unsigned long from non list\n",
        exit(1);
    }
    Json::destroy(node.value.list[index]);
    node.value.list.erase(node.value.list.begin()+index);
}

void me::Json::JsonValue::erase(std::string key) {
    Node& node = data[id];
    if (node.type != N_OBJECT) {
        std::cerr << "cannot erase value by string from non object\n";
        exit(1);
    }
    Json::destroy(node.value.object[key]);
    node.value.object.erase(key);
}

void me::Json::JsonValue::erase(iterator pos) {
    Node& node = data[id];
    if (pos.id != id) {
        std::cerr << "cannot erase value by invalid iterator\n";
        exit(1);
    }
    switch (node.type) {
        case N_LIST:
            Json::destroy(node.value.list[pos.idx]);
            node.value.list.erase(node.value.list.begin()+pos.idx);
        break;
        case N_OBJECT:
            Json::destroy(node.value.object[pos.keys[pos.idx]]);
            node.value.object.erase(pos.keys[pos.idx]);
        break;
        default:
            std::cerr << "cannot erase value from non list or object\n";
            exit(1);
    }
}

bool me::Json::JsonValue::empty() {
    Node& node = data[id];
    switch (node.type) {
        case N_LIST:
            return node.value.list.empty();

        case N_OBJECT:
            return node.value.object.empty();
    }
    std::cerr << "cannot use empty on non list or object\n";
    exit(1);
}

void me::Json::JsonValue::clear() {
    Node& node = data[id];
    switch (node.type) {
        case N_LIST:
            for (unsigned long n : node.value.list) {
                Json::destroy(n);
            }
            node.value.list.clear();
        break;
        case N_OBJECT:
            for (std::pair<std::string, unsigned long> n : node.value.object) {
                Json::destroy(n.second);
            }
            node.value.object.clear();
        break;
        default:
            std::cerr << "cannot clear non list or object\n";
            exit(1);
    }
}

me::Json::iterator me::Json::JsonValue::find(std::string key) {
    Node& node = data[id];
    if (node.type != N_OBJECT) {
        std::cerr << "cannot find key in non object\n";
        exit(1);
    }
    iterator it = begin();
    while (it != end() && it.key() != key) {
        it++;
    }
    return it;
}

me::Json::iterator me::Json::JsonValue::find(const char* key) {
    return find((std::string)key);
}

bool me::Json::JsonValue::operator==(Json other) {
    return cmp(id, other.value.id);
}

bool me::Json::JsonValue::operator==(JsonValue other) {
    return cmp(id, other.id);
}

bool me::Json::JsonValue::operator!=(Json other) {
    return !operator==(other);
}

bool me::Json::JsonValue::operator!=(JsonValue other) {
    return !operator!=(other);
}

void me::Json::JsonValue::destroy() {
    Json::destroy(id);
}

me::Json::Json(std::string str) {
    if (str.size() == 0) {
        std::cerr << "cannot create json object from empty string\n";
        exit(1);
    }
    tokenize(str);
    value.set(parse());
    if (i != tokens.size()) {
        std::cerr << "cannot create json object, junk found at end of string\n";
        exit(1);
    }
    tokens.clear();
}

me::Json::Json(Json& other) {
    value.set(copy(other.value.id));
}

void me::Json::operator=(Json other) {
    value.operator=(other);
}

me::Json::Type me::Json::type() {
    return value.type();
}

std::string me::Json::str() {
    return value.str();
}

me::Json::JsonValue me::Json::operator[](unsigned long i) {
    return value.operator[](i);
}

me::Json::JsonValue me::Json::operator[](std::string str) {
    return value.operator[](str);
}

unsigned long me::Json::size() {
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

void me::Json::push_back(Json value) {
    this->value.push_back(value);
}

void me::Json::insert(unsigned long index, Json value) {
    this->value.insert(index, value);
}

bool me::Json::insert(std::string key, Json value) {
    return this->value.insert(key, value);
}

void me::Json::erase(unsigned long index) {
    value.erase(index);
}

void me::Json::erase(std::string key) {
    value.erase(key);
}

void me::Json::erase(iterator pos) {
    value.erase(pos);
}

bool me::Json::empty() {
    return value.empty();
}

void me::Json::clear() {
    value.clear();
}

me::Json::iterator me::Json::find(std::string key) {
    return value.find(key);
}

bool me::Json::operator==(Json other) {
    return value.operator==(other);
}

bool me::Json::operator==(JsonValue other) {
    return value.operator==(other);
}

bool me::Json::operator!=(Json other) {
    return value.operator!=(other);
}

bool me::Json::operator!=(JsonValue other) {
    return value.operator!=(other);
}

me::Json::~Json() {
    value.destroy();
}

me::Json::Token::Token(TokenType type) : type(type) {}
me::Json::Token::Token(bool b) : type(T_BOOL), value({.b=b}) {}
me::Json::Token::Token(double number) : type(T_NUMBER), value({.number=number}) {}
me::Json::Token::Token(std::string str) : type(T_STRING), value({.str=str}) {}

me::Json::Node::Node() : type((NodeType)-1) {}
me::Json::Node::Node(bool b) : type(N_BOOL), value({.b=b}) {}
me::Json::Node::Node(double number) : type(N_NUMBER), value({.number=number}) {}
me::Json::Node::Node(std::string str) : type(N_STRING), value({.str=str}) {}
me::Json::Node::Node(std::vector<unsigned long> list) : type(N_LIST), value({.list=list}) {}
me::Json::Node::Node(std::map<std::string, unsigned long> object) : type(N_OBJECT), value({.object=object}) {}

me::Json::getClass::getClass(unsigned long id) : id(id) {}

me::Json::getClass::operator me::Json() {
    return id;
}

me::Json::getClass::operator bool() {
    Node& node = data[id];
    if (node.type != N_BOOL) {
        std::cerr << "cannot get bool from non bool\n";
        exit(1);
    }
    return node.value.b;
}

me::Json::getClass::operator double() {
    Node& node = data[id];
    if (node.type != N_NUMBER) {
        std::cerr << "cannot get double from non number\n";
        exit(1);
    }
    return node.value.number;
}

me::Json::getClass::operator std::string() {
    Node& node = data[id];
    if (node.type != N_STRING) {
        std::cerr << "cannot get string from non string\n";
        exit(1);
    }
    return node.value.str;
}

me::Json::getClass::operator char() {
    return operator double();
}

me::Json::getClass::operator short() {
    return operator double();
}

me::Json::getClass::operator int() {
    return operator double();
}

me::Json::getClass::operator long() {
    return operator double();
}

me::Json::getClass::operator unsigned char() {
    return operator double();
}

me::Json::getClass::operator unsigned short() {
    return operator double();
}

me::Json::getClass::operator unsigned int() {
    return operator double();
}

me::Json::getClass::operator unsigned long() {
    return operator double();
}

me::Json::getClass::operator float() {
    return operator double();
}

me::Json::createClass::createClass(bool b) {
    data.insert({index, b});
    id = index++;
}

me::Json::createClass::createClass(double number) {
    data.insert({index, number});
    id = index++;
}

me::Json::createClass::createClass(std::string str) {
    data.insert({index, str});
    id = index++;
}


me::Json::createClass::createClass(char number) {
    data.insert({index, (double)number});
    id = index++;
}

me::Json::createClass::createClass(short number) {
    data.insert({index, (double)number});
    id = index++;
}

me::Json::createClass::createClass(int number) {
    data.insert({index, (double)number});
    id = index++;
}

me::Json::createClass::createClass(long number) {
    data.insert({index, (double)number});
    id = index++;
}

me::Json::createClass::createClass(unsigned char number) {
    data.insert({index, (double)number});
    id = index++;
}

me::Json::createClass::createClass(unsigned short number) {
    data.insert({index, (double)number});
    id = index++;
}

me::Json::createClass::createClass(unsigned int number) {
    data.insert({index, (double)number});
    id = index++;
}

me::Json::createClass::createClass(unsigned long number) {
    data.insert({index, (double)number});
    id = index++;
}

char me::Json::hexChars[23] = "0123456789abcdefABCDEF";
std::map<unsigned long, me::Json::Node> me::Json::data;
unsigned long me::Json::index = 0;

me::Json::Json(unsigned long id) {
    value.set(copy(id));
}

char me::Json::get_char(std::string str, int base) {
    char ret = 0;
    for (char c : str) {
        ret *= base;
        if (isdigit(c)) {
            if (c-'0' >= base) {
                std::cerr << "invalid escape sequence: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'0';
        } else if (isupper(c)) {
            if (c-'A'+10 >= base) {
                std::cerr << "invalid escape sequence: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'A'+10;
        } else if (islower(c)) {
            if (c-'a'+10 >= base) {
                std::cerr << "invalid escape sequence: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'a'+10;
        } else {
            std::cerr << "invalid escape sequence: not a number '" << c << "'\n";
            exit(1);
        }
    }
    return ret;
}

double me::Json::get_number(std::string str, int base) {
    double ret = 0;
    unsigned long i = 0;
    for (; i < str.size() && str[i] != '.'; i++) {
        char c = str[i];
        ret *= base;
        if (isdigit(c)) {
            if (c-'0' >= base) {
                std::cerr << "invalid number: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'0';
        } else if (isupper(c)) {
            if (c-'A'+10 >= base) {
                std::cerr << "invalid number: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'A'+10;
        } else if (islower(c)) {
            if (c-'a'+10 >= base) {
                std::cerr << "invalid number: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'a'+10;
        } else {
            std::cerr << "invalid number: not a number '" << c << "'\n";
            exit(1);
        }
    }
    unsigned long j = 1;
    if (i < str.size()) {
        if (base != 10) {
            std::cerr << "wrong base for decimal number " << base << "\n";
            exit(1);
        }
        i++;
        for (; i < str.size(); i++) {
            if (isdigit(str[i])) {
                if (str[i]-'0' >= 10) {
                    std::cerr << "invalid decimal digit: number exceeds 9 " << str[i] << " \n";
                    exit(1);
                }
                ret += ((double)(str[i]-'0'))/pow(10, j++);
            } else {
                std::cerr << "invalid decimal digit " << str[i] << "\n";
                exit(1);
            }
        }
    }
    return ret;
}

unsigned long me::Json::tokenize(std::string str) {
    std::string buf;
    unsigned long i = 0;
    while (i < str.size()) {
        switch (str[i]) {
            case '{':
                i++;
                tokens.push_back({T_COPEN});
            break;
            case '}':
                i++;
                tokens.push_back({T_CCLOSE});
            break;
            case '[':
                i++;
                tokens.push_back({T_BOPEN});
            break;
            case ']':
                i++;
                tokens.push_back({T_BCLOSE});
            break;
            case ',':
                i++;
                tokens.push_back({T_COMMA});
            break;
            case ':':
                i++;
                tokens.push_back({T_COLON});
            break;
            case '"':
                i++;
                while (str[i] != '"') {
                    if (str[i] == '\\') {
                        i++;
                        switch (str[i]) {
                            case 'a':
                                i++;
                                buf.push_back('\a');
                            break;
                            case 'b':
                                i++;
                                buf.push_back('\b');
                            break;
                            case 'e':
                                i++;
                                buf.push_back('\e');
                            break;
                            case 'f':
                                i++;
                                buf.push_back('\f');
                            break;
                            case 'n':
                                i++;
                                buf.push_back('\n');
                            break;
                            case 'r':
                                i++;
                                buf.push_back('\r');
                            break;
                            case 't':
                                i++;
                                buf.push_back('\t');
                            break;
                            case 'v':
                                i++;
                                buf.push_back('\v');
                            break;
                            case 'x':
                                i++;
                                buf.push_back(get_char(str.substr(i, 2), 16));
                                i += 2;
                            break;
                            case '"':
                                i++;
                                buf.push_back('"');
                            break;
                            default:
                                if (isdigit(str[i]) && str[i] <= '8') {
                                    std::string b;
                                    b.push_back(str[i++]);
                                    while (isdigit(str[i]) && str[i] <= '8' && b.size() < 3) {
                                        b.push_back(str[i++]);
                                    }
                                    buf.push_back(get_char(b, 8));
                                    break;
                                }
                                std::cerr << "invalid escape sequence '" << str[i] << "'\n";
                                exit(1);
                        }
                    } else {
                        if (!isprint(str[i])) {
                            std::cerr << "expected '\"' at end of string\n";
                            exit(1);
                        }
                        buf.push_back(str[i]);
                        i++;
                    }
                }
                i++;
                tokens.push_back(buf);
                buf.clear();
            break;
            default:
                if (isspace(str[i])) {
                    while (isspace(str[i])) {
                        i++;
                    }
                } else if (isdigit(str[i])) {
                    std::string b = "0";
                    int base = 10;
                    if (str[i] == '0') {
                        i++;
                        switch (str[i]) {
                            case 'b':
                                i++;
                                base = 2;
                            break;
                            case 'o':
                                i++;
                                base = 8;
                            break;
                            case 'x':
                                i++;
                                base = 16;
                            break;
                            default:
                                base = 10;
                        }
                    }
                    if (base == 16) {
                        while (strchr(hexChars, str[i]) && str[i] != '\0') {
                            b.push_back(str[i++]);
                        }
                    } else {
                        while (isdigit(str[i]) && str[i] <= base+'0') {
                            b.push_back(str[i++]);
                        }
                        if (base == 10 && str[i] == '.') {
                            i++;
                            b.push_back('.');
                            while (isdigit(str[i])) {
                                b.push_back(str[i++]);
                            }
                        }
                    }
                    tokens.push_back(get_number(b, base));
                } else if (i+3 < str.size() && str.substr(i, 4) == "true") {
                    i += 4;
                    tokens.push_back(true);
                } else if (i+4 < str.size() && str.substr(i, 5) == "false") {
                    i += 5;
                    tokens.push_back(false);
                } else {
                    std::cerr << "invalid JSON character '" << str[i] << "'\n";
                    exit(1);
                }
        }
    }
    return i;
}

me::Json::TokenValue me::Json::match(TokenType type, std::string str) {
    if (tokens[i].type != type) {
        std::cerr << "expected " << str << "\n";
        exit(1);
    }
    return tokens[i++].value;
}

unsigned long me::Json::parse() {
    switch (tokens[i].type) {
        case T_BOOL:
            data.insert({index, tokens[i].value.b});
            i++;
            return index++;

        case T_NUMBER:
            data.insert({index, tokens[i].value.number});
            i++;
            return index++;

        case T_STRING:
            data.insert({index, tokens[i].value.str});
            i++;
            return index++;

        case T_BOPEN: {
            i++;
            std::vector<unsigned long> list;
            if (tokens[i].type != T_BCLOSE) {
                list.push_back(parse());
                while (tokens[i].type == T_COMMA) {
                    i++;
                    list.push_back(parse());
                }
            }
            match(T_BCLOSE, "]");
            data.insert({index, list});
            return index++;
        }
        case T_COPEN: {
            i++;
            std::map<std::string, unsigned long> object;
            std::string key;
            if (tokens[i].type != T_CCLOSE) {
                key = match(T_STRING, "string").str;
                match(T_COLON, ":");
                object.insert({key, parse()});
                while (tokens[i].type == T_COMMA) {
                    i++;
                    key = match(T_STRING, "string").str;
                    match(T_COLON, ":");
                    if (!object.insert({key, parse()}).second) {
                        std::cerr << "Duplicate key \"" << key << "\" in object\n";
                        exit(1);
                    }
                }
            }
            match(T_CCLOSE, "}");
            data.insert({index, object});
            return index++;
        }
    }
    std::cerr << "unexpected token\n";
    exit(1);
}

std::string me::Json::hex(char c) {
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

std::string me::Json::str(unsigned long id, std::string indent) {
    std::stringstream ss;
    Node& node = data[id];
    switch (node.type) {
        case N_BOOL:
            if (node.value.b) {
                return "true";
            }
            return "false";

        case N_NUMBER:
            ss << node.value.number;
            return ss.str();

        case N_STRING:
            return prep(node.value.str);

        case N_LIST:
            ss << "[";
            for (unsigned long i = 0; i < node.value.list.size(); i++) {
                unsigned long n = node.value.list[i];
                ss << "\n" << indent << INDENT << str(n, indent+INDENT);
                if (i+1 < node.value.list.size()) {
                    ss << ",";
                } else {
                    ss << "\n" << indent;
                }
            }
            ss << "]";
            return ss.str();

        case N_OBJECT:
            ss << "{";
            for (std::map<std::string, unsigned long>::iterator it = node.value.object.begin(); it != node.value.object.end();) {
                ss << "\n" << indent << INDENT << prep(it->first) << ": " << str(it->second, indent+INDENT);
                if (++it != node.value.object.end()) {
                    ss << ",";
                } else {
                    ss << "\n" << indent;
                }
            }
            ss << "}";
            return ss.str();
    }
    std::cerr << "unreachable in me::Json::str\n";
    exit(1);
}

unsigned long me::Json::copy(unsigned long id) {
    Node& node = data[id];
    switch (node.type) {
        case N_BOOL:
            data.insert({index, node.value.b});
            return index++;

        case N_NUMBER:
            data.insert({index, node.value.number});
            return index++;

        case N_STRING:
            data.insert({index, node.value.str});
            return index++;

        case N_LIST: {
            std::vector<unsigned long> list;
            for (unsigned long n : node.value.list) {
                list.push_back(copy(n));
            }
            data.insert({index, list});
            return index++;
        }

        case N_OBJECT: {
            std::map<std::string, unsigned long> object;
            for (std::pair<std::string, unsigned long> n : node.value.object) {
                object.insert({n.first, copy(n.second)});
            }
            data.insert({index, object});
            return index++;
        }
    }
    std::cerr << "unreachable: me::Json::copy\n";
    exit(1);
}

void me::Json::destroy(unsigned long id) {
    Node& node = data[id];
    switch (node.type) {
        case N_BOOL:
        case N_NUMBER:
        case N_STRING:
            data.erase(id);
        break;
        case N_LIST:
            for (unsigned long n : node.value.list) {
                destroy(n);
                data.erase(n);
            }
        break;
        case N_OBJECT:
            for (std::pair<std::string, unsigned long> n : node.value.object) {
                destroy(n.second);
                data.erase(n.second);
            }
        break;
    }
}

bool me::Json::cmp(unsigned long id1, unsigned long id2) {
    Node& node1 = data[id1];
    Node& node2 = data[id2];
    if (node1.type != node2.type) {
        return false;
    }
    switch (node1.type) {
        case N_BOOL:
            return node1.value.b==node2.value.b;

        case N_NUMBER:
            return node1.value.number==node2.value.number;

        case N_STRING:
            return node1.value.str==node2.value.str;

        case N_LIST:
            if (node1.value.list.size() != node2.value.list.size()) {
                return false;
            }
            for (std::vector<unsigned long>::iterator n1 = node1.value.list.begin(), n2 = node2.value.list.begin(); n1 != node1.value.list.end(); n1++, n2++) {
                if (!cmp(*n1.base(), *n2.base())) {
                    return false;
                }
            }
            return true;

        case N_OBJECT:
            if (node1.value.object.size() != node2.value.object.size()) {
                return false;
            }
            for (std::map<std::string, unsigned long>::iterator n1 = node1.value.object.begin(), n2 = node2.value.object.begin(); n1 != node1.value.object.end(); n1++, n2++) {
                if (n1->first != n2->first) {
                    return false;
                }
                if (!cmp(n1->second, n2->second)) {
                    return false;
                }
            }
            return true;
    }
    std::cerr << "unreachable: me::Json::cmp\n";
    exit(1);
}

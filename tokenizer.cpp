#include "json-dev.hpp"

#include <cmath>
#include <cstring>

char me::Json::hexChars[23] = "0123456789abcdefABCDEF";

me::Json::TokenValue::TokenValue() {}

me::Json::TokenValue::TokenValue(bool b) : b(b) {}

me::Json::TokenValue::TokenValue(double number) : number(number) {}

me::Json::TokenValue::TokenValue(std::string& str) : str(str) {}

char me::Json::get_char(std::string str, int base) {
    char ret = 0;
    for (char c : str) {
        ret *= base;
        if (isdigit(c)) {
            if (c-'0' >= base) {
                std::cerr << "Invalid escape sequence: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'0';
        } else if (isupper(c)) {
            if (c-'A'+10 >= base) {
                std::cerr << "Invalid escape sequence: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'A'+10;
        } else if (islower(c)) {
            if (c-'a'+10 >= base) {
                std::cerr << "Invalid escape sequence: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'a'+10;
        } else {
            std::cerr << "Invalid escape sequence: not a number '" << c << "'\n";
            exit(1);
        }
    }
    return ret;
}

double me::Json::get_number(std::string& str, int base) {
    double ret = 0;
    unsigned int i = 0;
    for (; i < str.size() && str[i] != '.'; i++) {
        char c = str[i];
        ret *= base;
        if (isdigit(c)) {
            if (c-'0' >= base) {
                std::cerr << "Invalid number: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'0';
        } else if (isupper(c)) {
            if (c-'A'+10 >= base) {
                std::cerr << "Invalid number: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'A'+10;
        } else if (islower(c)) {
            if (c-'a'+10 >= base) {
                std::cerr << "Invalid number: number exceed base " << c << " \n";
                exit(1);
            }
            ret += c-'a'+10;
        } else {
            std::cerr << "Invalid number: not a number '" << c << "'\n";
            exit(1);
        }
    }
    unsigned int j = 1;
    if (i < str.size()) {
        if (base != 10) {
            std::cerr << "Wrong base for decimal number " << base << "\n";
            exit(1);
        }
        i++;
        for (; i < str.size(); i++) {
            if (isdigit(str[i])) {
                if (str[i]-'0' >= 10) {
                    std::cerr << "Invalid decimal digit: number exceeds 9 " << str[i] << " \n";
                    exit(1);
                }
                ret += ((double)(str[i]-'0'))/pow(10, j++);
            } else {
                std::cerr << "Invalid decimal digit " << str[i] << "\n";
                exit(1);
            }
        }
    }
    return ret;
}

void me::Json::tokenize(std::string& str, std::vector<Token>& tokens) {
    std::string buf;
    unsigned int i = 0;
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
                                std::cerr << "Invalid escape sequence '" << str[i] << "'\n";
                                exit(1);
                        }
                    } else {
                        buf.push_back(str[i]);
                        i++;
                    }
                }
                i++;
                tokens.push_back({T_STRING, {buf}});
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
                    tokens.push_back({T_NUMBER, {get_number(b, base)}});
                } else if (i+3 < str.size() && str.substr(i, 4) == "true") {
                    i += 4;
                    tokens.push_back({T_BOOL, {true}});
                } else if (i+4 < str.size() && str.substr(i, 5) == "false") {
                    i += 5;
                    tokens.push_back({T_BOOL, {false}});
                } else {
                    std::cerr << "Invalid JSON character '" << str[i] << "'\n";
                    exit(1);
                }
        }
    }
}

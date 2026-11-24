#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <vector>

std::string float_to_str(float val) {
    if (val == 0.0f) return "0.0";
    
    std::string res = "";
    if (val < 0) {
        res += "-";
        val = -val;
    }
    
    int int_part = (int)val;
    if (int_part == 0) res += "0";
    else {
        std::string int_str = "";
        while (int_part > 0) {
            int_str = (char)('0' + (int_part % 10)) + int_str;
            int_part /= 10;
        }
        res += int_str;
    }
    
    res += ".";
    float frac = val - (float)(int)val;
    for (int i = 0; i < 4; ++i) {
        frac *= 10.0f;
        int digit = (int)frac;
        res += (char)('0' + digit);
        frac -= digit;
    }
    while (res.back() == '0' && res.back() != '.') {
        res.pop_back();
    }
    if (res.back() == '.') res.pop_back();
    
    return res;
}

int main() {
    char buffer[4096];
    std::string current_line = "";
    ssize_t bytes_read;

    while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytes_read; ++i) {
            if (buffer[i] == '\n') {

                std::vector<float> numbers;
                std::string current_token = "";

                for (char ch : current_line) {
                    if (ch == ' ' || ch == '\t' || ch == '\r') {
                        if (!current_token.empty()) {
                          
                            char* endptr;
                            float val = strtof(current_token.c_str(), &endptr);
                            if (endptr != current_token.c_str()) {
                                numbers.push_back(val);
                            }
                            current_token = "";
                        }
                    } else {
                        current_token += ch;
                    }
                }

                if (!current_token.empty()) {
                    char* endptr;
                    float val = strtof(current_token.c_str(), &endptr);
                    if (endptr != current_token.c_str()) {
                        numbers.push_back(val);
                    }
                }

                if (numbers.size() >= 2) {
                    float result = numbers[0];
                    bool div_by_zero = false;

                    for (size_t i = 1; i < numbers.size(); ++i) {
                        if (numbers[i] > -1e-6f && numbers[i] < 1e-6f) {
                            div_by_zero = true;
                            break;
                        }
                        result /= numbers[i];
                    }

                    if (div_by_zero) {
                        const char* err = "Error: Division by zero detected in child!\n";
                        write(STDERR_FILENO, err, 44);
                        exit(1);
                    }

                    std::string out_str = float_to_str(result) + "\n";
                    write(STDOUT_FILENO, out_str.c_str(), out_str.length());
                }

                current_line = "";
            } else {
                current_line += buffer[i];
            }
        }
    }

    return EXIT_SUCCESS;
}

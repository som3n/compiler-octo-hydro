#include <bits/stdc++.h>

using namespace std;

enum class TokenType {
    _return,
    int_lit,
    semi
};

struct Token {
    TokenType type;
    optional<string> value {};
};

vector<Token> tokenize(const string & str) {
    vector<Token> tokens;
    string buff;
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str.at(i);
        if (isalpha(c)) {
            buff.clear(); 
            do {
                buff.push_back(c);
                c = str.at(++i);
            } while (i < str.length() && isalpha(c));
            i--; 

            if (buff == "exit") {
                tokens.push_back({TokenType::_return});
            } else {
                cerr << "Unknown keyword: " << buff << endl;
                exit(EXIT_FAILURE);
            }
        } 
        else if (isdigit(c)) {
            buff.clear();
            do {
                buff.push_back(c);
                c = str.at(++i);
            } while (i < str.length() && isdigit(c));
            i--;

            tokens.push_back({TokenType::int_lit, buff});
        } 
        else if (c == ';') {
            tokens.push_back({TokenType::semi});
        }
        else if (isspace(c)) {
            continue;
        } 
        else {
            cerr << "Unexpected character: " << c << endl;
            exit(EXIT_FAILURE);
        }
    }

    return tokens;
}

string tokens_to_asm(const vector<Token> &tokens) {
    stringstream output;
    output << "global _start\n";
    output << "_start:\n";

    for (size_t i = 0; i < tokens.size(); i++) {
        const Token& token = tokens.at(i);
        
        if (token.type == TokenType::_return) {
            if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::int_lit) {
                const Token& intToken = tokens.at(i + 1);
                
                if (i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semi) {
                    output << "    mov rax, 60\n";
                    output << "    mov rdi, " << intToken.value.value() << "\n";
                    output << "    syscall\n";
                    i += 2;
                } else {
                    cerr << "Syntax Error: Missing semicolon after return statement." << endl;
                    exit(EXIT_FAILURE);
                }
            } else {
                cerr << "Syntax Error: 'return' must be followed by an integer literal." << endl;
                exit(EXIT_FAILURE);
            }
        } else if (token.type != TokenType::semi) {
            cerr << "Syntax Error: Unexpected token." << endl;
            exit(EXIT_FAILURE);
        }
    }
    return output.str();
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Incorrect Usage" << endl;
        cerr << "Usage: hydrogen <input.hy>" << endl;
        return EXIT_FAILURE;
    }

    string contents;
    {
        fstream input(argv[1], ios::in);
        if (!input) {
            cerr << "Error opening file: " << argv[1] << endl;
            return EXIT_FAILURE;
        }
        
        stringstream contents_stream;
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }
    
    vector<Token> tokens = tokenize(contents);
    {
        fstream file("./out.asm", ios::out);
        if (!file) {
            cerr << "Error creating output file: out.asm" << endl;
            return EXIT_FAILURE;
        }
        file << tokens_to_asm(tokens);
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}

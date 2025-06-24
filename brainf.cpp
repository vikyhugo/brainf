#define mem_size 30000
#define log false

#include <cstdint>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <conio.h>
using namespace std;

int main(int argc, char *argv[]) {
    if (argc == 1) {
        cout << "An interpreter for brainf**k" << "\n";
        cout << "Usage:" << "\n";
        cout << "BRAINF <file>" << "\n";
        cout << "\n";
        cout << "<file> - A file with a .brainf extension" << "\n";
        return 0;
    } else if (!filesystem::exists(argv[1])) {
        cerr << "Specified source file doesn't exist";
        return 1;
    } else if (filesystem::is_directory(argv[1])) {
        cerr << "Specified source file is a directory";
        return 2;
    }
    
    ifstream in(argv[1]);
    ofstream logfile;
    if (log) {
        logfile.open("brainf.log");
    }
    uint8_t memory[mem_size];
    uint16_t mem_ptr = 0;

    while (!in.eof()) {
        char instruction = (char)in.get();
        switch (instruction) {
            case '+':
                memory[mem_ptr] = memory[mem_ptr] + uint8_t{1};
                if (log) {
                    logfile << "added 1 to memory address " << mem_ptr << " new value " << memory[mem_ptr] << "\n";
                }
                break;

            case '-':
                memory[mem_ptr] = memory[mem_ptr] - uint8_t{1};
                if (log) {
                    logfile << "subtracted 1 from memory address " << mem_ptr << " new value " << memory[mem_ptr] << "\n";
                }
                break;

            case '<':
                if (mem_ptr == 0) {
                    mem_ptr = mem_size - 1;
                } else {
                    mem_ptr--;
                }
                if (log) {
                    logfile << "moved memory pointer back new value " << mem_ptr << "\n";
                }
                break;

            case '>':
                mem_ptr = (mem_ptr + 1) % mem_size;
                if (log) {
                    logfile << "moved memory pointer forward new value " << mem_ptr << "\n";
                }
                break;

            case '.':
                cout << (char)memory[mem_ptr];
                if (log) {
                    logfile << "outputed data at memory address " << mem_ptr << " data " << memory[mem_ptr] << "\n";
                }
                break;

            case ',':
                memory[mem_ptr] = (uint8_t)_getch();
                if (log) {
                    logfile << "inputed data to memory address " << mem_ptr << " data " << memory[mem_ptr] << "\n";
                }
                break;

            case '[':
                if (memory[mem_ptr] == 0) {
                    uint16_t nested = 0;
                    while (true) {
                        instruction = in.get();
                        if (instruction == ']') {
                            if (nested == 0) {
                                break;
                            } else {
                                nested--;
                            }
                        } else if (instruction == '[') {
                            nested++;
                        }
                    }
                    if (log) {
                        logfile << "skipped loop" << "\n";
                    }
                } else {
                    if (log) {
                        logfile << "executed loop" << "\n";
                    }
                }
                break;

            case ']': {
                uint16_t nested = 0;
                while (true) {
                    in.unget();in.unget();
                    instruction = in.get();
                    if (instruction == '[') {
                        if (nested == 0) {
                            break;
                        } else {
                            nested--;
                        }
                    } else if (instruction == ']') {
                            nested++;
                    }
                }
                in.unget();
                if (log) {
                    logfile << "returned back to loop" << "\n";
                }
                break;
            }

            default:
                break;
        }
    }
    return 0;
}
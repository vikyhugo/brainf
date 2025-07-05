#define MEM_SIZE 30000
#define MEM_TYPE uint8_t
#define MEM_PTR_TYPE uint16_t
#define DEBUG false

#define STRINGIFY(x) #x
#define GET_TYPE_NAME(x) STRINGIFY(x)

#include <cstdint>
#include <cstring>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <conio.h>
using namespace std;

ofstream log_file;

void log(string x) {
    if (DEBUG) {
        log_file << x;
    }
}

int main(int argc, char *argv[]) {
    bool compilerMode = false;
    if (argc == 1) {
        cout << "An interpreter for brainfuck" << "\n";
        cout << "Usage:" << "\n";
        cout << "BRAINF <file> [-c <destination>]" << "\n";
        cout << "\n";
        cout << "<file> - A file with a .brainf extension" << "\n";
        cout << "-c <destination> - Compiles <file> to C++ and outputs it to <destinaton>" << "\n";
        return 0;
    } else if (!filesystem::exists(argv[1])) {
        cerr << "Specified source file doesn't exist\n";
        return 1;
    } else if (filesystem::is_directory(argv[1])) {
        cerr << "Specified source file is a directory\n";
        return 2;
    } else if ((argc >= 4) and (strcmp(argv[2], "-c") == 0)) {
        if (filesystem::is_directory(argv[3])) {
            cerr << "Specified destination file is a directory\n";
            return 3;
        }
        compilerMode = true;
    }
    
    ifstream in(argv[1]);
    ofstream out;
    uint16_t nextLoopId = 0;
    uint16_t loopIds[__UINT16_MAX__ + 1];
    uint16_t loopIdPtr = 0;
    bool loopLimitReached = false;

    if (compilerMode) {
        out.open(argv[3]);
        out << "#define MEM_SIZE " + to_string(MEM_SIZE) + "\n";
        out << "#define MEM_TYPE " + string(GET_TYPE_NAME(MEM_TYPE)) + "\n";
        out << "#define MEM_PTR_TYPE " + string(GET_TYPE_NAME(MEM_PTR_TYPE)) + "\n";
        out << "#define DEBUG " + to_string(DEBUG) + "\n";
        out << "#include <cstdint>\n";
        out << "#include <iostream>\n";
        out << "#include <fstream>\n";
        out << "#include <conio.h>\n";
        out << "using namespace std;\n";
        out << "ofstream log_file;\n";
        out << "void log(string x) {\n";
        out << "    if (DEBUG) {\n";
        out << "        log_file << x;\n";
        out << "    }\n";
        out << "}\n";
        out << "int main(int argc, char *argv[]) {\n";
        out << "if (DEBUG) {\n";
        out << "    log_file.open(\"brainf.log\");\n";
        out << "}\n";
        out << "MEM_TYPE memory[MEM_SIZE];\n";
        out << "MEM_PTR_TYPE memPtr = 0;\n";
    }
    if (DEBUG) {
        log_file.open("brainf.log");
    }
    
    MEM_TYPE memory[MEM_SIZE];
    MEM_PTR_TYPE memPtr = 0;

    while (!in.eof()) {
        char instruction = (char)in.get();
        switch (instruction) {
            case '+':
                if (compilerMode) {
                    out << "memory[memPtr] = memory[memPtr] + MEM_TYPE{1};\n";
                    out << "log(\"added 1 to memory address \" + to_string(memPtr) + \" new value \" + to_string(memory[memPtr]) + \"\\n\");\n";
                    break;
                }
                memory[memPtr] = memory[memPtr] + MEM_TYPE{1};
                log("added 1 to memory address " + to_string(memPtr) + " new value " + to_string(memory[memPtr]) + "\n");
                break;

            case '-':
                if (compilerMode) {
                    out << "memory[memPtr] = memory[memPtr] - MEM_TYPE{1};\n";
                    out << "log(\"subtracted 1 from memory address \" + to_string(memPtr) + \" new value \" + to_string(memory[memPtr]) + \"\\n\");\n";
                    break;
                }
                memory[memPtr] = memory[memPtr] - MEM_TYPE{1};
                log("subtracted 1 from memory address " + to_string(memPtr) + " new value " + to_string(memory[memPtr]) + "\n");
                break;

            case '<':
                if (compilerMode) {
                    out << "if (memPtr == 0) {\n";
                    out << "    memPtr = MEM_SIZE - 1;\n";
                    out << "} else {\n";
                    out << "    memPtr--;\n";
                    out << "}\n";
                    out << "log(\"moved memory pointer back new value \" + to_string(memPtr) + \"\\n\");\n";
                    break;
                }
                if (memPtr == 0) {
                    memPtr = MEM_SIZE - 1;
                } else {
                    memPtr--;
                }
                log("moved memory pointer back new value " + to_string(memPtr) + "\n");
                break;

            case '>':
                if (compilerMode) {
                    out << "memPtr = (memPtr + 1) % MEM_SIZE;\n";
                    out << "log(\"moved memory pointer forward new value \" + to_string(memPtr) + \"\\n\");\n";
                    break;
                }
                memPtr = (memPtr + 1) % MEM_SIZE;
                log("moved memory pointer forward new value " + to_string(memPtr) + "\n");
                break;

            case '.':
                if (compilerMode) {
                    out << "cout << (char)memory[memPtr];\n";
                    out << "log(\"outputed data at memory address \" + to_string(memPtr) + \" data \" + to_string(memory[memPtr]) + \"\\n\");\n";
                    break;
                }
                cout << (char)memory[memPtr];
                log("outputed data at memory address " + to_string(memPtr) + " data " + to_string(memory[memPtr]) + "\n");
                break;

            case ',':
                if (compilerMode) {
                    out << "memory[memPtr] = (MEM_TYPE)_getch();\n";
                    out << "log(\"inputed data to memory address \" + to_string(memPtr) + \" data \" + to_string(memory[memPtr]) + \"\\n\");\n";
                    break;
                }
                memory[memPtr] = (MEM_TYPE)_getch();
                log("inputed data to memory address " + to_string(memPtr) + " data " + to_string(memory[memPtr]) + "\n");
                break;

            case '[':
                if (compilerMode) {
                    if (loopLimitReached) {
                        cerr << "Too many loops\n";
                        cerr << "When compiling, there can be up to " + to_string(UINT16_MAX + 1) + "loops.\n";
                        return 6;
                    }
                    uint16_t x = nextLoopId;
                    nextLoopId++;
                    if (nextLoopId == 0) {
                        loopLimitReached = true;
                    }
                    out << "LoopStart" + to_string(x) + ":\n";
                    loopIds[loopIdPtr] = x;
                    loopIdPtr++;
                    out << "if (memory[memPtr] == 0){\n";
                    out << "    log(\"skipped loop\\n\");\n";
                    out << "    goto LoopEnd" + to_string(x) + ";\n";
                    out << "} else {\n";
                    out << "    log(\"executed loop\\n\");\n";
                    out << "}\n";
                    break;
                }
                if (memory[memPtr] == 0) {
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
                        } else if (instruction == EOF) {
                            cerr << "Missing loop end\n";
                            return 4;
                        }
                    }
                    log("skipped loop\n");
                } else {
                    log("executed loop\n");
                }
                break;

            case ']': {
                if (compilerMode) {
                    if (loopIdPtr == 0) {
                        cerr << "Missing loop start\n";
                        return 7;
                    }
                    loopIdPtr--;
                    uint16_t x = loopIds[loopIdPtr];
                    out << "log(\"returned back to loop\\n\");\n";
                    out << "goto LoopStart" + to_string(x) + ";\n";
                    out << "LoopEnd" + to_string(x) + ":\n";
                    break;
                }
                uint16_t nested = 0;
                while (true) {
                    in.unget();
                    if (in.fail()) {
                        cerr << "Missing loop start\n";
                        return 5;
                    }
                    in.unget();
                    if (in.fail()) {
                        cerr << "Missing loop start\n";
                        return 5;
                    }
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
                log("returned back to loop\n");
                break;
            }

            default:
                break;
        }
    }
    if (compilerMode) {
        out << "log(\"program end\\n\");\n";
        out << "}\n";
        return 0;
    }
    log("program end\n");
    return 0;
}
# Brainf
A simple Brainfuck interpreter written in C++.

More information about Brainfuck can be found [here](http://wikipedia.org/wiki/Brainfuck).

## Usage
`BRAINF <file> -c <destination>`

`<file>` - The Brainfuck source file to be executed<br>
`-c <destination>` - Compiles `<file>` to C++ and outputs it to `<destination>`

## Examples
`BRAINF example.brainf`<br>
Takes in 5 characters and then outputs them.

`BRAINF example.brainf -c example.cpp`<br>
Creates an example.cpp file, which when compiled and run, takes in 5 characters and then outputs them.

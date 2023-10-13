import os
import sys

from codegen_message import *
from parser import *

def main():
    if len(sys.argv) != 2:
        sys.exit("Takes name of file.")

    filename = sys.argv[1]

    with open(filename, mode='r') as f:
        filedata = f.read()
        lexemes = lexer(filedata)

        parser = Parser(lexemes)
        ast = parser.parse()
        type_check(ast)

        print(generate_message_impl(filename, ast))

if __name__ == "__main__":
    main()

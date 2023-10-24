#! python3
import os
import pathlib
import sys

from jinja2 import Environment, FileSystemLoader

from parser import *

def main():
    if len(sys.argv) != 2:
        sys.exit("Takes name of file.")

    filename = sys.argv[1]

    ast = None
    with open(filename, mode='r') as f:
        filedata = f.read()
        lexemes = lexer(filedata)

        parser = Parser(lexemes)
        ast = parser.parse()
        type_check(ast)

    messages = [m for m in ast if type(m) is Message]
    interfaces = [i for i in ast if type(i) is Interface]

    jinja_env = Environment(loader=FileSystemLoader(pathlib.Path(__file__).parent.resolve()))

    message_header_tmpl = jinja_env.get_template("message.h.jinja")
    with open(filename + '.h', mode='w') as f:
        message_header = message_header_tmpl.render(messages=messages)
        f.write(message_header)

    message_impl_tmpl = jinja_env.get_template("message.cpp.jinja")
    message_impl_tmpl.globals['Type'] = Type
    with open(filename + '.cpp', mode='w') as f:
        message_impl =  message_impl_tmpl.render(file=filename, messages=messages)
        f.write(message_impl)


if __name__ == "__main__":
    main()

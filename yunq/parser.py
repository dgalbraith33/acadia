from enum import Enum
import sys

class LexemeType(Enum):
    NONE = 0

    EOF = 1

    # Identifiers and Keywords
    NAME = 2
    
    # Symbols
    LEFT_BRACE = 3
    RIGHT_BRACE = 4
    LEFT_PAREN = 5
    RIGHT_PAREN = 6
    ARROW = 7
    SEMICOLON = 8
    DOT = 9


class Lexeme():
    def __init__(self, lextype: LexemeType, value = None):
        self.type = lextype
        self.value = value

    def __str__(self):
        if self.value:
            return "(%s, %s)" % (self.type, self.value)
        return "(%s)" % self.type

    def __repr__(self):
        return self.__str__()


def lexer(program: str):
    line = 1
    start = 0
    current = 0
    tokens: list[Lexeme] = [] 
    while current < len(program):
        # Scan next token.
        start = current
        curr = program[current]
        if curr == '\n':
            line += 1
        elif curr == '\t' or curr == ' ' or curr == '\r':
            pass
        elif curr == '{':
            tokens.append(Lexeme(LexemeType.LEFT_BRACE))
        elif curr == '}':
            tokens.append(Lexeme(LexemeType.RIGHT_BRACE))
        elif curr == '(':
            tokens.append(Lexeme(LexemeType.LEFT_PAREN))
        elif curr == ')':
            tokens.append(Lexeme(LexemeType.RIGHT_PAREN))
        elif curr == ';':
            tokens.append(Lexeme(LexemeType.SEMICOLON))
        elif curr == '.':
            tokens.append(Lexeme(LexemeType.DOT))
        elif curr == '-':
            current += 1
            if program[current] == '>': 
                tokens.append(Lexeme(LexemeType.ARROW))
            else:
                sys.exit("Expected > after - got '%s' on line %d" % (program[current], line))
        elif curr.isalpha():
            while program[current + 1].isalnum() or program[current + 1] == '_':
                current += 1
            tokens.append(Lexeme(LexemeType.NAME, program[start:current + 1]))
        elif curr == '/' and program[current + 1] == '/':
            while program[current] != '\n':
                current += 1
        else:
            sys.exit("Got unexpected token %s on line %s." % (curr, line))

        current += 1

    tokens.append(Lexeme(LexemeType.EOF))

    return tokens

class Package():
    def __init__(self, names: list[str]):
        self.names = names

    def cpp_namespace(self):
        return "::".join(self.names)

class Method():
    def __init__(self, name: str, request: str, response: str):
        self.name = name
        self.request = request
        self.response = response

class Interface():
    def __init__(self, name: str, methods: list[Method]):
        self.name = name
        self.methods = methods

class Type(Enum):
    NONE = 0
    U64 = 1
    I64 = 2
    STRING = 3
    BYTES = 4
    CAPABILITY = 5

type_str_dict = {
        "u64": Type.U64,
        "i64": Type.I64,
        "string": Type.STRING,
        "bytes": Type.BYTES,
        "capability": Type.CAPABILITY,
        }

type_to_cppstr = {
        Type.U64: "uint64_t",
        Type.I64: "int64_t",
        Type.STRING: "glcr::String",
        Type.CAPABILITY: "z_cap_t",
        Type.BYTES: "glcr::Vector<uint8_t>"
        }

class Field():
    def __init__(self, fieldtype: Type, name: str, repeated = False):
        self.type = fieldtype
        self.name = name
        self.repeated = repeated

    def cpp_type(self):
        return type_to_cppstr[self.type]

class Message():
    def __init__(self, name: str, fields: list[Field]):
        self.name = name
        self.fields = fields

Decl = Interface | Message

name_dict: dict[str, Decl] = {}

class Parser():
    def __init__(self, tokens: list[Lexeme]):
        self.tokens = tokens
        self.current = 0

    def peektype(self) -> LexemeType:
        return self.tokens[self.current].type

    def peekvalue(self) -> str:
        return self.tokens[self.current].value

    def consume(self) -> Lexeme:
        self.current += 1
        return self.tokens[self.current - 1]

    def consume_identifier(self) -> str:
        tok = self.consume()
        if tok.type != LexemeType.NAME:
            sys.exit("Expected identifier got %s" % tok.type)
        return tok.value

    def consume_check(self, lex_type: LexemeType):
        tok = self.consume()
        if tok.type != lex_type:
            sys.exit("Expected %s got %s" % (lex_type, tok.type))

    def consume_check_identifier(self, name: str):
        tok = self.consume()
        if tok.type != LexemeType.NAME:
            sys.exit("Expected '%s' got a %s" % (name, tok.type))
        if tok.value != name:
            sys.exit("Expected '%s' got '%s'" % (name, tok.value))

    def parse(self) -> list[Decl]:
        decls = []
        while self.peektype() != LexemeType.EOF:
            decls.append(self.decl())
        return decls

    def decl(self) -> Decl:
        token = self.consume()
        if token.type != LexemeType.NAME:
            sys.exit("Unexpected token: %s", token)
        if token.value == "package":
            # TODO: Enforce that package decl comes before all messages and interface.
            return self.package()
        if token.value == "message":
            return self.message()
        elif token.value == "interface":
            return self.interface()
        sys.exit("Unexpected identifier '%s', expected package, message, interface" % token.value)

    def package(self):
        names = [self.consume_identifier()]

        while self.peektype() == LexemeType.DOT:
            self.consume_check(LexemeType.DOT)
            names += [self.consume_identifier()]

        self.consume_check(LexemeType.SEMICOLON)

        return Package(names)

    def interface(self):
        # "interface" consumed by decl.
        name = self.consume_identifier()

        if name in name_dict.keys():
            sys.exit("Name '%s' already exists." % name)

        self.consume_check(LexemeType.LEFT_BRACE)

        methods: list[Method] = []
        method_names = set()
        while self.peektype() != LexemeType.RIGHT_BRACE:
            m = self.method()
            if m.name in method_names:
                sys.exit("Method %s declared twice on %s" % (m.name, name))
            method_names.add(m.name)
            methods.append(m)

        self.consume_check(LexemeType.RIGHT_BRACE)

        i = Interface(name, methods)
        name_dict[name] = i
        return i

    def method(self):
        self.consume_check_identifier("method")

        name = self.consume_identifier()

        self.consume_check(LexemeType.LEFT_PAREN)
        
        request = None
        # FIXME: Fix error handling here (and for response). We want to show
        # "expected rparen or identifier" if type is wrong rather than just 
        # "expected rparen".
        if self.peektype() == LexemeType.NAME:
            request = self.consume_identifier()

        self.consume_check(LexemeType.RIGHT_PAREN)
        self.consume_check(LexemeType.ARROW)
        self.consume_check(LexemeType.LEFT_PAREN)

        response = None
        if self.peektype() == LexemeType.NAME:
            response = self.consume_identifier()

        self.consume_check(LexemeType.RIGHT_PAREN)
        self.consume_check(LexemeType.SEMICOLON)

        return Method(name, request, response)

    def message(self):
        # "message" consumed by decl.

        name = self.consume_identifier()

        if name in name_dict:
            sys.exit("Name '%s' already exists." % name)

        self.consume_check(LexemeType.LEFT_BRACE)

        fields: list[Field] = []
        field_names = set()
        while self.peektype() != LexemeType.RIGHT_BRACE:
            f = self.field()
            if f.name in field_names:
                sys.exit("Field %s declared twice on %s" % (f.name, name))
            field_names.add(f.name)
            fields.append(f)

        self.consume_check(LexemeType.RIGHT_BRACE)

        m = Message(name, fields)
        name_dict[name] = m
        return m

    def field(self):

        repeated = False
        field_type_str = self.consume_identifier()
        if field_type_str == "repeated":
            repeated = True
            field_type_str = self.consume_identifier()
            
        if field_type_str not in type_str_dict.keys():
            sys.exit("Expected type got '%s'" % field_type_str)
        field_type = type_str_dict[field_type_str]

        name = self.consume_identifier()
        self.consume_check(LexemeType.SEMICOLON)
        return Field(field_type, name, repeated)

def type_check(decls: list[Decl]):
    if sum(1 for decl in decls if type(decl) is Package) > 1:
        sys.exit("Cannot have more than one package declaration")
    for decl in decls:
        if type(decl) is Interface:
            for method in decl.methods:
                if method.request is None and method.response is None:
                    sys.exit("Method '%s.%s' cannot have empty request and response" % (decl.name, method.name))
                if method.request is not None:
                  if method.request not in name_dict.keys():
                      sys.exit("Request type '%s' for '%s.%s' does not exist" % (method.request, decl.name, method.name))
                  if type(name_dict[method.request]) is not Message:
                      sys.exit("Request type '%s' for '%s.%s' should be a message" % (method.request, decl.name, method.name))
                if method.response is not None:
                  if method.response not in name_dict.keys():
                      sys.exit("Response type '%s' for '%s.%s' does not exist" % (method.response, decl.name, method.name))
                  if type(name_dict[method.response]) is not Message:
                      sys.exit("Response type '%s' for '%s.%s' should be a message" % (method.response, decl.name, method.name))

def print_ast(decls: list[Decl]):
    for decl in decls:
        if type(decl) is Package:
            print("%s (Package)" % decl.cpp_namespace())
        elif type(decl) is Interface:
            print("%s (Interface)" % decl.name)
            for method in decl.methods:
                print("\t%s (%s -> %s)" % (method.name, method.request, method.response))
        elif type(decl) is Message:
            print("%s (Message)" % decl.name)
            for field in decl.fields:
                print("\t%s %s" % (field.type.name, field.name))
        else:
            print("unknown type")




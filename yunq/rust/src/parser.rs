use std::fmt::Debug;
use std::fmt::Display;

use crate::lexer::Token;
use crate::lexer::TokenType;

pub enum Type {
    U64,
    I64,
    Message(String),
}

impl Display for Type {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "{}",
            match self {
                Type::U64 => "u64",
                Type::I64 => "i64",
                Type::Message(s) => s,
            }
        )
    }
}

impl TryFrom<&String> for Type {
    type Error = String;

    fn try_from(value: &String) -> Result<Self, Self::Error> {
        match value.as_str() {
            "u64" => Ok(Type::U64),
            "i64" => Ok(Type::I64),
            _ => Ok(Type::Message(value.clone())),
        }
    }
}

pub struct Field {
    field_type: Type,
    name: String,
    number: u64,
    repeated: bool,
}

pub struct Message {
    name: String,
    fields: Vec<Field>,
}

pub struct Method {
    name: String,
    number: u64,
    request: Option<String>,
    response: Option<String>,
}

impl Debug for Method {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let request_str = match &self.request {
            Some(s) => format!("({})", s),
            None => "()".to_string(),
        };
        let response_str = match &self.response {
            Some(s) => format!("({})", s),
            None => "()".to_string(),
        };

        write!(
            f,
            "{}: {}: {} -> {}",
            self.number, self.name, request_str, response_str
        )
    }
}

pub struct Interface {
    name: String,
    methods: Vec<Method>,
}

pub enum Decl {
    Message(Message),
    Interface(Interface),
    Package(Vec<String>),
}

impl Debug for Decl {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Decl::Message(m) => {
                writeln!(f, "Message {}", m.name)?;
                for field in &m.fields {
                    let typestr = if field.repeated {
                        format!("repeated {}", field.field_type)
                    } else {
                        field.field_type.to_string()
                    };
                    writeln!(f, "\t{}: {} ({})", field.number, field.name, typestr)?;
                }
            }
            Decl::Interface(i) => {
                writeln!(f, "Interface {}", i.name)?;
                for method in &i.methods {
                    writeln!(f, "\t{:?}", method)?;
                }
            }
            Decl::Package(p) => {
                writeln!(f, "Package {};", p.join("."))?;
            }
        }
        Ok(())
    }
}

pub struct Parser<'a> {
    tokens: &'a Vec<Token>,
    current_index: usize,
    ast: Vec<Decl>,
}

impl<'a> Parser<'a> {
    pub fn new(tokens: &'a Vec<Token>) -> Self {
        Self {
            tokens,
            current_index: 0,
            ast: Vec::new(),
        }
    }

    fn next_type_is(&self, tok_type: TokenType) -> bool {
        self.current_index < self.tokens.len()
            && self.tokens[self.current_index].token_type == tok_type
    }

    fn consume_token<'b>(&'b mut self) -> &'b Token {
        if self.current_index >= self.tokens.len() {
            panic!("Consumed tokens past end of input.")
        }

        let t = &self.tokens[self.current_index];
        self.current_index += 1;
        t
    }

    fn consume_token_type<'b>(&'b mut self, t: TokenType) -> Result<&'b Token, String> {
        let token = self.consume_token();
        if token.token_type == t {
            Ok(token)
        } else {
            Err(format!("Expected {:?} but found {}", t, token))
        }
    }

    fn consume_identifier<'b>(&'b mut self) -> Result<&'b Token, String> {
        self.consume_token_type(TokenType::Name)
    }

    fn consume_keyword<'b>(&'b mut self, keyword: &str) -> Result<&'b Token, String> {
        let token = self.consume_token_type(TokenType::Name)?;
        if token.chars != keyword {
            Err(format!("Expected '{}' but got {}", keyword, token))
        } else {
            Ok(token)
        }
    }

    fn package(&mut self) -> Result<Decl, String> {
        // 'package' was consumed by decl.
        let mut package_names = vec![self.consume_identifier()?.chars.clone()];
        while self.next_type_is(TokenType::Dot) {
            self.consume_token_type(TokenType::Dot)?;
            package_names.push(self.consume_identifier()?.chars.clone());
        }
        self.consume_token_type(TokenType::Semicolon)?;
        Ok(Decl::Package(package_names))
    }

    fn field(&mut self, number: u64) -> Result<Field, String> {
        let mut type_identifier = self.consume_identifier()?;
        let mut repeated = false;
        if type_identifier.chars == "repeated" {
            repeated = true;
            type_identifier = self.consume_identifier()?;
        }

        let parsed_type = Type::try_from(&type_identifier.chars)
            .map_err(|err| format!("{}: {}", err, type_identifier))?;
        let name_identifier = self.consume_identifier()?.chars.clone();

        self.consume_token_type(TokenType::Semicolon)?;

        Ok(Field {
            field_type: parsed_type,
            name: name_identifier,
            number,
            repeated,
        })
    }

    fn message(&mut self) -> Result<Decl, String> {
        // 'message' was consumed by decl.
        let name = self.consume_identifier()?.chars.clone();

        self.consume_token_type(TokenType::LeftBrace)?;

        let mut fields = Vec::new();
        let mut next_field_number = 0;
        while !self.next_type_is(TokenType::RightBrace) {
            fields.push(self.field(next_field_number)?);
            next_field_number += 1;
        }

        self.consume_token_type(TokenType::RightBrace)?;

        Ok(Decl::Message(Message { name, fields }))
    }

    fn method(&mut self, number: u64) -> Result<Method, String> {
        self.consume_keyword("method")?;

        let name = self.consume_identifier()?.chars.clone();

        self.consume_token_type(TokenType::LeftParen)?;
        let request = if self.next_type_is(TokenType::Name) {
            Some(self.consume_identifier()?.chars.clone())
        } else {
            None
        };
        self.consume_token_type(TokenType::RightParen)?;

        self.consume_token_type(TokenType::Arrow)?;

        self.consume_token_type(TokenType::LeftParen)?;
        let response = if self.next_type_is(TokenType::Name) {
            Some(self.consume_identifier()?.chars.clone())
        } else {
            None
        };
        self.consume_token_type(TokenType::RightParen)?;

        self.consume_token_type(TokenType::Semicolon)?;

        Ok(Method {
            name,
            request,
            response,
            number,
        })
    }

    fn interface(&mut self) -> Result<Decl, String> {
        let name = self.consume_identifier()?.chars.clone();

        self.consume_token_type(TokenType::LeftBrace)?;

        let mut methods = Vec::new();
        let mut next_method_number = 0;
        while !self.next_type_is(TokenType::RightBrace) {
            methods.push(self.method(next_method_number)?);
            next_method_number += 1;
        }

        self.consume_token_type(TokenType::RightBrace)?;

        Ok(Decl::Interface(Interface { name, methods }))
    }

    fn decl(&mut self) -> Result<Decl, String> {
        let token = self.consume_identifier()?;
        match token.chars.as_str() {
            "package" => self.package(),
            "message" => self.message(),
            "interface" => self.interface(),
            _ => Err(format!(
                "Expected one of 'package', 'message', or 'interface' but got {}",
                token
            )),
        }
    }

    pub fn parse_ast(&mut self) -> Result<(), String> {
        while !self.next_type_is(TokenType::EndOfFile) {
            let decl = self.decl()?;
            self.ast.push(decl);
        }
        Ok(())
    }

    pub fn ast(&'a mut self) -> &'a Vec<Decl> {
        &self.ast
    }
}

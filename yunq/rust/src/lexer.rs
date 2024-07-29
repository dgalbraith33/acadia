#[derive(Debug, PartialEq)]
pub enum TokenType {
    EndOfFile,
    Name,
    LeftBrace,
    RightBrace,
    LeftParen,
    RightParen,
    Arrow,
    Semicolon,
    Dot,
    Equals,
}

#[derive(Debug)]
#[allow(dead_code)]
pub struct Token {
    pub token_type: TokenType,
    line: usize,
    start: usize,
    end: usize,
    pub chars: String,
}

impl std::fmt::Display for Token {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "'{}' at line {} position {} ",
            self.chars, self.line, self.start
        )
    }
}

pub fn lex_input(input: &str) -> Result<Vec<Token>, String> {
    let mut index = 0;
    let mut position = 0;
    let mut line = 1;

    let mut tokens = Vec::new();

    let mut chars = input.chars().peekable();
    loop {
        match chars.next() {
            Some(c) => {
                position += 1;
                match c {
                    '\n' => {
                        position = 0;
                        line += 1;
                    }
                    ' ' | '\t' | '\r' => {}
                    '/' => {
                        match chars.next() {
                            Some('/') => {
                                index += 1;
                                // TODO: Add a consume until function.
                                loop {
                                    match chars.next() {
                                        Some('\n') => {
                                            index += 1;
                                            position = 0;
                                            line += 1;
                                            break;
                                        }
                                        Some(_) => {
                                            index += 1;
                                        }
                                        None => break,

                                    }

                                }
                            }
                            _ => {
                                return Err(format!("Unexpected token '/' at line {} position {}", line, position));
                            }

                        }
                    }
                    '{' => tokens.push(Token{
                        token_type: TokenType::LeftBrace,
                        line,
                        start: position,
                        end: position + 1,
                        chars: input[index..index+1].to_string(),

                    }),
                    '}' => tokens.push(Token{
                        token_type: TokenType::RightBrace,
                        line,
                        start: position,
                        end: position + 1,
                        chars: input[index..index+1].to_string(),

                    }),
                    '(' => tokens.push(Token{
                        token_type: TokenType::LeftParen,
                        line,
                        start: position,
                        end: position + 1,
                        chars: input[index..index+1].to_string(),

                    }),
                    ')' => tokens.push(Token{
                        token_type: TokenType::RightParen,
                        line,
                        start: position,
                        end: position + 1,
                        chars: input[index..index+1].to_string(),

                    }),
                    ';' => tokens.push(Token{
                        token_type: TokenType::Semicolon,
                        line,
                        start: position,
                        end: position + 1,
                        chars: input[index..index+1].to_string(),

                    }),
                    '.' => tokens.push(Token{
                        token_type: TokenType::Dot,
                        line,
                        start: position,
                        end: position + 1,
                        chars: input[index..index+1].to_string(),

                    }),
                    '=' => tokens.push(Token{
                        token_type: TokenType::Equals,
                        line,
                        start: position,
                        end: position + 1,
                        chars: input[index..index+1].to_string(),

                    }),
                    '-' => match chars.next() {
                        Some('>') => {
                            position += 1;
                            tokens.push(Token{
                                token_type: TokenType::Arrow,
                                line,
                                start: position - 1,
                                end: position + 1,
                                chars: input[index..index+1].to_string(),
                            });
                            index += 1;
                        }
                        Some(c) => return Err(format!("Expected > to follow - (to form arrow '->') on line {} at position {}. But got '{}'", line, position + 1, c)),
                        None => return Err(format!("Expected > to follow - but got end of input instead on line {} position {}", line, position))
                    },
                    'a'..='z' | 'A'..='Z' => {
                        let name_start = index;
                        let pos_start = position;
                        loop {
                            match chars.peek() {
                                Some(c) => {
                                    match c {
                                        'a'..='z' | 'A'..='Z' | '0'..='9' | '_' => {
                                            position += 1;
                                            chars.next();
                                            index += 1;
                                        },
                                        '\n' => {
                                            chars.next();
                                            tokens.push(Token{
                                                token_type: TokenType::Name,
                                                line,
                                                start: pos_start,
                                                end: position + 1,
                                                chars: input[name_start..index+1].to_string(),
                                            });
                                            position = 0;
                                            index += 1;
                                            line += 1;
                                            break;
                                        },
                                        _ => {
                                            tokens.push(Token{
                                                token_type: TokenType::Name,
                                                line,
                                                start: pos_start,
                                                end: position + 1,
                                                chars: input[name_start..index+1].to_string()
                                            });
                                            break;
                                        },
                                    }
                                }
                                None => {
                                    tokens.push(Token{
                                        token_type: TokenType::Name,
                                        line,
                                        start: pos_start,
                                        end: position + 1,
                                      chars: input[name_start..index].to_string(),
                                    });
                                    break;
                                }
                            }
                        }
                    },
                    _ => {
                        return Err(format!(
                            "Unexpected token on line {} character {}: {}",
                            line, position, c
                        ))
                    }
                }
            }
            None => {
                tokens.push(Token {
                    token_type: TokenType::EndOfFile,
                    line,
                    start: position,
                    end: position,
                    chars: "EOF".to_string(),
                });
                break;
            }
        }
        index += 1;
    }

    Ok(tokens)
}

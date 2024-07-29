mod codegen;
mod lexer;
mod parser;

use std::error::Error;

pub fn codegen(input: &str) -> Result<String, Box<dyn Error>> {
    let tokens = lexer::lex_input(input)?;

    let mut ast_parser = parser::Parser::new(&tokens);
    ast_parser.parse_ast()?;
    ast_parser.type_check()?;

    Ok(codegen::generate_code(ast_parser.ast()))
}

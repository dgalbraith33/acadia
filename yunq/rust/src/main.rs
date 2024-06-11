mod lexer;
mod parser;

use std::error::Error;
use std::fs::read_to_string;

use clap::Parser;

#[derive(Parser)]
#[command(about)]
struct Args {
    // The .yunq file to parse
    #[arg(short, long)]
    input_path: String,
}

fn main() -> Result<(), Box<dyn Error>> {
    let args = Args::parse();
    let input = read_to_string(args.input_path)?;
    let tokens = lexer::lex_input(&input)?;

    let mut ast_parser = parser::Parser::new(&tokens);
    ast_parser.parse_ast()?;
    ast_parser.type_check()?;

    for decl in ast_parser.ast() {
        println!("{:?}", decl);
    }

    Ok(())
}

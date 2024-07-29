use std::error::Error;
use std::fs;

use clap::Parser;

#[derive(Parser)]
#[command(about)]
struct Args {
    // The .yunq file to parse
    #[arg(short, long)]
    input_path: String,

    // The .rs file to generate
    #[arg(short, long)]
    output_path: String,
}

fn main() -> Result<(), Box<dyn Error>> {
    let args = Args::parse();
    let input = fs::read_to_string(args.input_path)?;

    let code = yunqc::codegen(&input)?;

    fs::write(args.output_path, code)?;

    Ok(())
}

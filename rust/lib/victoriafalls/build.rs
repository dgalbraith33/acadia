use std::fs;

fn main() {
    let input_file = "../../../sys/victoriafalls/lib/victoriafalls/victoriafalls.yunq";

    println!("cargo::rerun-if-changed={input_file}");

    let input = fs::read_to_string(input_file).expect("Failed to read input file");

    let code = yunqc::codegen(&input).expect("Failed to generate yunq code.");

    let out = std::env::var("OUT_DIR").unwrap() + "/yunq.rs";
    fs::write(out, code).expect("Failed to write generated code.");
}

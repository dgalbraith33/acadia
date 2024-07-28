use std::process::Command;

fn main() {
    let out = std::env::var("OUT_DIR").unwrap() + "/yunq.rs";

    let status = Command::new("cargo")
        .current_dir("../../../yunq/rust")
        .arg("run")
        .arg("--")
        .arg("--input-path")
        .arg("../../sys/yellowstone/lib/yellowstone/yellowstone.yunq")
        .arg("--output-path")
        .arg(out)
        .status()
        .expect("Failed to start execution");

    assert!(status.success());
}

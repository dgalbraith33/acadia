use std::env;

fn main() {
    let mut curr_directory = env::current_dir().unwrap();
    println!("{:?}", curr_directory);
    assert!(curr_directory.pop());
    assert!(curr_directory.pop());
    assert!(curr_directory.pop());

    curr_directory.push("builddbg");
    curr_directory.push("zion");

    println!(
        "cargo:rustc-link-search={}",
        curr_directory.to_str().unwrap()
    );
    println!("cargo:rustc-link-lib=zion_stub");
}

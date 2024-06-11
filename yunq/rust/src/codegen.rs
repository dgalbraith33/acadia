use crate::parser::{Decl, Field, Interface, Message};
use genco::fmt;
use genco::fmt::FmtWriter;
use genco::prelude::quote_fn;
use genco::prelude::quote_in;
use genco::prelude::rust;
use genco::prelude::FormatInto;
use genco::prelude::Rust;

pub fn generate_field_def<'a>(field: &'a Field) -> impl FormatInto<Rust> + 'a {
    quote_fn!(
        $(field.name.clone()): $(field.field_type.rust_type()),
    )
}

pub fn generate_message_code<'a>(message: &'a Message) -> impl FormatInto<Rust> + 'a {
    quote_fn!(
    struct $(&message.name) {$['\r']
    $(for field in &message.fields =>
        $[' ']$(generate_field_def(field))$['\r']
    )}$['\n']

    impl $(&message.name) {$['\r']
        jjj

    }$['\n']
    )
}

pub fn generate_code(ast: &Vec<Decl>) -> Result<String, std::fmt::Error> {
    let mut tokens = rust::Tokens::new();

    for decl in ast {
        match decl {
            Decl::Message(message) => quote_in!(tokens => $(generate_message_code(message))),
            _ => {}
        }
    }

    let mut w = FmtWriter::new(String::new());

    let fmt = fmt::Config::from_lang::<Rust>()
        .with_indentation(fmt::Indentation::Space(4))
        .with_newline("\n");

    let config = rust::Config::default()
        // Prettier imports and use.
        .with_default_import(rust::ImportMode::Qualified);

    tokens.format_file(&mut w.as_formatter(&fmt), &config)?;

    Ok(w.into_inner())
}

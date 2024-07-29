use crate::parser::{Decl, Interface, Message, Method};
use convert_case::{Case, Casing};
use proc_macro2::Ident;
use proc_macro2::Span;
use proc_macro2::TokenStream;
use quote::quote;

fn ident(s: &str) -> Ident {
    Ident::new(s, Span::call_site())
}

fn generate_message(message: &Message) -> TokenStream {
    let name = ident(&message.name);
    let field_names = message.fields.iter().map(|f| ident(&f.name));
    let field_types = message
        .fields
        .iter()
        .map(|f| Ident::new(f.field_type.rust_type(), Span::call_site()));
    quote! {
        #[derive(YunqMessage)]
        pub struct #name {
            #(pub #field_names: #field_types),*
        }

    }
}

fn generate_method(method: &Method) -> TokenStream {
    let name = ident(&method.name.to_case(Case::Snake));
    let maybe_req = method.request.clone().map(|r| ident(&r));
    let maybe_resp = method.response.clone().map(|r| ident(&r));
    match (maybe_req, maybe_resp) {
        (Some(req), Some(resp)) => quote! {
            pub fn #name (&mut self, req: & #req) -> Result<#resp, ZError> {
                yunq::client::call_endpoint(req, &mut self.byte_buffer, self.endpoint_cap)
            }
        },
        (Some(req), None) => quote! {
            pub fn #name (&mut self, req: & #req) -> Result<yunq::message::Empty, ZError> {
                yunq::client::call_endpoint(req, &mut self.byte_buffer, self.endpoint_cap)
            }
        },
        (None, Some(resp)) => quote! {
            pub fn #name (&mut self) -> Result<#resp, ZError> {
                yunq::client::call_endpoint(&yunq::message::Empty{}, &mut self.byte_buffer, self.endpoint_cap)
            }
        },
        _ => unreachable!(),
    }
}

fn generate_interface(interface: &Interface) -> TokenStream {
    let client_name = interface.name.clone() + "Client";
    let name = ident(&client_name);
    let methods = interface.methods.iter().map(|m| generate_method(&m));
    quote! {
        pub struct #name {
            endpoint_cap: z_cap_t,
            byte_buffer: ByteBuffer<0x1000>,
        }

        impl #name {
            pub fn new(endpoint_cap: z_cap_t) -> Self {
                Self {
                    endpoint_cap,
                    byte_buffer: ByteBuffer::new(),
                }
            }
            #(#methods)*
        }

    }
}

pub fn generate_code(ast: &Vec<Decl>) -> String {
    let prelude = quote! {

    extern crate alloc;

    use alloc::string::String;
    use alloc::string::ToString;
    use mammoth::zion::z_cap_t;
    use mammoth::zion::ZError;
    use yunq::ByteBuffer;
    use yunq::YunqMessage;
    use yunq_derive::YunqMessage;
    };

    let message_decls = ast
        .iter()
        .filter_map(|decl| match decl {
            Decl::Message(m) => Some(m),
            _ => None,
        })
        .map(|message| generate_message(&message));

    let interface_decls = ast
        .iter()
        .filter_map(|decl| match decl {
            Decl::Interface(i) => Some(i),
            _ => None,
        })
        .map(|interface| generate_interface(&interface));

    let output = quote! {
        #prelude

        #(#message_decls)*

        #(#interface_decls)*
    }
    .to_string();

    prettyplease::unparse(&syn::parse_file(&output).unwrap())
}

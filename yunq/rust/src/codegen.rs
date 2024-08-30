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
        #[derive(YunqMessage, PartialEq, Eq)]
        pub struct #name {
            #(pub #field_names: #field_types),*
        }

    }
}

fn generate_method(method: &Method) -> TokenStream {
    let id = proc_macro2::Literal::u64_suffixed(method.number);
    let name = ident(&method.name.to_case(Case::Snake));
    let maybe_req = method.request.clone().map(|r| ident(&r));
    let maybe_resp = method.response.clone().map(|r| ident(&r));
    match (maybe_req, maybe_resp) {
        (Some(req), Some(resp)) => quote! {
            pub fn #name (&mut self, req: & #req) -> Result<#resp, ZError> {
                yunq::client::call_endpoint(#id, req, &mut self.byte_buffer, &self.endpoint_cap)
            }
        },
        (Some(req), None) => quote! {
            pub fn #name (&mut self, req: & #req) -> Result<yunq::message::Empty, ZError> {
                yunq::client::call_endpoint(#id, req, &mut self.byte_buffer, &self.endpoint_cap)
            }
        },
        (None, Some(resp)) => quote! {
            pub fn #name (&mut self) -> Result<#resp, ZError> {
                yunq::client::call_endpoint(#id, &yunq::message::Empty{}, &mut self.byte_buffer, &self.endpoint_cap)
            }
        },
        _ => unreachable!(),
    }
}

fn generate_client(interface: &Interface) -> TokenStream {
    let client_name = interface.name.clone() + "Client";
    let name = ident(&client_name);
    let methods = interface.methods.iter().map(|m| generate_method(&m));
    quote! {
        pub struct #name {
            endpoint_cap: Capability,
            byte_buffer: ByteBuffer<0x1000>,
        }

        impl #name {
            pub fn new(endpoint_cap: Capability) -> Self {
                Self {
                    endpoint_cap,
                    byte_buffer: ByteBuffer::new(),
                }
            }
            #(#methods)*
        }

    }
}

fn generate_server_case(method: &Method) -> TokenStream {
    let id = proc_macro2::Literal::u64_suffixed(method.number);
    let name = ident(&method.name.to_case(Case::Snake));
    let maybe_req = method.request.clone().map(|r| ident(&r));
    let maybe_resp = method.response.clone().map(|r| ident(&r));
    match (maybe_req, maybe_resp) {
        (Some(req), Some(_)) => quote! {
            #id => {
                let req = #req::parse_from_request(byte_buffer, cap_buffer)?;
                let resp = self.handler.#name(req)?;
                cap_buffer.resize(0, 0);
                let resp_len = resp.serialize_as_request(0, byte_buffer, cap_buffer)?;
                Ok(resp_len)
            },
        },
        (Some(req), None) => quote! {
            #id => {
                let req = #req::parse_from_request(byte_buffer, cap_buffer)?;
                self.handler.#name(req)?;
                cap_buffer.resize(0, 0);
                // TODO: Implement serialization for EmptyMessage so this is less hacky.
                yunq::message::serialize_error(byte_buffer, ZError::from(0));
                Ok(0x10)
            },
        },
        (None, Some(_)) => quote! {
            #id => {
                let resp = self.handler.#name()?;
                cap_buffer.resize(0, 0);
                let resp_len = resp.serialize_as_request(0, byte_buffer, cap_buffer)?;
                Ok(resp_len)
            },
        },
        _ => unreachable!(),
    }
}

fn generate_server_method(method: &Method) -> TokenStream {
    let name = ident(&method.name.to_case(Case::Snake));
    let maybe_req = method.request.clone().map(|r| ident(&r));
    let maybe_resp = method.response.clone().map(|r| ident(&r));
    match (maybe_req, maybe_resp) {
        (Some(req), Some(resp)) => quote! {
            fn #name (&mut self, req: #req) -> Result<#resp, ZError>;
        },
        (Some(req), None) => quote! {
            fn #name (&mut self, req: #req) -> Result<(), ZError>;
        },
        (None, Some(resp)) => quote! {
            fn #name (&mut self) -> Result<#resp, ZError>;
        },
        _ => unreachable!(),
    }
}

fn generate_server(interface: &Interface) -> TokenStream {
    let server_name = ident(&(interface.name.clone() + "Server"));
    let server_trait = ident(&(interface.name.clone() + "ServerHandler"));
    let server_trait_methods = interface.methods.iter().map(|m| generate_server_method(&m));
    let server_match_cases = interface.methods.iter().map(|m| generate_server_case(&m));
    quote! {
        pub trait #server_trait {
            #(#server_trait_methods)*
        }

        pub struct #server_name<T: #server_trait> {
            endpoint_cap: Capability,
            handler: T
        }

        impl<T: #server_trait> #server_name<T> {
            pub fn new(handler: T) -> Result<Self, ZError> {
                Ok(Self {
                    endpoint_cap: syscall::endpoint_create()?,
                    handler,
                })
            }

            pub fn run_server(&self) -> Result<Box<thread::Thread>, ZError> {
                let thread_entry = |server_ptr: *const c_void| {
                    let server = unsafe { (server_ptr as *mut #server_name<T>).as_mut().expect("Failed to convert to server") };
                    server.server_loop();
                };
                thread::Thread::spawn(
                    thread_entry,
                    self as *const Self as *const core::ffi::c_void,
                )
            }
        }

        impl<T: #server_trait> yunq::server::YunqServer for #server_name<T> {
            fn endpoint_cap(&self) -> &Capability {
                &self.endpoint_cap
            }

            fn handle_request(
                &mut self,
                method_number: u64,
                byte_buffer: &mut ByteBuffer<1024>,
                cap_buffer: &mut Vec<z_cap_t>,
                ) -> Result<usize, ZError> {
                match method_number {
                    #(#server_match_cases)*

                    _ => Err(ZError::UNIMPLEMENTED)
                }
            }
        }
    }
}

fn generate_interface(interface: &Interface) -> TokenStream {
    let client = generate_client(interface);
    let server = generate_server(interface);
    quote! {
        #client

        #server
    }
}

pub fn generate_code(ast: &Vec<Decl>) -> String {
    let prelude = quote! {

    extern crate alloc;

    use alloc::boxed::Box;
    use alloc::string::String;
    use alloc::string::ToString;
    use alloc::vec::Vec;
    use core::ffi::c_void;
    use mammoth::syscall;
    use mammoth::thread;
    use mammoth::cap::Capability;
    use mammoth::zion::z_cap_t;
    use mammoth::zion::ZError;
    use yunq::ByteBuffer;
    use yunq::YunqMessage;
    use yunq::server::YunqServer;
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

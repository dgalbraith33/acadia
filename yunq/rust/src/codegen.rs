use crate::parser::{Decl, Interface, Message, Method, Type};
use crate::parser::{Field, FieldType};
use convert_case::{Case, Casing};
use proc_macro2::Ident;
use proc_macro2::Span;
use proc_macro2::TokenStream;
use quote::quote;

fn ident(s: &str) -> Ident {
    Ident::new(s, Span::call_site())
}

fn to_path(field_type: &FieldType) -> TokenStream {
    let rust_type = ident(&field_type.inner_type.rust_type());
    if field_type.repeated {
        quote! {
            Vec::<#rust_type>
        }
    } else {
        quote! {
            #rust_type
        }
    }
}

fn serialize_field(field: &Field) -> proc_macro2::TokenStream {
    let ind = field.number as usize;
    let name = ident(&field.name);
    if field.field_type.repeated {
        match &field.field_type.inner_type {
            Type::String => unimplemented!(),
            Type::Capability => unimplemented!(),
            Type::U64 => quote! {
                {
                    let rep_offset = next_extension;
                    let rep_len = self.#name.len() as u32;
                    next_extension = yunq::message::serialize_repeated(buf, next_extension as usize, &self.#name)? as u32;

                    buf.write_at(yunq::message::field_offset(offset, #ind), rep_offset)?;
                    buf.write_at(yunq::message::field_offset(offset, #ind) + 4, rep_len)?;
                }
            },
            Type::I64 => unimplemented!(),
            Type::Bytes => unimplemented!(),
            Type::Message(_) => quote! {
                {
                    let rep_offset = next_extension;
                    let rep_len = self.#name.len() as u32;
                    next_extension = yunq::message::serialize_repeated_message(buf, next_extension as usize, &self.#name, caps)? as u32;

                    buf.write_at(yunq::message::field_offset(offset, #ind), rep_offset)?;
                    buf.write_at(yunq::message::field_offset(offset, #ind) + 4, rep_len)?;
                }
            },
        }
    } else {
        match &field.field_type.inner_type {
            Type::String => quote! {
                {
                    let str_offset = next_extension;
                    let str_length = self.#name.len() as u32;

                    buf.write_str_at(offset + next_extension as usize, &self.#name)?;
                    next_extension += str_length;

                    buf.write_at(yunq::message::field_offset(offset, #ind), str_offset)?;
                    buf.write_at(yunq::message::field_offset(offset, #ind) + 4, str_length)?;
                }
            },
            Type::Capability => quote! {
                {
                    let cap_ind = caps.len();
                    caps.push(self.#name);

                    buf.write_at(yunq::message::field_offset(offset, #ind), cap_ind as u64)?;
                }
            },
            Type::U64 => quote! {
                {
                    buf.write_at(yunq::message::field_offset(offset, #ind), self.#name)?;
                }
            },
            Type::I64 => quote! {
                {
                    buf.write_at(yunq::message::field_offset(offset, #ind), self.#name)?;
                }
            },
            Type::Message(_) => quote! {
                {
                    let msg_offset = next_extension;
                    let msg_length = self.#name.serialize(buf, offset + next_extension as usize, caps)? as u32;
                    next_extension += msg_length;

                    buf.write_at(yunq::message::field_offset(offset, #ind), msg_offset)?;
                    buf.write_at(yunq::message::field_offset(offset, #ind) + 4, msg_length)?;
                }
            },
            Type::Bytes => unimplemented!(),
        }
    }
}

fn parse_field(field: &Field) -> TokenStream {
    let ind = field.number as usize;
    let name = ident(&field.name);
    if field.field_type.repeated {
        match &field.field_type.inner_type {
            Type::String => unimplemented!(),
            Type::Capability => unimplemented!(),
            Type::U64 => quote! {
                let #name = {
                    let rep_offset = buf.at::<u32>(yunq::message::field_offset(offset, #ind))?;
                    let rep_len = buf.at::<u32>(yunq::message::field_offset(offset, #ind) + 4)?;

                    yunq::message::parse_repeated(buf, rep_offset as usize, rep_len as usize)?
                };
            },
            Type::I64 => unimplemented!(),
            Type::Bytes => unimplemented!(),
            Type::Message(_s) => quote! {
                let #name = {
                    let rep_offset = buf.at::<u32>(yunq::message::field_offset(offset, #ind))?;
                    let rep_len = buf.at::<u32>(yunq::message::field_offset(offset, #ind) + 4)?;

                    yunq::message::parse_repeated_message(buf, rep_offset as usize, rep_len as usize, &caps)?
                };
            },
        }
    } else {
        match &field.field_type.inner_type {
            Type::String => quote! {
                let #name = {
                    let str_offset = buf.at::<u32>(yunq::message::field_offset(offset, #ind))?;
                    let str_length = buf.at::<u32>(yunq::message::field_offset(offset, #ind) + 4)?;

                    buf.str_at(offset + str_offset as usize, str_length as usize)?.to_string()
                };
            },
            Type::Capability => quote! {
                let #name = {
                    let cap_ind = buf.at::<u64>(yunq::message::field_offset(offset, #ind))?;
                    caps[cap_ind as usize]
                };
            },
            Type::U64 => quote! {
                let #name = buf.at::<u64>(yunq::message::field_offset(offset, #ind))?;
            },
            Type::I64 => quote! {
                let #name = buf.at::<i64>(yunq::message::field_offset(offset, #ind))?;
            },
            Type::Bytes => {
                unimplemented!();
            }
            Type::Message(s) => {
                let m_type = ident(&s);
                quote! {
                    let #name = {
                        let msg_offset = buf.at::<u32>(yunq::message::field_offset(offset, #ind))? as usize;

                        #m_type::parse(buf, offset + msg_offset, caps)?
                    };
                }
            }
        }
    }
}

fn generate_serialize(message: &Message) -> TokenStream {
    let num_fields = message.fields.len();
    let serializers = message.fields.iter().map(serialize_field);
    quote! {
        #[allow(unused_variables)] // caps may be unused.
        fn serialize<const N: usize>(
            &self,
            buf: &mut yunq::ByteBuffer<N>,
            offset: usize,
            caps: &mut alloc::vec::Vec<z_cap_t>,
        ) -> Result<usize, ZError> {
            let num_fields = #num_fields;
            let core_size: u32 = (yunq::message::MESSAGE_HEADER_SIZE + 8 * num_fields) as u32;
            #[allow(unused_mut)]
            let mut next_extension = core_size;

            #(#serializers)*

            buf.write_at(offset + 0, yunq::message::MESSAGE_IDENT)?;
            buf.write_at(offset + 4, core_size)?;
            buf.write_at(offset + 8, next_extension)?;
            buf.write_at(offset + 12, 0 as u32)?;
            Ok(next_extension as usize)
        }
    }
}

fn generate_parse(message: &Message) -> TokenStream {
    let field_names = message.fields.iter().map(|field| ident(&field.name));
    let parsers = message.fields.iter().map(parse_field);
    quote! {
        #[allow(unused_variables)] // caps may be unused.
        fn parse<const N: usize>(
            buf: &yunq::ByteBuffer<N>,
            offset: usize,
            caps: &alloc::vec::Vec<z_cap_t>,
            ) -> Result<Self, ZError>
        where
            Self: Sized,
        {
            if buf.at::<u32>(offset + 0)? != yunq::message::MESSAGE_IDENT {
                return Err(ZError::INVALID_ARGUMENT);
            }
            // TODO: Parse core size.
            // TODO: Parse extension size.
            // TODO: Check CRC32
            // TODO: Parse options.

            #(#parsers)*

            Ok(Self {
                #(#field_names),*
            })

        }
    }
}

fn generate_message(message: &Message) -> TokenStream {
    let name = ident(&message.name);
    let field_names = message.fields.iter().map(|f| ident(&f.name));
    let field_types = message.fields.iter().map(|f| to_path(&f.field_type));

    let serialize = generate_serialize(message);
    let parse = generate_parse(message);
    quote! {
        #[derive(PartialEq, Eq)]
        pub struct #name {
            #(pub #field_names: #field_types),*
        }

        impl YunqMessage for #name {
            #serialize

            #parse
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

    use alloc::string::String;
    use alloc::string::ToString;
    use alloc::vec::Vec;
    use mammoth::zion::z_cap_t;
    use mammoth::zion::ZError;
    use yunq::ByteBuffer;
    use yunq::YunqMessage;


    // Used only by modules with an interface.
    #[allow(unused_imports)]
    use alloc::boxed::Box;
    #[allow(unused_imports)]
    use core::ffi::c_void;
    #[allow(unused_imports)]
    use mammoth::cap::Capability;
    #[allow(unused_imports)]
    use mammoth::syscall;
    #[allow(unused_imports)]
    use mammoth::thread;
    #[allow(unused_imports)]
    use yunq::server::YunqServer;

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

    let tokens = syn::parse_file(&output).unwrap_or_else(|e| {
        println!("{}", output);
        panic!("{}", e);
    });

    prettyplease::unparse(&tokens)
}

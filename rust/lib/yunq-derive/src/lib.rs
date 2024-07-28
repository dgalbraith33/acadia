use proc_macro::{self, TokenStream};
use quote::{quote, ToTokens};
use syn::{parse_macro_input, Data, DataStruct, DeriveInput, Fields, Ident, Path, Type, TypePath};

fn serialize_field(name: &Ident, ind: usize, path: &Path) -> proc_macro2::TokenStream {
    if path.is_ident("String") {
        quote! {
        {
            let str_offset = next_extension;
            let str_length = self.#name.len() as u32;

            buf.write_str_at(offset + next_extension as usize, &self.#name)?;
            next_extension += str_length;

            buf.write_at(yunq::message::field_offset(offset, #ind), str_offset)?;
            buf.write_at(yunq::message::field_offset(offset, #ind) + 4, str_length)?;
        }

        }
    } else if path.is_ident("z_cap_t") {
        quote! {
            {
                let cap_ind = caps.len();
                caps.push(self.#name);

                buf.write_at(yunq::message::field_offset(offset, #ind), cap_ind as u64)?;
            }
        }
    } else if path.is_ident("u64") {
        quote! {
            {
                buf.write_at(yunq::message::field_offset(offset, #ind), self.#name as u64)?;
            }
        }
    } else {
        panic!(
            "Serialization not implemented for: {}",
            path.to_token_stream()
        )
    }
}

fn parse_field(name: &Ident, ind: usize, path: &Path) -> proc_macro2::TokenStream {
    if path.is_ident("String") {
        quote! {
            let #name = {
                let str_offset = buf.at::<u32>(yunq::message::field_offset(offset, #ind))?;
                let str_length = buf.at::<u32>(yunq::message::field_offset(offset, #ind) + 4)?;

                buf.str_at(offset + str_offset as usize, str_length as usize)?.to_string()
            };
        }
    } else if path.is_ident("z_cap_t") {
        quote! {
            let #name = {
                let cap_ind = buf.at::<u64>(yunq::message::field_offset(offset, #ind))?;
                caps[cap_ind as usize]
            };
        }
    } else if path.is_ident("u64") {
        quote! {
            let #name = buf.at::<u64>(yunq::message::field_offset(offset, #ind))?;
        }
    } else {
        panic!("Parsing not implemented for: {}", path.into_token_stream());
    }
}

fn field_names(name: &Ident, _ind: usize, _path: &Path) -> proc_macro2::TokenStream {
    quote! { #name }
}

fn apply_to_struct_fields<T>(input: &DeriveInput, func: fn(&Ident, usize, &Path) -> T) -> Vec<T> {
    match &input.data {
        Data::Struct(DataStruct {
            fields: Fields::Named(fields),
            ..
        }) => fields
            .named
            .iter()
            .enumerate()
            .map(|(ind, field)| match &field.ty {
                Type::Path(TypePath { path, .. }) => {
                    func(&field.ident.as_ref().unwrap(), ind, path)
                }
                _ => {
                    panic!("Unrecognized type: {:?}", field)
                }
            })
            .collect(),
        _ => {
            panic!("Unrecognized input (Expected Struct): {:?}", input.data)
        }
    }
}

#[proc_macro_derive(YunqMessage)]
pub fn derive_client(input_tokens: TokenStream) -> TokenStream {
    let input: DeriveInput = parse_macro_input!(input_tokens);
    let ident = input.ident.clone();

    let prelude = quote! {
        impl YunqMessage for #ident
    };

    let num_fields = apply_to_struct_fields(&input, |_, _, _| ()).len();

    let serializers = apply_to_struct_fields(&input, serialize_field);
    let serialize = quote! {
        fn serialize<const N: usize>(
            &self,
            buf: &mut yunq::ByteBuffer<N>,
            offset: usize,
            caps: &mut alloc::vec::Vec<z_cap_t>,
        ) -> Result<usize, ZError> {
            let num_fields = #num_fields;
            let core_size: u32 = (yunq::message::MESSAGE_HEADER_SIZE + 8 * num_fields) as u32;
            let mut next_extension = core_size;

            #(#serializers)*

            buf.write_at(offset + 0, yunq::message::MESSAGE_IDENT)?;
            buf.write_at(offset + 4, core_size)?;
            buf.write_at(offset + 8, next_extension)?;
            buf.write_at(offset + 12, 0 as u32)?;
            Ok(next_extension as usize)
        }
    };

    let field_names = apply_to_struct_fields(&input, field_names);
    let parsers = apply_to_struct_fields(&input, parse_field);
    let parse = quote! {
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
    };

    let output = quote! {
        #prelude {
            #serialize

            #parse
        }
    };
    output.into()
}

#![no_std]

include!(concat!(env!("OUT_DIR"), "/yunq.rs"));

#[cfg(test)]
mod tests {
    use super::*;

    extern crate std;
    use std::println;
    use std::vec;

    #[test]
    fn basic_serialization() -> Result<(), ZError> {
        let basic = Basic { unsigned_int: 82, signed_int: -1234, strn: "abc".to_string() };

        let mut buf = ByteBuffer::<1024>::new();
        let mut caps = Vec::new();
        basic.serialize(&mut buf, 0, &mut caps)?;

        let parsed = Basic::parse(&buf, 0, &caps)?;

        assert!(parsed == basic);

        Ok(())
    }

    #[test]
    fn basic_serialization_as_request() -> Result<(), ZError> {
        let basic = Basic { unsigned_int: 82, signed_int: -1234, strn: "abc".to_string() };

        let mut buf = ByteBuffer::<1024>::new();
        let mut caps = Vec::new();
        let req_id = 12;
        basic.serialize_as_request(req_id, &mut buf, &mut caps)?;
        
        assert!(buf.at::<u64>(8)? == req_id);

        let parsed = Basic::parse_from_request(&buf, &caps)?;

        assert!(parsed == basic);
        Ok(())
    }

    #[test]
    fn capability_serialization() -> Result<(), ZError> {
        let cap_id = 100;
        let cap = Cap { cap: cap_id };

        let mut buf = ByteBuffer::<1024>::new();
        let mut caps = Vec::new();
        cap.serialize(&mut buf, 0, &mut caps)?;
        
        assert!(caps.len() == 1);
        assert!(caps[0] == cap_id);

        let parsed = Cap::parse(&buf, 0, &caps)?;

        assert!(parsed == cap);

        Ok(())
    }

    #[test]
    fn repeated_serialization() -> Result<(), ZError> {
        let rep = Repeated { unsigned_ints: vec![0, 1, 3],};

        let mut buf = ByteBuffer::<1024>::new();
        let mut caps = Vec::new();
        rep.serialize(&mut buf, 0, &mut caps)?;
        
        let parsed = Repeated::parse(&buf, 0, &caps)?;

        assert!(parsed == rep);

        Ok(())
    }

    #[test]
    fn nested_serialization() -> Result<(), ZError> {
        let nested = Nested {
            basic:  Basic { unsigned_int: 82, signed_int: -1234, strn: "abc".to_string() },
            cap1: Cap { cap: 37},
            cap2: Cap { cap: 39},
        };

        let mut buf = ByteBuffer::<1024>::new();
        let mut caps = Vec::new();
        nested.serialize(&mut buf, 0, &mut caps)?;
        
        let parsed = Nested::parse(&buf, 0, &caps)?;


        assert!(parsed == nested);

        Ok(())
    }

    #[test]
    fn repeated_nested_serialization() -> Result<(), ZError> {
        let nested = RepeatedNested {
            basics:  vec![Basic { unsigned_int: 82, signed_int: -1234, strn: "abc".to_string(),},
            Basic { unsigned_int: 21, signed_int: -8, strn: "def".to_string(), },],
            caps: vec![Cap{ cap: 123}, Cap {cap: 12343}],
        };

        let mut buf = ByteBuffer::<1024>::new();
        let mut caps = Vec::new();
        nested.serialize(&mut buf, 0, &mut caps)?;
        
        let parsed = RepeatedNested::parse(&buf, 0, &caps)?;


        assert!(parsed == nested);


        Ok(())
    }


}

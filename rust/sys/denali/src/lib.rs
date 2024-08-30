#![no_std]

use core::include;

include!(concat!(env!("OUT_DIR"), "/yunq.rs"));

pub mod ahci;

#![no_std]
#![feature(box_into_inner)]

use core::include;

include!(concat!(env!("OUT_DIR"), "/yunq.rs"));

pub mod listener;

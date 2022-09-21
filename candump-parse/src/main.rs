use std::convert::TryInto;
use std::fs::File;
use std::io::{BufRead, BufReader};
use std::path::PathBuf;

use clap::{value_parser, Parser, ValueEnum};
use clap_num::maybe_hex;
use hex::decode;
use sscanf::scanf;

#[derive(Debug, Parser)]
struct Args {
    // list of candump files to process
    dump_file: Vec<PathBuf>,

    // show data frames sorted by timestamp
    #[arg(long)]
    data: bool,

    // show only frames with id -- TODO make this a list
    #[arg(long, value_parser = maybe_hex::<u32>)]
    id: Option<u32>,

    // when converting bytes to values, how endian'ness to use
    #[arg(long, default_value_t = Endian::Little, value_enum)]
    endian: Endian,
    
    // start byte
    #[arg(long, default_value_t = 0, value_parser = value_parser!(u8).range(0..=7))]
    start_byte: u8,

    // start bit
    #[arg(long, default_value_t = 0xff, value_parser = maybe_hex::<u8>)]
    start_mask: u8,

    // end byte
    #[arg(long, default_value_t = 7, value_parser = value_parser!(u8).range(0..=7))]
    end_byte: u8,

    // bit mask to apply to end_byte
    #[arg(long, default_value_t = 0xff, value_parser = maybe_hex::<u8>)]
    end_mask: u8,

    // value factor
    #[arg(long, default_value_t = 1.0, allow_hyphen_values(true))]
    factor: f64,

    // value offset
    #[arg(long, default_value_t = 0.0, allow_hyphen_values(true))]
    offset: f64,
}

#[derive(Debug, Copy, Clone, PartialEq, Eq, PartialOrd, Ord, ValueEnum)]
enum Endian {
    Little,
    Big,
}

#[derive(Debug)]
struct CanFrame {
    timestamp: f64,
    device: String,
    id: u32,
    data: Vec<u8>,
}

impl CanFrame {
    fn new(timestamp: f64, device: String, id: String, data: String) -> Self {
        Self {
            timestamp,
            device,
            id: u32::from_be_bytes(decode(id).unwrap().try_into().unwrap()),
            data: decode(data).unwrap(),
        }
    }

    fn to_string(
        &self,
        endian: Endian,
        start_byte: u8,
        start_mask: u8,
        end_byte: u8,
        end_mask: u8,
        factor: f64,
        offset: f64
    ) -> String {
        let id: String;

        if self.id <= 0x7FF {
            id = format!("     {:03X}", self.id);
        } else {
            id = format!("{:08X}", self.id);
        }

        let mut data: String = String::with_capacity(32);
        let mut value_bytes: Vec<u8> = Vec::new();
        let data_len: u8 = std::cmp::min(self.data.len() as u8 - 1, end_byte);
    
        for i in start_byte..=data_len {
            let mut byte = self.data[i as usize];
            if i == start_byte {
                byte &= start_mask;
            }
            if i == data_len {
                byte &= end_mask;
            }
            value_bytes.push(byte);
            data += &format!("{:02X}", byte);
            if i < data_len {
                data += " ";
            }
        }

        let uvalue: u64;
        let svalue: i64;

        for _ in 0..(7-(data_len-start_byte)) {
            match endian {
                Endian::Little => value_bytes.push(0x00),
                Endian::Big => value_bytes.insert(0, 0x00),
            }
        }

        match endian {
            Endian::Little => {
                uvalue = u64::from_le_bytes(value_bytes.clone().try_into().unwrap());
                svalue = i64::from_le_bytes(value_bytes.clone().try_into().unwrap());
            },
            Endian::Big => {
                uvalue = u64::from_be_bytes(value_bytes.clone().try_into().unwrap());
                svalue = i64::from_be_bytes(value_bytes.clone().try_into().unwrap());
            },
        }

        format!(
            "{:.6} | {} | {} | {} | {:.6} | {:.6}",
            self.timestamp, self.device, id, data, (uvalue as f64 * factor) + offset, (svalue as f64 * factor) + offset
        )
    }
}

#[derive(Debug)]
struct Context {
    frames: Vec<CanFrame>,
}

impl Context {
    fn new() -> Self {
        Self {
            frames: Vec::with_capacity(100000),
        }
    }
}

fn main() {
    let args = Args::parse();
    let mut context = Context::new();

    for dump_file in args.dump_file.iter() {
        println!("Processing file {}...", dump_file.display());
        let reader = BufReader::new(File::open(dump_file).unwrap());
        for line in reader.lines().map(|l| l.unwrap()) {
            let (ts, dev, mut id, data) =
                scanf!(line, "({f64}) {String} {String}#{String}").unwrap();
            // hex crate does not like odd hex strings, 11-bit can ids need to prepend 0
            id.insert_str(0, "0".repeat(8 - id.len()).as_str());
            context.frames.push(CanFrame::new(ts, dev, id, data));
        }
    }

    if args.data {
        let frames = context.frames.iter().filter(|frame| match args.id {
            Some(id) => id == frame.id,
            None => true,
        });

        for frame in frames {
            println!(
                "{}",
                frame.to_string(
                    args.endian,
                    args.start_byte,
                    args.start_mask,
                    args.end_byte,
                    args.end_mask,
                    args.factor,
                    args.offset
                )
            );
        }
    }
}

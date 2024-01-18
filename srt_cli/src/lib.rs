use std::io;
use std::io::IsTerminal as _;

use clap::Parser;

pub const SRT_LOG_LEVEL_NONE: u8 = 0;
pub const SRT_LOG_LEVEL_VERBOSE: u8 = 1;

#[repr(C)]
pub struct SRTContext {
    log_level: u8,
}

#[derive(Parser, Debug)]
struct Args {
    #[arg(short, long, action)]
    verbose: bool,
}

extern "C" {
    pub fn spiff_process_start(ctx: &SRTContext);
}

#[cfg(not(test))]
#[no_mangle]
pub extern "C" fn main() {
    let args = Args::parse();
    let log_level = if args.verbose {
        SRT_LOG_LEVEL_VERBOSE
    } else {
        SRT_LOG_LEVEL_NONE
    };

    let ctx = SRTContext { log_level };

    unsafe {
        spiff_process_start(&ctx);
    }
}

#[no_mangle]
pub extern "C" fn srt_will_run_element(
    ctx: &SRTContext,
    process_id: *const u8,
    process_id_len: usize,
    element_id: *const u8,
    element_id_len: usize,
) {
    if ctx.log_level & SRT_LOG_LEVEL_VERBOSE == 0 {
        return;
    }

    let process_id = unsafe { as_str(process_id, process_id_len) };
    let element_id = unsafe { as_str(element_id, element_id_len) };

    println!("will run {}_{}", process_id, element_id);
}

#[no_mangle]
pub extern "C" fn srt_did_run_element(
    ctx: &SRTContext,
    process_id: *const u8,
    process_id_len: usize,
    element_id: *const u8,
    element_id_len: usize,
) {
    if ctx.log_level & SRT_LOG_LEVEL_VERBOSE == 0 {
        return;
    }

    let process_id = unsafe { as_str(process_id, process_id_len) };
    let element_id = unsafe { as_str(element_id, element_id_len) };

    println!("did run {}_{}", process_id, element_id);
}

#[no_mangle]
pub extern "C" fn srt_handle_manual_task(
    _ctx: &SRTContext,
    element_id: *const u8,
    element_id_len: usize,
    instructions: *const u8,
    instructions_len: usize,
) {
    let element_id = unsafe { as_str(element_id, element_id_len) };

    println!("Manual Task {}", element_id);

    if instructions_len > 0 {
        let instructions = unsafe { as_str(instructions, instructions_len) };

        println!("  * {}", instructions);
    }

    if io::stdout().is_terminal() {
        println!("\nPress enter to continue:");
	let mut input = String::new();
	let _ = io::stdin().read_line(&mut input);
    } else {
        println!("\nNot in interactive mode, automatically completing manual task...");
    }
}

unsafe fn as_str(data: *const u8, len: usize) -> &'static str {
    if let Ok(str) = std::str::from_utf8(std::slice::from_raw_parts(data, len)) {
        return str;
    }

    panic!("Invalid string");
}

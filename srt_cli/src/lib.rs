use std::io;
use std::io::IsTerminal as _;
use std::slice::from_raw_parts;
use std::str::from_utf8;

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
    pub fn spiff_process_start(ctx: &SRTContext) -> i32;
}

#[cfg(not(test))]
#[no_mangle]
pub extern "C" fn main() -> i32 {
    let args = Args::parse();
    let log_level = if args.verbose {
        SRT_LOG_LEVEL_VERBOSE
    } else {
        SRT_LOG_LEVEL_NONE
    };

    let ctx = SRTContext { log_level };

    unsafe { spiff_process_start(&ctx) }
}

/// # Safety
///
/// This function expects the caller to provide valid strings and lengths.
#[no_mangle]
pub unsafe extern "C" fn srt_will_run_element(
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

    match (process_id, element_id) {
        (None, _) => eprintln!("Invalid string passed as `process_id`"),
        (_, None) => eprintln!("Invalid string passed as `element_id`"),
        (Some(process_id), Some(element_id)) => println!("will run {}_{}", process_id, element_id),
    }
}

/// # Safety
///
/// This function expects the caller to provide valid strings and lengths.
#[no_mangle]
pub unsafe extern "C" fn srt_did_run_element(
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

    match (process_id, element_id) {
        (None, _) => eprintln!("Invalid string passed as `process_id`"),
        (_, None) => eprintln!("Invalid string passed as `element_id`"),
        (Some(process_id), Some(element_id)) => println!("did run {}_{}", process_id, element_id),
    }
}

/// # Safety
///
/// This function expects the caller to provide valid strings and lengths.
#[no_mangle]
pub unsafe extern "C" fn srt_handle_manual_task(
    _ctx: &SRTContext,
    element_id: *const u8,
    element_id_len: usize,
    instructions: *const u8,
    instructions_len: usize,
) {
    let element_id = unsafe { as_str(element_id, element_id_len) };

    if let Some(element_id) = element_id {
        println!("Manual Task {}", element_id);
    } else {
        eprintln!("Invalid string passed as `element_id`");
    }

    if instructions_len > 0 {
        if let Some(instructions) = unsafe { as_str(instructions, instructions_len) } {
            println!("  * {}", instructions);
        } else {
            eprintln!("Invalid string passed as `instructions`");
        }
    }

    if io::stdin().is_terminal() {
        println!("\nPress enter to continue:");
        let mut input = String::new();
        let _ = io::stdin().read_line(&mut input);
    } else {
        println!("\nNot in interactive mode, automatically completing manual task...");
    }
}

unsafe fn as_str(data: *const u8, len: usize) -> Option<&'static str> {
    data.as_ref()
        .and_then(|d| from_utf8(from_raw_parts(d, len)).ok())
}

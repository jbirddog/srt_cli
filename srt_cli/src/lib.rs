use std::ffi::c_void;
use std::io;
use std::io::IsTerminal as _;
use std::slice::from_raw_parts;
use std::str::from_utf8;

use clap::Parser;

mod context;
mod state;

use context::Context;

const SUCCESS: i32 = 0;
const INVALID_STRING: i32 = 1;

#[derive(Parser, Debug)]
struct Args {
    #[arg(short, long, action)]
    verbose: bool,
}

extern "C" {
    pub fn spiff_process_start(ctx: *mut c_void) -> i32;
}

macro_rules! as_valid_str {
    ($data:ident, $len:ident) => {{
        let Some($data) = (unsafe { as_str($data, $len) }) else {
            log::error!("Invalid string passed as `{:?}`", stringify!($data));
            return INVALID_STRING;
        };

        $data
    }};
}

#[cfg(not(test))]
#[no_mangle]
pub extern "C" fn main() -> i32 {
    let args = Args::parse();
    configure_logger(&args);

    let mut ctx = Context::default();

    unsafe { spiff_process_start(&mut ctx as *mut _ as *mut c_void) }
}

#[cfg(not(test))]
fn configure_logger(args: &Args) {
    let default_log_level = if args.verbose { "trace" } else { "error" };
    let env = env_logger::Env::default().filter_or("SRT_LOG_LEVEL", default_log_level);

    env_logger::init_from_env(env);
}

/// # Safety
///
/// This function expects the caller to provide valid strings and lengths.
#[no_mangle]
pub unsafe extern "C" fn srt_will_run_element(
    _ctx: *mut c_void,
    process_id: *const u8,
    process_id_len: usize,
    element_id: *const u8,
    element_id_len: usize,
) -> i32 {
    let process_id = as_valid_str!(process_id, process_id_len);
    let element_id = as_valid_str!(element_id, element_id_len);

    log::info!("will run {}_{}", process_id, element_id);

    SUCCESS
}

/// # Safety
///
/// This function expects the caller to provide valid strings and lengths.
#[no_mangle]
pub unsafe extern "C" fn srt_did_run_element(
    _ctx: *mut c_void,
    process_id: *const u8,
    process_id_len: usize,
    element_id: *const u8,
    element_id_len: usize,
) -> i32 {
    let process_id = as_valid_str!(process_id, process_id_len);
    let element_id = as_valid_str!(element_id, element_id_len);

    log::info!("did run {}_{}", process_id, element_id);

    SUCCESS
}

/// # Safety
///
/// This function expects the caller to provide valid strings and lengths.
#[no_mangle]
pub unsafe extern "C" fn srt_handle_manual_task(
    _ctx: *mut c_void,
    element_id: *const u8,
    element_id_len: usize,
    instructions: *const u8,
    instructions_len: usize,
) -> i32 {
    let element_id = as_valid_str!(element_id, element_id_len);
    println!("Manual Task {}", element_id);

    if instructions_len > 0 {
        let instructions = as_valid_str!(instructions, instructions_len);
        println!("  * {}", instructions);
    }

    if io::stdin().is_terminal() {
        println!("\nPress enter to continue:");
        let mut input = String::new();
        let _ = io::stdin().read_line(&mut input);
    } else {
        log::info!("Not in interactive mode, automatically completing manual task...");
    }

    SUCCESS
}

/// # Safety
///
/// This function expects the caller to provide valid strings and lengths.
#[no_mangle]
pub unsafe extern "C" fn srt_task_data_set_int64(
    ctx: *mut c_void,
    key: *const u8,
    key_len: usize,
    value: i64,
) -> i32 {
    let ctx: &mut Context = unsafe { &mut *(ctx as *mut Context) };
    let key = as_valid_str!(key, key_len);

    ctx.task_data.set_i64(key, value);

    log::trace!("Set task_data var '{}: i64 = {}'", key, value);

    SUCCESS
}

unsafe fn as_str(data: *const u8, len: usize) -> Option<&'static str> {
    data.as_ref()
        .and_then(|d| from_utf8(from_raw_parts(d, len)).ok())
}

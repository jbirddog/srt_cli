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
const UNKNOWN_TASK_DATA_KEY: i32 = 2;
const INVALID_POINTER: i32 = 3;

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

/// # Safety
///
/// This function expects the caller to provide valid strings and lengths.
#[no_mangle]
pub unsafe extern "C" fn srt_task_data_get_int64(
    ctx: *mut c_void,
    key: *const u8,
    key_len: usize,
    value: *mut i64,
) -> i32 {
    let ctx: &mut Context = unsafe { &mut *(ctx as *mut Context) };
    let key = as_valid_str!(key, key_len);

    if value.as_ref().is_none() {
        log::error!("Invalid pointer provided for 'value'");
        return INVALID_POINTER;
    }

    let Some(task_data_value) = ctx.task_data.get_i64(key) else {
        log::error!("No i64 task data value for key '{}'", key);
        return UNKNOWN_TASK_DATA_KEY;
    };

    *value = task_data_value;

    log::trace!("Get task_data var '{}: i64 = {}'", key, *value);

    SUCCESS
}

/// # Safety
///
/// This function expects the caller to provide valid strings and lengths.
#[no_mangle]
pub unsafe extern "C" fn srt_task_data_delete(
    ctx: *mut c_void,
    key: *const u8,
    key_len: usize,
) -> i32 {
    let ctx: &mut Context = unsafe { &mut *(ctx as *mut Context) };
    let key = as_valid_str!(key, key_len);

    if ctx.task_data.delete(key).is_none() {
        log::error!("No task data value to delete for key '{}'", key);
        return UNKNOWN_TASK_DATA_KEY;
    };

    log::trace!("Delete task_data var '{}'", key);

    SUCCESS
}

unsafe fn as_str(data: *const u8, len: usize) -> Option<&'static str> {
    data.as_ref()
        .and_then(|d| from_utf8(from_raw_parts(d, len)).ok())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn set_get_task_data_int64() {
        let mut ctx = Context::default();
        let key = "some_var";
        let key_len = key.len();
        let key = key.as_bytes().as_ptr() as *const u8;
        let expected = 77;

        let result = unsafe {
            srt_task_data_set_int64(&mut ctx as *mut _ as *mut c_void, key, key_len, expected)
        };

        assert_eq!(result, SUCCESS);

        let mut value: i64 = 0;
        let result = unsafe {
            srt_task_data_get_int64(&mut ctx as *mut _ as *mut c_void, key, key_len, &mut value)
        };

        assert_eq!(result, SUCCESS);
        assert_eq!(value, expected);
    }

    #[test]
    fn set_delete_task_data_int64() {
        let mut ctx = Context::default();
        let key = "some_var";
        let key_len = key.len();
        let key = key.as_bytes().as_ptr() as *const u8;

        let result =
            unsafe { srt_task_data_set_int64(&mut ctx as *mut _ as *mut c_void, key, key_len, 55) };

        assert_eq!(result, SUCCESS);

        let result =
            unsafe { srt_task_data_delete(&mut ctx as *mut _ as *mut c_void, key, key_len) };

        assert_eq!(result, SUCCESS);

        let mut value: i64 = 0;
        let result = unsafe {
            srt_task_data_get_int64(&mut ctx as *mut _ as *mut c_void, key, key_len, &mut value)
        };

        assert_eq!(result, UNKNOWN_TASK_DATA_KEY);
    }

    #[test]
    fn get_unset_task_data_int64() {
        let mut ctx = Context::default();
        let key = "some_var";
        let key_len = key.len();
        let key = key.as_bytes().as_ptr() as *const u8;

        let mut value: i64 = 300;
        let result = unsafe {
            srt_task_data_get_int64(&mut ctx as *mut _ as *mut c_void, key, key_len, &mut value)
        };

        assert_eq!(result, UNKNOWN_TASK_DATA_KEY);
        assert_eq!(value, 300);
    }

    #[test]
    fn delete_unset_task_data_int64() {
        let mut ctx = Context::default();
        let key = "some_var";
        let key_len = key.len();
        let key = key.as_bytes().as_ptr() as *const u8;

        let result =
            unsafe { srt_task_data_delete(&mut ctx as *mut _ as *mut c_void, key, key_len) };

        assert_eq!(result, UNKNOWN_TASK_DATA_KEY);
    }
}

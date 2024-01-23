use std::collections::HashMap;

use serde_json::Value;

#[derive(Default)]
pub struct State {
    state: HashMap<String, Value>,
}

impl State {
    pub fn set_i64(&mut self, key: &str, value: i64) -> bool {
        let value = Value::from(value);

        self.state.insert(key.to_string(), value).is_some()
    }
}

use std::collections::HashMap;

use serde_json::Value;

#[derive(Default)]
pub struct State {
    state: HashMap<String, Value>,
}

impl State {
    pub fn get_i64(&self, key: &str) -> Option<i64> {
        self.state.get(key).and_then(|v| v.as_i64())
    }

    pub fn set_i64(&mut self, key: &str, value: i64) {
        self.state.insert(key.to_string(), Value::from(value));
    }

    pub fn delete(&mut self, key: &str) -> Option<Value> {
        self.state.remove(key)
    }
}

extern crate libc;

extern {
    pub fn dump_layout();
}

#[cfg(test)]
mod tests {
    use crate::dump_layout;

    #[test]
    fn it_works() {
        unsafe {
            dump_layout();
        }
    }
}

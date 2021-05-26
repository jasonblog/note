#![feature(specialization)]

#[macro_use]
extern crate pyo3;
extern crate rand;
extern crate rayon;

use pyo3::prelude::*;
use rayon::prelude::*;

#[pyfunction]
fn hello() -> PyResult<()> {
    println!("Hello from Rust");
    Ok(())
}

const TIMES: i32 = 1000_0000;

#[pyfunction]
fn calculate_pi() -> PyResult<f64> {
    let mut hit = 0;
    for _ in 0..TIMES {
        let x = rand::random::<f64>();
        let y = rand::random::<f64>();
        if x * x + y * y <= 1.0 {
            hit += 1;
        }
    }
    Ok(f64::from(hit * 4) / f64::from(TIMES))
}

#[pyfunction]
fn calculate_pi_parallel(py: Python) -> PyResult<f64> {
    let hit: i32 = py.allow_threads(|| {
        (0..TIMES)
            .into_par_iter()
            .map(|_| {
                let x = rand::random::<f64>();
                let y = rand::random::<f64>();
                if x * x + y * y <= 1.0 {
                    1
                } else {
                    0
                }
            })
            .sum()
    });
    Ok(f64::from(hit * 4) / f64::from(TIMES))
}

#[pymodinit]
fn binding(_py: Python, m: &PyModule) -> PyResult<()> {
    m.add_function(wrap_function!(hello))?;
    m.add_function(wrap_function!(calculate_pi))?;
    m.add_function(wrap_function!(calculate_pi_parallel))?;

    Ok(())
}

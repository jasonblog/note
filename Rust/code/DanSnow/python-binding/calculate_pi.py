from random import random

# You need to copy `target/release/libbinding.so` to `binding.so`
import binding
import timeit

TIMES = 1000_0000


def calculate_pi():
    hit = 0

    for _i in range(0, TIMES):
        x = random()
        y = random()
        if x * x + y * y <= 1.0:
            hit += 1
    return hit * 4 / TIMES


if __name__ == "__main__":
    print("Python:", timeit.timeit("calculate_pi()", number=3, globals=globals()))
    print("Rust:", timeit.timeit("binding.calculate_pi()", number=3, globals=globals()))
    print(
        "Rust parallel:",
        timeit.timeit("binding.calculate_pi_parallel()", number=3, globals=globals()),
    )


# The Optic Programming Language

Optic is a multi-paradigm programming language designed to combine the syntax simplicity and readability of high-level languages like Python with the safety of Rust and the power, performance, and low-level control of C and C++. 

It acts as a high-level abstraction layer over C/C++, providing direct hardware interaction without sacrificing performance or safety.

## Key Features

- **High-Level Abstraction over C/C++:** Built on top of the C/C++ ecosystem to achieve execution speeds and efficiency practically identical to native code.
- **Clean and Simple Syntax:** Heavily inspired by Python's readability, removing visual clutter and unnecessary boilerplate code.
- **Direct Interoperability (Bridges):** Features native communication bridges to easily wrap and integrate existing C/C++ libraries (`OpticLib`) similar to Python or Lua extensions.
- **Flexible Execution (VM + JIT):** Programs primarily run in RAM via an optimized Virtual Machine (VM) engine supporting Just-In-Time (JIT) compilation, with the optional capability to compile directly into standalone disk binaries.
- **Hybrid Typing System:** Implements static typing to ensure memory safety and compile-time optimization, with potential future support for dynamic typing where flexibility is needed.
- **Granular Memory Control:** Features built-in memory management and a Garbage Collector (GC) that can be controlled either explicitly within the source code or via the Command Line Interface (CLI).
- **Low-Level Hardware Access:** Facilitates systems programming through clear abstractions and classes designed to manipulate physical hardware directly, eliminating heavy software overhead.

## Syntax Example

### 1. Hello World

```optic
function main() {
    print("Hello World")
}
```

### 2. Fibonacci Series

```optic
function fibonacci(n: int) -> int {
    if n <= 1 {
        return n
    }
    return fibonacci(n - 1) + fibonacci(n - 2)
}
```

### 3. Binary Search

```optic
function binary_search(&arr: array[int], x: int) -> int {
    int low = 0
    int high = arr.size() - 1

    while (low <= high) {
        mid = low + (high - low) / 2

        if arr[mid] == x:
            return mid

        if arr[mid] < x:
            low = mid + 1

        else:
            high = mid - 1
    }

    return -1
}
```

## Future Roadmap

The Optic ecosystem is planned to expand into cutting-edge fields through specialized language extensions:
- **QOptic:** Dedicated extension to support quantum computing paradigms.
- **GOptic:** A powerful graphics framework optimized for high-performance rendering and visual computing.

## README: Not completed yet

Optic is an open-source code programming language in an early stage of development that is being designed by an inexperienced 17 years old programmer.
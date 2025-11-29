# The CStar Project

A newly developed programming language that combines features of C++, Java, and C#.

## Naming

- **Type it as:** C*, CStar
- **Call it as:** CStar, CAsterisk
- **File extensions:** *.cstar, *.ca, *.c*

## Overview

CStar is a superset of C/C++, Java, and C# designed to provide a modern, expressive syntax while maintaining compatibility with existing C++ ecosystems. The language is currently in **Beta development (version 1.5.9)**.

### How It Works

The CStar compiler transpiles CStar source code to C++ and then compiles the resulting C++ code using your system's C++ compiler (g++, clang, MSVC, etc.).

**Pipeline:**
```
CStar source (.cstar) → Transpile to C++ (.cpp) → Compile to executable
```

## Getting Started

### Installation

1. Clone or download the CStar repository.
2. Ensure you have a C++ compiler installed (GCC, Clang, or MSVC).
3. Build the CStar compiler:
   ```bash
   g++ -std=gnu++23 cstcompiler.cpp -o cstarc
   ```

### Hello, World!

Create a file named `hello.cstar`:

```cpp
#include <cstar.h>

using int main() {
    cpp20::println("Hello, World!");
    return 0;
}
```

Compile and run:

```bash
cstarc hello.cstar -c
```

Expected output:
```
Hello, World!
```

## Command-Line Options

| Option | Description |
|--------|-------------|
| `-c` | Compile the transpiled C++ code to an executable (required for running) |
| `-s` | Silent mode: suppress all compiler output messages |
| `--version` or `-v` | Display compiler version and copyright information |
| `--lstdcst` | Invoke the linker after compilation |
| `--lstdcst-v` | Display linker version information |

### Examples

```bash
# Compile and run
cstarc myprogram.cstar -c

# Compile silently
cstarc myprogram.cstar -c -s

# Check version
cstarc --version

# Compile with linker
cstarc myprogram.cstar -c --lstdcst
```

## Language Features

### Type System

CStar provides familiar type keywords and modern type aliases:

```cpp
using int main() {
    var x = 42;           // auto-deduced type
    def add(int a, int b) { return a + b; }  // function definition
    
    return 0;
}
```

### Console I/O

Use the `CONSOLE` class for standard I/O:

```cpp
#include <cstar.h>

using int main() {
    Console.WriteLine("Enter your name:");
    std::string name;
    Console.ReadLine(name);
    Console.WriteLine("Hello, " + name + "!");
    
    return 0;
}
```

### POSIX I/O

For low-level I/O, use the `UNIX` class (posixprintf / posixscanf):

```cpp
using int main() {
    unix.posixprintf(false, "#s", "Enter a number: ");
    int num = 0;
    unix.posixscanf("#d", num);
    unix.posixprintf(true, "#d", num * 2);
    
    return 0;
}
```

### Keyboard Input

Use the `keyboard` class for blocking and non-blocking key detection:

```cpp
using int main() {
    Console.WriteLine("Press 'q' to quit...");
    while (true) {
        if (kb.retchkbd()) {  // non-blocking check
            int ch = kb.reckbd('\0', true);  // blocking read
            if (ch == 'q') break;
        }
    }
    
    return 0;
}
```

### Sound & Music

Play sounds and music files (requires Python and pygame):

```cpp
#include <ext/sound.h>

using int main() {
    play_sound("beep.wav");
    play_music("background.ogg");
    
    return 0;
}
```

### CMP Build Files

CStar supports `.cmp` (CStar Make) files for project configuration:

```cmp
(STRING)TARGET = myapp.cstar
(STRING)OUTPUT = myapp.exe

def compile:
    console("cstarc $(TARGET) -c")

def run:
    #ifdef _WIN32
        console("$(OUTPUT)")
    #else
        console("./$(OUTPUT)")
    #endif

__init__:
    compile, run
```

Process with `maketrans`:
```bash
maketrans CStarMake.cmp
```

## TextMate Grammar

A TextMate grammar is provided for syntax highlighting in VS Code and other editors.

**Installation:**
1. Copy `cstar-cmp.tmLanguage.json` to `~/.vscode/extensions/cstar-cmp-0.0.1/syntaxes/`
2. Add to VS Code settings:
   ```json
   {
     "files.associations": { "*.cmp": "cstar-cmp" }
   }
   ```

## Architecture

- **cstcompiler.cpp** — Main transpiler: parses CStar, generates C++
- **maketrans.cpp** — Build file processor for .cmp files
- **i686runner.cpp** — Executor for i686 bytecode files
- **include/ext/stdcstar.h** — Core CStar standard library
- **include/ext/sound.h** — Sound/music playback support
- **include/stdcstio** — Keyboard and console I/O utilities
- **sound_play.py** — Python helper for audio playback

## Platform Support

- **Windows:** Full support (MSVC, GCC MinGW, Clang)
- **Linux/Unix:** Full support (GCC, Clang)
- **macOS:** Full support (Clang)

## Contributing

Contributions are welcome! Please ensure:
- Code follows the existing style (C++17+)
- New features include appropriate test cases
- Documentation is updated

## License

Licensed under the **MIT License**. See LICENSE file for details.

**Copyright © November 2025 Hoang Viet. All rights reserved.**

## Version History

- **CStar25**: The Beta Stage of the Language
- **CStar26**: The Current Release version

# Notes
- The language is mainly for Windows, so please wait for the update to the Unix-systems compilers.
- Remember to add an environment variable to the PATH.

---

For issues, questions, or feedback, please open an issue on the project repository.

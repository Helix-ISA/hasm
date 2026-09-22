# HASM

HASM is an assembler for the Helix ISA.

It translates Helix assembly files (```.hxs```) into machine code that can be
used by the Helix toolchain, processors, and emulators.

> [!WARNING]
> HASM is still in development and may receive several patches each week.
> Features and behavior may change as the Helix ISA evolves.

## Features
* Helix assembly lexer and parser
* Symbol resolution
* Lexer debugging
* Parser debugging
* Symbol debugging

## Building

### Requirements
* A C compiler with Clang support
* GNU Make
* Nucleus repository or manually install ```flagparser```

HASM is currently built using ```clang``` and the provided ```Makefile```.

Clone the repository:

``` bash
git clone git@github.com:Helix-ISA/hasm.git
cd hasm
```

Build:

``` bash
make
```

The executable with be built in:
``` bash
bin/hasm
```

To delete build artifacts run:
``` bash
make clean
```

## Usage

``` bash
Usage: ./bin/hasm [OPTIONS] [ARGS...]

Helix assembler (0.1.0)

Options:
  -v, --verbose           Enable verbose mode
  -o, --output FILE       Specifies output file [default: out]
  -d, --disassemble FILE  Specify file to disassemble
  --lexer-debug           Enable lexer debug output
  --parser-debug          Enable parser debug output
  --symbol-debug          Enable symbol debug output
```

## Debugging

HASM includes several debugging modes that are useful when developing the
assembler and the Helix ISA itself

> [!WARNING]
> Debugging is currently disabled

## Model

![model](assets/model.png)

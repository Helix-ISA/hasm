# HASM

HASM is an assembler for the Helix ISA.

It translates Helix assembly files (hxs) into machine code that can be
used by the Helix toolchain, processors, and emulators.

> [!WARNING]
> HASM is still in development and will have several patches weekly

## Features
* Helix assembly lexer and parser
* Symbol resolution
* Lexer debugging
* Parser debugging
* Symbol debugging
* Strict compilation with Clang warnings treated as errors
* No external dependencies

## Building

### Requirements
* A C compiler with Clang support
* GNU Make

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

The basic invocation for HASM is:

``` bash
./bin/hasm <source.hxs>
```

This will produce out.hxo

## Debugging

HASM includes several debugging modes that are useful when developing the
assembler and the Helix ISA itself

> [!WARNING]
> Debugging will be removed from release builds

## Model

![model](assets/model.png)

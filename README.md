# Ammonium

Ammonium is a molecular dynamics (MD) simulation software. It handles electrostatic, Van der Waals, and covalent interactions.

## Compilation

Compilation is achieved using the provided makefile. No additional software is required.

## Usage

`ammonium -i input_file -o output_file [options]`

Options:

- `--in` Required. Specifies the path to the input file (`.nh4` format).

- `--out` Required. Specifies the output file (`.xyz` format)

- `--time t` Specifies the simulation duration, in picoseconds

- `--dt t` Specifies the timestep, in picoseconds

- `--temp` Specifies the thermodynamic temperature of the system, in Kelvin

## Introduction

Ammonium will simulate its provided system in a classical way. Particles are charged point-masses, covalent bonds are springs, and Van der Waals interactions are approximated with the Lennard-Jones potential. After each timestep, each particle's file will be appended with the current particle's state, which consists of its vector components and magnitude. The files are formatted in `.xyz` and `.csv`, allowing easy data processing.

The simulation's initial state is read from a `.nh4` file. It is a format similar to `xyz`, but with extra bond information.

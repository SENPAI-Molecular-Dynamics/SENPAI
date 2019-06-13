# Ammonium

Ammonium is a molecular dynamics (MD) simulation software. It handles electrostatic, Van der Waals, and covalent interactions. I'm not trying to change the world here, I'm just having a good time writing software.

## Compilation

Compilation is achieved using the provided makefile. No additional software is required.

## Usage

`ammonium -i input_file -o output_file [options]`

Options:

- `--in` Required. Specifies the path to the input file (`.nh4` format)

- `--out` Required. Specifies the output file (`.xyz` format)

- `--numerical` Configures the simulation to use numerical differentiation of the potential energy (defaults to analytical solving)

- `--time t` Specifies the simulation duration (ps) (defaults to `1ns`)

- `--dt t` Specifies the timestep (fs) (defaults to `1 fs`)

- `--temp` Specifies the thermodynamic temperature of the system (K) (defaults to `273.15 K`)

- `--pressure` Specifies the pressure of the system (Pa) (defaults to `100 kPa`)

- `--mol` Specifies the number of molecules to be inserted in the simulation

## Example use

`./ammonium.bin --in examples/benzene.nh4 --out render.xyz --time 1000 --dt 2 --mol 512`

This will tell ammonium to simulate 512 benzene molecules at STP for one nanosecond using a two femtosecond timestep. The rendered simulation will be saved in `render.xyz` for further analysis.

## Introduction

Ammonium will simulate its provided system in a classical way. Particles are charged point-masses, covalent bonds are springs, and Van der Waals interactions are approximated with the Lennard-Jones potential. After each timestep, each particle's file will be appended with a file formatted in `.xyz`.

The simulation's initial state is read from a `.nh4` file. It is a format similar to `.xyz`, but with extra bond information.

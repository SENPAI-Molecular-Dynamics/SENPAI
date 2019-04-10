# Ammonium

Ammonium is a MD simulator based on classical approximations aiming to simulate large systems on devices ranging from a laptop to a peta-scale parallel computing cluster.

#Compilation

Ammonium requires the C standard library. No additional software is required. Compilation is achieved using the provided makefile.

#Usage

`ammonium -i input_file -o output_file [options]`

Options:

- `-i | --in` Required. Specifies the path to the input file.

- `-o | --out` Required. Specifies the name prefix for the output file. using `-o out` with two particles would tell Ammonium to create `out0.csv` and `out1.csv`.

- `--elec nb` Specifies the value for the electromagnetic coupling constant, in SI units

- `--time t` Specifies the simulation duration, in picoseconds

- `--dt t` Specifies the timestep, in picoseconds

#Introduction

Ammonium will simulate its provided system in a classical way. Particles are charged point-masses, covalent bonds are springs, and Van der Waals interactions are approximated with the Lennard-Jones potential. After each timestep, each particle's file will be appended with the current particle's state, which consists of its vector components and magnitude. The files are formatted in `.csv` format, allowing data processing and plotting either via UNIX commands or dedicated software.

The simulation's initial state is read from a `.nh4` file, which is just a posh renaming of the `.csv` format, allowing again the use of dedicated software to initiate a simulation.

Ammonium generates tremendous amounts of data, and will put even the most powerful computing systems on their knees.

# Ammonium

Ammonium is a MD simulator based on classical approximations aiming to simulate large systems on devices ranging from a laptop to a peta-scale parallel computing cluster.

#Compilation

Ammonium requires the C standard library. No additional software is required. Compilation is achieved using the provided makefile.

#Usage

`ammonium -i input_file -o output_file [options]`

Options:

- `-i | --in` Required. Specifies the path to the input file.

- `-o | --out` Required. Specifies the name prefix for the output file. using `-o out` with two particles would tell Ammonium to create `out0.csv` and `out1.csv`.

- `--grav nb` Specifies the value for the gravitational coupling constant, in SI units

- `--elec nb` Specifies the value for the electromagnetic coupling constant, in SI units

- `--time t` Specifies the simulation duration, in picoseconds

- `--dt t` Specifies the timestep, in picoseconds

# Ammonium

Ammonium is a terminal-based molecular simulation software for large-scale workloads. Reading an initial state from a file, Ammonium outputs a `.csv` log for every simulated particle, allowing the user to analyse the behaviour of a system in extreme details.

#Compilation

Ammonium requires the C standard library. No additional software is required. Compilation is achieved using the provided makefile.

#Usage

`ammonium -i input_file -o output_file [options]`

Options:

`-i | --in` Required. Specifies the path to the input file.
`-o | --out` Required. Specifies the name prefix for the output file. using `-o out` with two particles would tell Ammonium to create `out0.csv` and `out1.csv`.
`--grav nb` Specifies the value for the gravitational coupling constant. (default: SI value)
`--elec nb` Specifies the value for the electromagnetic coupling constant. (default: SI value)
`--time t` Specifies the time at which the simulation stops. (default: 1 nanosecond)
`--dt t` Specifies the numerical integration constant (default: 1 picosecond)

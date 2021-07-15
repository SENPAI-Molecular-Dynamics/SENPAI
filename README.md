# SENPAI - Simplified Evolutive N-body Processing and Analytics for Integration

SENPAI is a molecular dynamics (MD) simulation software aimed at simulating organic systems. It simulates the following forces:

- Covalence (as undamped Newtonian harmonic oscillators)

- Bond angles (as angular undamped Newtonian harmonic oscillators)

- Electrostatics (treating atoms as point charges)

- Van der Waals (by analytically solving or numerically differentiating the Lennard-Jones potential)

# Cool features

- **OpenMP:** simulator go brrrrrrr but like very fast wow

- **C99 compliance:** SENPAI is 100% compliant with the C99 standard. **SENPAI could run on a Roomba**.

- **Efficient computing model:** the ONICHaN (Optimized Numerical Integration, Classical Harmonics, and N-body) model was tailored for SENPAI over months of work. Inspired by the AMBER family of force fields, it allows SENPAI to compute forces with unprecedented efficiency.

- **No potential differentiation:** Traditional MD simulators compute a particle's total potential and numerically differentiate it to compute the applied force. SENPAI analytically solves for the force vector, resulting in an extreme speed-up.

- **Tunable thermodynamics:** Efficiently write shell scripts on your host system to run simulations with various thermodynamical settings and analyse the impact of the system's surroundings on its evolution.

- **Render as XYZ:** Keep it simple. Simulations are rendered using the XYZ format, and can be directly visualized with software like VMD.

- **Integrated potential reduction:** Easily mitigate the issue of "garbage in, garbage out"! If told to do so, SENPAI will reduce the potential energy of the system to a user-selected target value before simulating.

## Installation

The first step consists of configuring SENPAI for your machine and use case. This is done by editing the constants in `headers.config.h`.

On UNIX systems, compilation is achieved using the provided makefile. No additional software is required.

`$ git clone https://github.com/Garuda1/senpai`

`$ cd senpai`

`$ make`

The installation process requires the C standard library, a C99-compliant compiler, and GNU Make.

## Usage

`senpai --in substrate_file --solvent solvent_file --out output_file [options]`

Options:

- `--in` **Required.** Specifies the path to the substrate to simulate

- `--solvent` **Required.** Specified the path to the solvent file

- `--out` **Required.** Specifies the output file

- `--numerical` Configures the simulation to use numerical differentiation of the potential energy (defaults to analytical solving)

- `--time t` Specifies the simulation duration (ns) (defaults to `1ns`)

- `--dt t` Specifies the timestep (fs) (defaults to `1 fs`)

- `--temp` Specifies the thermodynamic temperature of the system (K) (defaults to `298.15 K`)

- `--pressure` Specifies the pressure of the system (mbar) (defaults to `1 atm`)

- `--density` Specifies the density of the system (g.cm-3) (defaults to `1 g.cm-3`)

- `--copy` Specifies how many copies of the loaded system are to be inserted in the simulation

- `--frameskip` Sets a frameskip to reduce I/O usage. Disabled by default

- `--reduce_potential` Reduces the potential energy to the provided value (pJ) or lower (defaults to 10 pJ)

## Example use

`$ ./senpai.bin --in examples/DES.mol --solvent examples/water.mol --out render.xyz`

This will tell SENPAI to simulate the contents of `examples/DES.mol` in STP water for one nanosecond using a one femtosecond timestep. The rendered simulation will be saved in `render.xyz` for further analysis.

`$ ./senpai.bin --in examples/water.mol examples/void.mol --out water_render.xyz --copy 5000 --reduce_potential 100 --temp 300`

This will have SENPAI load 1000 water molecules from `examples/water.mol`, arrange them so that the total potential energy is lower than 100 pJ, and finally simulate the system at 300 Kelvin for 1 ns, saving the render in `water_render.xyz`.

# A complete list of functions and structures is available in the wiki, so as to make contributions easier. Documentation on the way.

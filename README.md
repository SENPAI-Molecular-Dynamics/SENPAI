# SENPAI - Simplified Evolutive N-body Processing and Analytics for Integration

SENPAI is a molecular dynamics (MD) simulation software aimed at simulating organic systems. It handles  the following interractions:

- Covalence (as undamped Newtonian harmonic oscillators)

- Bond angles (by enforcing atomic orbital hybridisation via angular undamped Newtonian harmonic oscillators)

- Electrostatics (treating atoms as point charges)

- Van der Waals (by analytically differentiating the Lennard-Jones potential function)

# Cool features

- Unlimited portability: SENPAI does not require any library, and is 100% compliant with the C99 standard. **SENPAI could run on a Roomba**.

- Efficient computing model: the **Optimized Numerical Integration / Classical Harmonics and N-body** (ONI/CHaN) model was tailored for SENPAI over months of work. Inspired by the AMBER family of force fields, it allows SENPAI to compute forces with unprecedented efficiency.

- No potentials: Traditional MD simulators compute a particle's total potential and numerically differentiate it to compute the applied force. SENPAI analytically solves for the force vector, resulting in an extreme speed-up.

- Tunable thermodynamics: Efficiently write scripts to run simulations with various thermodynamical settings and analyse the impact of the system's surroundings on its evolution.

- Render as `.xyz`: Keep it simple. Simulations are rendered using the `.xyz` format, and can be directly visualized with software like VMD.

## Installation

On UNIX systems, compilation is achieved using the provided makefile. No additional software is required.

` git clone https://github.com/Garuda1/senpai && cd senpai && make`

## Usage

`senpai --i input_file --o output_file [options]`

Options:

- `--in` Required. Specifies the path to the input file (`.nh4` format)

- `--out` Required. Specifies the output file (`.xyz` format)

- `--numerical` Configures the simulation to use numerical differentiation of the potential energy (defaults to analytical solving)

- `--time t` Specifies the simulation duration (ns) (defaults to `1ns`)

- `--dt t` Specifies the timestep (fs) (defaults to `1 fs`)

- `--temp` Specifies the thermodynamic temperature of the system (K) (defaults to `273.15 K`)

- `--pressure` Specifies the pressure of the system (mbar) (defaults to `1 atm`)

- `--mol` Specifies the number of molecules to be inserted in the simulation

- `--frameskip` Sets a frameskip to reduce I/O usage. Disabled by default

## Example use

`./senpai.bin --in examples/urea.nh4 --out render.xyz --time 1000 --dt 0.5 --mol 512`

This will tell SENPAI to simulate 512 benzene molecules at STP for one nanosecond using a half femtosecond timestep. The rendered simulation will be saved in `render.xyz` for further analysis.

## Questions and Answers

Q: Tell me about you!

A: Thomas Murgia, 18y old when the first commit got on GitHub, 19y old at the time of writing this. I'm a second year undergraduate student of chemistry at the Université Toulouse 3 in France.

-----

Q: I WANT YOU IN MY LAB

A: e-mail me at <thomas.murgia@univ-tlse3.fr> (academic email). I'm open to any offers :)

-----

Q: Why are you doing this? Go out, the sun's out

A:   T h e   s u n   c a n ' t   s i m u l a t e   d e e p   e u t e c t i c   s o l v e n t s

# Documentation currently unavailable, as things are constantly suspect to drastic changes

## SENPAI file format 
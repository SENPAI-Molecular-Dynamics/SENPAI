# SENPAI - Simplified Evolutive N-body Processing and Analytics for Integration

SENPAI is a molecular dynamics (MD) simulation software aimed at simulating organic systems. It handles  the following interractions:

- Covalence (as undamped Newtonian harmonic oscillators)

- Bond angles (by enforcing atomic orbital hybridisation via angular undamped Newtonian harmonic oscillators)

- Electrostatics (treating atoms as point charges)

- Van der Waals (by analytically differentiating the Lennard-Jones potential function)

The simulation is rendered in a cubic universe of constant size, enforcing periodic boundary conditions via coordinate restriction.

# Cool features

- **Unlimited portability:** SENPAI does not require any library, and is 100% compliant with the C99 standard. **SENPAI could run on a Roomba**.

- **Efficient computing model:** the **Optimized Numerical Integration, Classical Harmonics, and N-body** (ONICHaN) model was tailored for SENPAI over months of work. Inspired by the AMBER family of force fields, it allows SENPAI to compute forces with unprecedented efficiency.

- **No potential differentiation:** Traditional MD simulators compute a particle's total potential and numerically differentiate it to compute the applied force. SENPAI analytically solves for the force vector, resulting in an extreme speed-up.

- **Tunable thermodynamics:** Efficiently write scripts to run simulations with various thermodynamical settings and analyse the impact of the system's surroundings on its evolution.

- **Render as `.xyz`:** Keep it simple. Simulations are rendered using the `.xyz` format, and can be directly visualized with software like VMD.

- **Integrated potential reduction:** SENPAI will reduce the system's potential energy with a user-selectable number of Monte Carlo cycles before starting the simulation.

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

- `--montecarlo` Sets a number of potential reduction cycles to apply. Defaults to 100.

## Example use

`./senpai.bin --in examples/urea.nh4 --out render.xyz --time 1000 --dt 0.5 --mol 512`

This will tell SENPAI to simulate 512 benzene molecules at STP for one nanosecond using a half femtosecond timestep. The rendered simulation will be saved in `render.xyz` for further analysis.

## SENPAI file format (NH4)

SENPAI uses its own file format, `.nh4`, named after SENPAI's predescesor, *Ammonium*. A `.nh4` file is structured the following way:

    OFFSET  SIZE    TYPE       DESCRIPTION
    0x00    4       Text       Must be "nh4\0" in ASCII (0x6E 0x68 0x34 0x00)
    0x04    8       uint64_t   Number of atoms in the system
	0x0C	??      ??         Atom data

Atom data is encoded in the following way, for each atom:

    OFFSET  SIZE    TYPE       DESCRIPTION
    0x00    1       uint8_t    Atom type (Full list in headers/model.h)
    0x01    8       double     Electrostatic charge (in elementary charges)
    0x09    8       double     Lennard-Jones epsilon (kJ/Kb)
    0x11    8       double     Lennard-Jones sigma (A)
    0x19    8       double     Coordinate along the x axis (A)
    0x21    8       double     Coordinate along the y axis (A)
    0x29    8       double     Coordinate along the z axis (A)
    0x31    1       uint8_t    Number of covalent bonds
    0x32    ??      ??         Bond data

Bond data for each atom is structure the following way:

    OFFSET  SIZE    TYPE       DESCRIPTION
    0x00    8       uint64_t   ID of the atom the current atom is bonded to
    0x08    8       double     Bond force constant (N/m or kg.s-2)

## Questions and Answers

Q: Why are you doing this? Go out, the sun's out.

A: The sun can't simulate deep eutectic solvents.

-----

Q: Who are you?

A: Thomas Murgia, 19y old at the time of writing this. I'm a second year undergraduate student of chemistry at the Université Toulouse 3 in France.

I plan to keep going with projects like those and hopefully get involved in purely academic and fundamental research in chemistry until the end of my days.

-----

Q: Are you looking for grad programs? Or even internships?

A: Definitely. E-mail me at <thomas.murgia@univ-tlse3.fr> (academic email). I'm open to all offers :)
    

# Documentation currently unavailable, as things are constantly suspect to drastic changes
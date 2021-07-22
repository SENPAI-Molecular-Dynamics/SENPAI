# SENPAI - Simplified Evolutive N-body Processing and Analytics for Integration

SENPAI is a flexible, highly-configurable molecular dynamics (MD) simulation software aimed at students and academia.

It provides a way to simulate molecular systems of educative and academic interest on devices ranging from personal laptops to purpose-built mainframes.

SENPAI is licensed under the terms of the GPLv3.

![SENPAI being used with VMD](https://i.imgur.com/kSL3KkY.png)*SENPAI being used with VMD to simulate a urea-ChCl deep eutectic solvent*

# Keeping it simple

- **Integrated potential reduction:** Easily mitigate the issue of "garbage in, garbage out"! If told to do so, SENPAI will reduce the potential energy of the system to a user-selected target value before simulating. SENPAI sequentially runs several algorithms like gradient descent to properly set the system up for simulation.

- **No potential differentiation:** Traditional MD simulators compute a particle's total potential and numerically differentiate it to compute the applied force. SENPAI analytically solves for the force vector, resulting in an extreme speedup. Should you prefer numerical potential differentiation, SENPAI offers two different numerical differentiation methods which can be configured as well!

- **No complicated file formats:** The MDS, MDM, and MDP file formats developped for SENPAI can be hand-written! No need for special software. Quickly and easily prepare simulations and adjust them with no hassle. Simulations are rendered as XYZ, and can be immediately viewed using existing software without prior conversion!

- **Full C99 compliance, no libraries needed:** SENPAI is 100% compliant with the C99 standard. No extra libraries are required!

- **Efficient computing model:** the ONICHaN (Optimized Numerical Integration, Classical Harmonics, and N-body) model was tailored for SENPAI over months of work. Inspired by the AMBER family of force fields, it allows SENPAI to compute forces with unprecedented efficiency.

# Installation

You might first want to configure SENPAI for your machine and use case. This is done by editing the constants in `headers/config.h`.

Should you want to go ahead with the default settings, compilation is achieved on UNIX systems using the provided Makefile.

```
$ git clone https://github.com/Chelsea486MHz/senpai
$ cd senpai
$ make
```

The installation process requires the C standard library, a C99-compliant compiler, and GNU Make.

# Usage
From a terminal:

`$ senpai [options]`

Options:

- `--in` **Required.** Specifies the path to the substrate to simulate

- `--solvent` **Required.** Specified the path to the solvent file

- `--out` **Required.** Specifies the path to the output file

- `--model` **Required.** Specifies the path to the model file 

- `--numerical` Configures the simulation to use numerical differentiation of the potential energy (defaults to analytical solving)

- `--numerical-tetra` is similar to `--numerical`, but numerical differentiation uses points in a tetrahedron instead of points in an octahedron

- `--time t` Specifies the simulation duration (ns) (defaults to `1ns`)

- `--dt t` Specifies the timestep (fs) (defaults to `1 fs`)

- `--temp` Specifies the thermodynamic temperature of the system (K) (defaults to `298.15 K`)

- `--pressure` Specifies the pressure of the system (mbar) (defaults to `1 atm`)

- `--density` Specifies the density of the system (g.cm-3) (defaults to `1 g.cm-3`)

- `--copy` Specifies how many copies of the loaded system are to be inserted in the simulation

- `--frameskip` Sets a frameskip to reduce I/O usage. Disabled by default

- `--reduce_potential` Reduces the potential energy to the provided value (pJ) or lower (defaults to 10 pJ)

- `--srand` Specifies an unsigned integer seed for randomization.  If the seed is not specified, the program will default to a random-enough seed.

# Example use

`$ ./senpai.bin --substrate examples/water.mol examples/void.mol --out water_render.xyz --model examples/models/senpai.mdm --srand 0`

This simulation is used to test-run SENPAI during development. It should run pretty fast and tell you if everything is working correctly.

`$ ./senpai.bin --substrate examples/DES.mol --solvent examples/water.mol --out render.xyz --model examples/models/senpai.mdm`

This will tell SENPAI to simulate the contents of `examples/DES.mol` in STP water for one nanosecond using a one femtosecond timestep. The rendered simulation will be saved in `render.xyz` for further analysis.

`$ ./senpai.bin --substrate examples/water.mol examples/void.mol --out water_render.xyz --model examples/models/senpai.mdm --copy 5000 --reduce_potential 10 --temp 300`

This will have SENPAI load 1000 water molecules from `examples/water.mol`, arrange them so that the total potential energy is lower than 100 pJ, and finally simulate the system at 300 Kelvin for 1 ns, saving the render in `water_render.xyz`.


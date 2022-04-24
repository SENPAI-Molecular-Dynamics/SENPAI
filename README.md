# SENPAI - Simplified Evolutive N-body Processing and Analytics for Integration

Latest stable release: 1.1

[Official SENPAI website](https://senpaimd.org)

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

# Documentation

Documentation is available on the [Official SENPAI website](https://senpaimd.org).

[Installation Manual](https://senpaimd.org/manuals/installation)

[Your first simulation](https://senpaimd.org/senpai/manuals/first-simulation)

[Creating substrates](https://senpaimd.org/senpai/manuals/creating-substrates)

[Creating models](https://senpaimd.org/senpai/manuals/creating-models)

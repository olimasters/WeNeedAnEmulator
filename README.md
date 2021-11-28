### C++ is a Dynamically-Typed Interpreted Language

This is an implementation of a solution for the We Need An Emulator CTF challenge from the Tenable 2021 CTF Competition.

It's written entirely in the C++ type system. We do not execute, or even create, an executable binary from the C++.
Instead, we produce an assembly file and extract the output from it.

This code has only been tested on x86-64 Linux, using both G++ and Clang++ on all standard optimization settings.

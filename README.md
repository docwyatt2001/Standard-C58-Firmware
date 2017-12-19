# Standard C58 Firmware

## Synopsis

Firmware to drive a Atmel ATMEGA8515 MCU as a replacement MCU controller in a Standard C58 (2m band) radio, which is obsolete due to its age.

## Motivation

The original chip that controlled this radio is no longer available, so in order to make it work, new code was required to be written. It was originally written Pierluciano Calzolato (IU3EVR), but there was trouble with compiling this code. After changing syntax on certain parts, it was realised it didn't include the entire functionality of the original radio, so this is the endevour to do the following:

- complete the functionality that is missing

- refactor and optimise the code so that it's a little more logical.

## Installation

Nothing to install - compile, upload onto the Atmel MCU, install and voila!

## Tests

Still need to be written

## Contributors

- Pierluciano Calzolato (IU3EVR/Italy), the inital kickstarter to this project with the original version

- Alan White (VK6YIG/Australia) whose radio this was needed for - he supplied the hardware for testing ;)

The issue tracker here is all I am using for it.

## License

There was no license in the original, so I will keep it so.

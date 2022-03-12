# passman++-cli
passman++-cli is a complete, command line, from-the-ground-up wrapper around [libpassman](https://github.com/binex-dsk/libpassman), using a standard UNIX-like command line syntax.

This project is written in pure C++, using Qt and Botan.

## passman++
[passman++](https://github.com/binex-dsk/passmanpp) was a simple-to-use, near-complete, and practical GUI implementation of libpassman. I have since abandoned the project, due to the overall complexities of the entire GUI system, and the mess of code created due to such complexities.

## From the Ground Up
passman++-cli is written completely from scratch, with little code taken directly from passman++. Built with the power user in mind, yet still being simple to use for beginners, passman++-cli is designed to provide the most flexible and dead-simple way to manage databases without the use of GPG.

## WIP
This project is in the earliest stages of development, as I work on creating a basic outline of the application's main infrastructure. Given the from-scratch philosophy, this will likely take a while depending on how far the basic infrastructure goes. There are still many questions to answer, like:

- Use a per-user (i.e. `~/.local/share/passman`) storage location, or just relative folders?
- Interactive prompt? (probably not).
- Make 2fa per-database or global?
- Require Qt 6 or not? (may require a rewrite of libpassman to support Qt 5)

And much more.

## Contributing Guidelines and Rules
There are none. Everyone who is willing and able to contribute is always welcome to do so.

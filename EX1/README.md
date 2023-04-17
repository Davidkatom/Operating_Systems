# Operating Systems Exercise 1

This repository contains a custom shell implementation (`myshell.c`) and two shell scripts (`gccfind.sh` and `tennis.sh`).

## Table of Contents
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
  - [MyShell](#myshell)
  - [GCCFind](#gccfind)
  - [Tennis](#tennis)
- [Author](#author)

## Prerequisites
- GCC Compiler
- Bash Shell

## Installation
1. Clone this repository to your local machine:
```bash
git clone https://github.com/username/custom-shell-and-scripts.git
```
2. Compile the `myshell.c` file:
```bash
gcc -o myshell myshell.c
```
3. Give executable permissions to the shell scripts:
```bash
chmod +x gccfind.sh
chmod +x tennis.sh
```

## Usage
### MyShell
This is a custom shell implementation that supports basic commands like `cd`, `exit`, and a `history` command to display previously executed commands with their process IDs.

To run the custom shell, execute the compiled binary:
```bash
gcc myshell.c
./a.out
```

### GCCFind
This shell script searches for C source files containing a specific word and compiles them using GCC. It can either search recursively or within the current directory.

Usage:
```bash
./gccfind.sh <path> <search_word> [-r]
```

- `<path>`: The directory path where the search should start
- `<search_word>`: The word to search for in the C source files
- `-r` (optional): Enables recursive search

### Tennis
This shell script is a text-based tennis game for two players. Each player picks a number, and the game updates the score based on the player's choices.

To start the game, run the script:
```bash
./tennis.sh
```

## Author
- David Shnaiderov 

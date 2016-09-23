# SIC

Saving
Implemention of a
Context-free grammar

### What is it ?

It is a context-free grammar, quite similar to an [EBNF](https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_Form), which allows to parse an input given a set of rules.

On top of this, SIC add the possibility to save parts of your input from rules. (See [examples](./examples))

### Install

`$> make`

Then use the compiled file 'libsic.a' as any static library

### Syntax

See [syntax file](./doc/syntax.txt)

### Simple example

```c
sic_t sic;
char* check_it = "Am I Ok ?";

if (sc_init(&sic) == NULL) //Initialize sic
  return (1);
if (sc_load_file(&sic, "rules.rl") == 0) //Load rules
  return (1);
if (sc_parse(&sic, check_it, strlen(check_it))) //Try parse input using rules
  printf("SIC tells it's OK !\n");
else
  printf("SIC tells to fuck off !\n");
```

`$> make example` to get a functional example of a binary using SIC.

* Moar in [examples/](./examples)

### Prerequisites

* GCC-compatible compiler
* make
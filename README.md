# SIC

Saving
Implemention of a
Context-free-grammar

### What is it ?

//TODO

### Install

Just compile it with `make`
//TODO

### Syntax

See [syntax file](./doc/syntax.txt)

### Example

```c
sic_t sic;
char* check_it = "Am I Ok ?";

if (sc_init(&sic) == NULL || sc_load_file(&sic, "rules.rl") == 0)
  return (1);
if (sc_parse(&sic, check_it, strlen(check_it)))
  printf("SIC tells it's OK !\n");
else
  printf("SIC tells to fuck off !\n");
```

Moar in [examples/](./examples)

Moreover, you can `make example` to get a functional example of a binary using SIC

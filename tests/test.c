#include <assert.h>
#include "sic/sic.h"

void bytes()
{
  sc_bytes_t bytes;
  assert(sc_binit(&bytes, NULL, 0, NULL) != NULL);
  assert(bytes.size == 0);
  assert(sc_bcpy(&bytes, "hello", 5) != NULL);
  assert(bytes.size == 5);
  assert(sc_bapp(&bytes, " world", 6) != NULL);
  assert(bytes.size == 11);
  assert(sc_bappc(&bytes, 0) != NULL);
  assert(strcmp(bytes.array, "hello world") == 0);

  assert(sc_berase(&bytes, 0, 1) != NULL);
  assert(strcmp(bytes.array, "ello world") == 0);
  assert(sc_berase(&bytes, 42, 1) != NULL); //Can't erase outside of byte array
  assert(strcmp(bytes.array, "ello world") == 0);
  assert(sc_berase(&bytes, 9, 1) != NULL);
  assert(strcmp(bytes.array, "ello worl") == 0);
  assert(sc_berase(&bytes, 0, 100) != NULL); //Erase as there is content
  assert(bytes.size == 0);
  sc_bdestroy(&bytes);
}

void hashmap()
{
  //Default behaviour w/ string
  sc_hashmp_t string_map;
  assert(sc_hinit(&string_map, 1024, &sc_jenkins_hash, SC_KY_STRING) != NULL);
  assert(sc_hget(&string_map, "42") == NULL);
  assert(sc_hadd(&string_map, "42", "UNIVERSE") != NULL); //Standard add
  assert(sc_hget(&string_map, "42") != NULL);
  assert(strcmp(sc_hget(&string_map, "42"), "UNIVERSE") == 0);
  assert(sc_hadd(&string_map, "42", "ME") != NULL); //Standard replace
  assert(strcmp(sc_hget(&string_map, "42"), "UNIVERSE") != 0);
  assert(strcmp(sc_hget(&string_map, "42"), "ME") == 0);
  sc_hdestroy(&string_map);

  //Default behaviour w/ pointer
  //TODO :)

  //Default behavior w/ collisions
  sc_hashmp_t collide_map;
  assert(sc_hinit(&collide_map, 1, &sc_jenkins_hash, SC_KY_STRING) != NULL);
  assert(sc_hadd(&collide_map, "GUILLAUME", "TWAT") != NULL);
  assert(sc_hget(&collide_map, "GUILLAUME") != NULL);
  assert(strcmp(sc_hget(&collide_map, "GUILLAUME"), "TWAT") == 0);
  assert(sc_hadd(&collide_map, "THEO", "I LIKE CAPS") != NULL); //Add on 1-size bucket, collision
  assert(sc_hget(&collide_map, "THEO") != NULL);
  assert(strcmp(sc_hget(&collide_map, "THEO"), "I LIKE CAPS") == 0);
  assert(strcmp(sc_hget(&collide_map, "GUILLAUME"), "TWAT") == 0);
  sc_hdestroy(&collide_map);
}

int main()
{
  bytes();
  hashmap();
  //consumer();
  //sic();
  printf("Success :)\n");
  return (0);
}
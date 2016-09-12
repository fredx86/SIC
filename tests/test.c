#include <assert.h>
#include "sic/sic.h"

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
}

int main()
{
  //bytes();
  hashmap();
  //consumer();
  //sic();
  printf("Success :)\n");
  return (0);
}
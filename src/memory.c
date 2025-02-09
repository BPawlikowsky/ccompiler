#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#define MEMORY_SIZE 1024 * 1024 * 40

char memory[MEMORY_SIZE];

long memory_counter = 0;

void *get_memory(long size) {
  if (memory_counter + size >= MEMORY_SIZE) {
    assert(memory_counter + size < MEMORY_SIZE);
    return NULL;
  }
  void *result = (void *)&memory[memory_counter];
  memory_counter += size;
  return result;
}


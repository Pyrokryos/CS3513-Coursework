#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

typedef struct HashTableEntry
{
  char *key;
  size_t type;
  union
  {
    char *s;
    int i;
    double d;
  } val;

} HashTableEntry;

typedef struct HashTable
{
  size_t size;
  HashTableEntry **entries;
} HashTable;

HashTable *init_hash_table(size_t size);

static size_t hash(char *str, size_t tbl_size);

size_t insert(HashTable *ht, HashTableEntry *entry);

size_t delete(HashTable *ht, char *key);

HashTableEntry *search(HashTable *ht, char *key);

void free_hash_table_entry(HashTableEntry *entry);
void free_hash_table(HashTable *ht);

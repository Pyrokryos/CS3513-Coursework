#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

HashTable *init_hash_table(size_t size);

static size_t hash(char *str, size_t tbl_size);

size_t insert(HashTable *ht, HashTableEntry *entry);
HashTableEntry *search(HashTable *ht, char *key);
size_t delete(HashTable *ht, char *key);

HashTable *merge_hash_tables(HashTable *ht1, HashTable *ht2);

void free_hash_table_entry(HashTableEntry *entry);
void free_hash_table(HashTable *ht);

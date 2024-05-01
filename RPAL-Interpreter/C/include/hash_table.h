#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct HashTableEntry {
    const char *key;
    const char *val;
} HashTableEntry;

typedef struct HashTable {
    size_t size;
    HashTableEntry *entries;
} HashTable;

HashTable *initHashTable(size_t size);

static size_t hash(const char* str, size_t table_size);

int insert(HashTable *ht, const char *key, const char *val);

int delete(HashTable *ht, const char *key);

const char *search(HashTable *ht, const char *key);

void freeHashTable(HashTable *ht);

#include "../include/hash_table.h"
#include <stdio.h>

HashTable *initHashTable(size_t size) {
    HashTable *ht = (HashTable *) malloc(sizeof(HashTable));
    if (ht == NULL) {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    
    ht->size = size;
    ht->entries = (HashTableEntry *) malloc(size * sizeof(HashTableEntry));
    if (ht->entries == NULL) {
        free(ht);
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    for (size_t i = 0; i < size; ++i) {
        ht->entries[i].key = NULL;
        ht->entries[i].val = NULL;
    }
    
    return ht;
}

static size_t hash(const char* str, size_t table_size) {
    if (str == NULL) {
        perror("Cannot hash a null string.\n");
        exit(EXIT_FAILURE);
    }

    size_t sum = 0;
    for (size_t i = 0; str[i] != '\0'; ++i) {
        sum += (size_t)str[i];
    }
    return sum % table_size;
}

int insert(HashTable *ht, const char *key, const char *val) {
    size_t index = hash(key, ht->size);
    size_t originalIndex = index;

    while (ht->entries[index].key != NULL) {
        if (strcmp(ht->entries[index].key, key) == 0) {
            free((char *) ht->entries[index].val);
            ht->entries[index].val = strdup(val);
            return EXIT_SUCCESS;
        }

        if (index == originalIndex) {
            perror("Hash table is full.");
            exit(EXIT_FAILURE);
        }
        
        index = (index + 1) % ht->size;
    }

    ht->entries[index].key = strdup(key);
    ht->entries[index].val = strdup(val);
    return EXIT_SUCCESS;
}

int delete(HashTable *ht, const char *key) {
    size_t index = hash(key, ht->size);
    size_t originalIndex = index;
        
    while (ht->entries[index].key != NULL) {
        if (strcmp(ht->entries[index].key, key) == 0) {
            free((char *) ht->entries[index].key);
            free((char *) ht->entries[index].val);
            
            ht->entries[index].key = NULL;
            ht->entries[index].val = NULL;
            
            return EXIT_SUCCESS;
        }
        
        if (index == originalIndex) {
            return EXIT_FAILURE;
        }
            
        index = (index + 1) % ht->size;
    }
    
    return EXIT_FAILURE;
}

const char *search(HashTable *ht, const char *key) {
    size_t index = hash(key, ht->size);
    size_t originalIndex = index;
    
    while (ht->entries[index].key != NULL) {
        if (strcmp(ht->entries[index].key, key) == 0) {
            return strdup(ht->entries[index].val);
        }
    
        if (index == originalIndex) {
            return NULL;
        }
        
        index = (index + 1) % ht->size;
    }
    
    return NULL;
}

void freeHashTable(HashTable *ht) {
    if (ht == NULL) {
        return;
    }

    for (size_t i = 0; i < ht->size; ++i) {
        free((char *) ht->entries[i].key);
        free((char *) ht->entries[i].val);
    }

    free(ht->entries);

    free(ht);
}

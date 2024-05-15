#include <stdio.h>

#include "../include/hash_table.h"

HashTable *init_hash_table(size_t size)
{
  HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
  if (ht == NULL)
  {
    perror("Memory allocation failed");
    exit(EXIT_FAILURE);
  }

  ht->size = size;
  ht->entries = (HashTableEntry **)malloc(size * sizeof(HashTableEntry *));
  if (ht->entries == NULL)
  {
    perror("Memory allocation failed");
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < size; ++i)
  {
    ht->entries[i] = NULL;
    ht->entries[i] = NULL;
  }

  return ht;
}

static size_t hash(char *str, size_t tbl_size)
{
  if (str == NULL)
  {
    perror("Cannot hash a null string");
    exit(EXIT_FAILURE);
  }

  size_t sum = 0;
  for (size_t i = 0; str[i] != '\0'; ++i)
  {
    sum += (size_t)str[i];
  }
  return sum % tbl_size;
}

size_t insert(HashTable *ht, HashTableEntry *entry)
{
  size_t index = hash(entry->key, ht->size);
  size_t original_index = index;

  while (ht->entries[index] != NULL)
  {
    if (strcmp(ht->entries[index]->key, entry->key) == 0)
    {
      free_hash_table_entry(ht->entries[index]);
      ht->entries[index] = entry;
      return EXIT_SUCCESS;
    }

    index = (index + 1) % ht->size;
  
    if (index == original_index)
    {
      perror("Hash table is full");
      exit(EXIT_FAILURE);
    }
  }

  ht->entries[index] = entry;
  return EXIT_SUCCESS;
}

size_t delete(HashTable *ht, char *key)
{
  size_t index = hash(key, ht->size);
  size_t original_index = index;

  while (ht->entries[index] != NULL)
  {
    if (strcmp(ht->entries[index]->key, key) == 0)
    {
      free_hash_table_entry(ht->entries[index]);
      return EXIT_SUCCESS;
    }

    if (index == original_index)
    {
      return EXIT_FAILURE;
    }

    index = (index + 1) % ht->size;
  }

  return EXIT_FAILURE;
}

HashTableEntry *search(HashTable *ht, char *key)
{
  size_t index = hash(key, ht->size);
  size_t original_index = index;

  while (ht->entries[index] != NULL)
  {
    if (strcmp(ht->entries[index]->key, key) == 0)
    {
      return ht->entries[index];
    }

    index = (index + 1) % ht->size;
  
    if (index == original_index)
    {
      return NULL;
    }
  }

  return NULL;
}

HashTable *merge_hash_tables(HashTable *ht1, HashTable *ht2)
{
  if (ht1 == NULL)
  {
    return ht2;
  }
  else if (ht2 == NULL)
  {
    return ht1;
  }
  else
  {
    HashTable *merged = init_hash_table(ht1->size + ht2->size);

    for (size_t i = 0; i < ht1->size; ++i)
    {
      if (ht1->entries[i] != NULL)
      {
        insert(merged, ht1->entries[i]);
      }
    }

    for (size_t i = 0; i < ht2->size; ++i)
    {
      if (ht2->entries[i] != NULL)
      {
        insert(merged, ht2->entries[i]);
      }
    }

    return merged;
  }
}

void free_hash_table_entry(HashTableEntry *entry)
{
  if (entry == NULL)
  {
    return;
  }

  free(entry->key);
  entry->key = NULL;

  if (entry->type == STRING)
  {
    free(entry->val.s);
    entry->val.s = NULL;
  }

  free(entry);
  entry = NULL;
}

void free_hash_table(HashTable *ht)
{
  if (ht == NULL)
  {
    return;
  }

  for (size_t i = 0; i < ht->size; ++i)
  {
    free_hash_table_entry(ht->entries[i]);
  }

  free(ht->entries);
  ht->entries = NULL;

  free(ht);
  ht = NULL;
}

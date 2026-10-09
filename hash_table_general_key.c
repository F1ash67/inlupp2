#include "hash_table_general_key.h"
#include "hash_table_iterator.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define No_buckets 17

typedef struct entry entry_t;

typedef struct word_frequency_pair word_frequency_pair_t;

struct entry {
    elem_t key;
    elem_t value;
    entry_t *next;
};

struct hash_table {
    ioopm_hash_function *hash_fn;
    ioopm_eq_function *key_eq_fn;
    entry_t buckets[No_buckets];
};

struct hash_table_iterator {
  ioopm_hash_table_t *ht;
  size_t current_bucket;
  entry_t *current_entry;
};

struct word_frequency_pair {
    char* word;
    size_t frequency;
};

ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function *hash_fn, ioopm_eq_function *key_eq_fn) {
    ioopm_hash_table_t *ht = calloc(1, sizeof(ioopm_hash_table_t));
    ht->hash_fn = hash_fn;
    ht->key_eq_fn = key_eq_fn;
    return ht;
}

static size_t string_knr_hash(const char *str) {
    size_t result = 0;
    while(*str != '\0') {
        result = result * 31 + ((unsigned char) *str);
        str++;
    }
    return result;
}

static entry_t *entry_create(elem_t key, elem_t value, entry_t *next) {
    entry_t *new_entry = malloc(sizeof(entry_t));
    new_entry->key = key;
    new_entry->value = value;
    new_entry->next = next;
    return new_entry;
}

static void entry_destroy(entry_t *ent) {
    free(ent);
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht) {
    for(size_t i = 0; i < No_buckets; i++) {
        entry_t *current = ht->buckets[i].next;
        while(current != NULL) {
            entry_t *next = current->next;
            entry_destroy(current);
            current = next;
        }
    }
    free(ht);
}

static entry_t *find_previous_entry(ioopm_hash_table_t *ht, elem_t key) {
    size_t bucket = ht->hash_fn(key) % No_buckets;
    entry_t *current = &ht->buckets[bucket];

    while(current->next != NULL && !ht->key_eq_fn(current->next->key, key)) {
        current = current->next;
    }
    return current;
}


void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value) {
    entry_t *previous = find_previous_entry(ht, key);

    if(previous->next != NULL) {
        previous->next->value = value;
    }
    else {
        previous->next = entry_create(key, value, NULL);
    }
}

bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key, elem_t *result) {
    entry_t *previous = find_previous_entry(ht, key);

    if(previous->next != NULL) {
        *result = previous->next->value;
        return true;
    }
    else {
        return false;
    }
}

bool ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key, elem_t *result) {
    entry_t *previous = find_previous_entry(ht, key);

    if(previous->next != NULL) {
        entry_t *to_remove = previous->next;
        *result = to_remove->value;

        previous->next = to_remove->next;
        entry_destroy(to_remove);

        return true;
    }
    else {
        return false;
    }
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key) {
    elem_t result;
    return ioopm_hash_table_lookup(ht, key, &result);
}

size_t ioopm_hash_table_size(ioopm_hash_table_t *ht) {
    size_t size = 0;

    for(size_t i = 0; i < No_buckets; i++) {
        entry_t *current = ht->buckets[i].next;

        while(current != NULL) {
            size++;
            current = current->next;
        }
    }
    return size;
}

bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht) {
    if(ioopm_hash_table_size(ht) == 0) {
        return true;
    }
    return false;
}

static void advance_iterator_state(ioopm_hash_table_iterator_t *it) {
    it->current_entry = it->current_entry->next;

    if(it->current_entry == NULL) {
        it->current_bucket += 1;

        if(it->current_bucket != No_buckets) {
            it->current_entry = &it->ht->buckets[it->current_bucket];
        }
    }
}

static void skip_sentinel_nodes(ioopm_hash_table_iterator_t *it) {
    while(it->current_bucket != No_buckets && it->current_entry == &it->ht->buckets[it->current_bucket]) {
        advance_iterator_state(it);
    }
}

ioopm_hash_table_iterator_t *ioopm_hash_table_iterator_create(ioopm_hash_table_t *ht) {
    ioopm_hash_table_iterator_t *it = malloc(sizeof(ioopm_hash_table_iterator_t));
    it->ht = ht;
    it->current_bucket = 0;
    it->current_entry = &ht->buckets[0];
    skip_sentinel_nodes(it);
    return it;
}

void ioopm_hash_table_iterator_destroy(ioopm_hash_table_iterator_t *it) {
    free(it);
}

bool ioopm_hash_table_iterator_at_end(ioopm_hash_table_iterator_t *it) {
    return it->current_bucket == No_buckets;
}

void ioopm_hash_table_iterator_advance(ioopm_hash_table_iterator_t *it) {
    advance_iterator_state(it);
    skip_sentinel_nodes(it);
}

elem_t ioopm_hash_table_iterator_current_key(ioopm_hash_table_iterator_t *it) {
    return it->current_entry->key;
}

elem_t ioopm_hash_table_iterator_current_value(ioopm_hash_table_iterator_t *it) {
    return it->current_entry->value;
}

static int comp(const void *a, const void *b) {
        const word_frequency_pair_t *pa = a;
        const word_frequency_pair_t *pb = b;
        return (pb->frequency > pa->frequency) - (pb->frequency < pa->frequency);
}

size_t str_hash(elem_t key) {
    return string_knr_hash(key.s);
}

bool str_eq(elem_t a, elem_t b) {
    return strcmp(a.s, b.s) == 0;
}

void ioopm_read_file(char *file_src, ioopm_hash_table_t *ht) {

    FILE *file = fopen(file_src, "r");
    if(file == NULL) {
        perror(file_src);
        return;
    }
    
    char* line = NULL;
    size_t bufsize = 0;

    while(getline(&line, &bufsize, file) != -1) {
        char *word = strtok(line, " \t\n.,!?;:\"()");
        while(word != NULL) {
            elem_t result;
            if(ioopm_hash_table_lookup(ht, string_elem(word), &result)) {
                ioopm_hash_table_insert(ht, string_elem(word), int_elem(result.i + 1));
            }
            else{
                ioopm_hash_table_insert(ht, string_elem(strdup(word)), int_elem(1));
            }
            word = strtok(NULL, " \t\n.,!?;:\"()");
        }
    }
    
    fclose(file);
    free(line);
}
void ioopm_sort_by_frequency(ioopm_hash_table_t *ht) {

    ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(ht);

    size_t count = ioopm_hash_table_size(ht);
    word_frequency_pair_t *pairs = malloc(count * sizeof(word_frequency_pair_t));

    size_t ent = 0;

    while(!ioopm_hash_table_iterator_at_end(it)) {
        pairs[ent].word =  ioopm_hash_table_iterator_current_key(it).s;
        pairs[ent].frequency = ioopm_hash_table_iterator_current_value(it).i;
        ent++;
        ioopm_hash_table_iterator_advance(it);
    }

    qsort(pairs, count, sizeof(word_frequency_pair_t), comp);

    for(size_t i = 0; i < count; i++) {
        printf("%s: %zu\n", pairs[i].word, pairs[i].frequency);
    }

    for(size_t i = 0; i < count; i++) {
        free(pairs[i].word);
    }

    free(pairs);
    ioopm_hash_table_destroy(ht);
    ioopm_hash_table_iterator_destroy(it);
}
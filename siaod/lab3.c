#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct table{
    char** key;
    char** value;
    int* tombstone;
    size_t size;
} hashTable;

hashTable* newHashTable(size_t);
void freeHashTable(hashTable*);
int addValueForKey(hashTable*, const char*, const char*);
void printHashTable(hashTable*);
void removeValueForKey(hashTable*, char*);
const char* getValueForKey(hashTable*, const char*);
unsigned long getStringHash(const char*);

hashTable* newHashTable(size_t size) {
    hashTable* ret = (hashTable*)malloc(sizeof(hashTable));
    ret->size = size;
    ret->key = (char**)malloc(sizeof(char*) * size);
    for (size_t i = 0; i < size; i++)
        ret->key[i] = NULL;
    ret->value = (char**)malloc(sizeof(char*) * size);
    ret->tombstone = (int*)calloc(size, sizeof(int));
    return ret;
}

unsigned long getStringHash(const char* value) {
    unsigned long hash = 7;
    for (size_t i = 0; i < strlen(value); i++)
        hash = hash * 31 + value[i];
    return hash;
}

void freeHashTable(hashTable* table) {
    for (size_t i = 0; i < table->size; i++)
        if (table->key[i] && !table->tombstone) {
            free(table->key[i]);
            free(table->value[i]);
        }
    free(table->tombstone);
    free(table);
}

void printHashTable(hashTable* table) {
    for (size_t i = 0; i < table->size; i++)
        if (table->key[i] && !table->tombstone[i])
            printf("  key: %s; value: %s\n", table->key[i], table->value[i]);
}

const char* getValueForKey(hashTable* table, const char* key) {
    size_t index = getStringHash(key) % table->size;
    size_t current = index;
    do {
        if (!table->key[current])
            return NULL;
        else if (!table->tombstone[current] && !strcmp(table->key[current], key))
            return table->value[current];
        current = (current + 1) % table->size;
    } while (current != index);
    return NULL;
}

int addValueForKey(hashTable* table, const char* key, const char* value) {
    size_t index = getStringHash(key) % table->size;
    size_t current = index;
    do {
        if (table->tombstone[current]) {
            table->key[current] = strdup(key);
            table->value[current] = strdup(value);
            table->tombstone[current] = 0;
            return 0;
        } else if (!table->key[current]) {
            table->key[current] = strdup(key);
            table->value[current] = strdup(value);
            return 0;
        } else if (!strcmp(key, table->key[current])){
            free(table->value[current]);
            table->value[current] = strdup(value);
            return 0;
        }
        current = (current + 1) % table->size;
    } while (current != index);
    return 1;
}

void removeValueForKey(hashTable* table, char* key) {
    size_t index = getStringHash(key) % table->size;
    size_t current = index;
    do {
        if (!table->key[current])
            return;
        if (!table->tombstone[current] && !strcmp(table->key[current], key)) {
            free(table->key[current]);
            free(table->value[current]);
            table->tombstone[current] = 1;
            return;
        }
        current = (current + 1) % table->size;
    } while (current != index);
}

int main(int argc, char** argv) {
    hashTable* table = newHashTable(10);
    size_t maxStringLen = 256;
    char input[maxStringLen];
    while (1) {
        puts("\nusage: a <key> <value> - add value for key\n" \
             "       r <key> - remove value for key\n" \
             "       f <key> - get value for key\n" \
             "       p - print table\n" \
             "       q - quit");
        fgets(input, maxStringLen-1, stdin);
        char* token = strtok(input, " ");
        switch (input[0]) {
            case 'a': {
                const char* key = strtok(NULL, " \n");
                key = key ? key : "";
                const char* value = strtok(NULL, "\n");
                value = value ? value : "";
                if (addValueForKey(table, key, value))
                    puts("ERROR: Unable to add new key value pair, table overflow");
                else
                    printHashTable(table);
                break;
            }
            case 'r': {
                char* key = strtok(NULL, "\n");
                key = key ? key : "";
                removeValueForKey(table, key);
                printHashTable(table);
                break;
            }
            case 'f': {
                const char* key = strtok(NULL, "\n");
                key = key ? key : "";
                const char* value = getValueForKey(table, key);
                if (value)
                    printf("value for key %s: %s\n", key, value);
                else
                    printf("ERROR: value for key %s not found\n", key);
                break;
            }
            case 'p':
                printHashTable(table);
                break;
            case 'q':
                freeHashTable(table);
                return 0;
            default:
                break;
        }
    }
}

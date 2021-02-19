#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char** first;
    size_t size;
    size_t used;
} List;

List* newList();
void printList(List*);
void find(List*, char*);
void freeList(List*);
void resizeList(List*);
void addToList(List*, char*);
void addToListBefore(List*, char*, char*);
void removeFirst(List*, char*);
void removeAll(List*, char*);

List* newList() {
    List* ret = (List*)malloc(sizeof(List));
    ret->size = 2;
    ret->first = (char**)malloc(sizeof(char*) * ret->size);
    ret->used = 0;
    return ret;
}

void printList(List* list) {
    printf("List size: %lu; used: %lu\n", list->size, list->used);
    for (size_t i = 0; i < list->used; i++)
        printf("[%lu] %s\n", i, list->first[i]);
}

void find(List* list, char* value) {
    for (size_t i = 0; i < list->used; i++)
        if (!strcmp(list->first[i], value)) {
            printf("[%lu] %s\n", i, list->first[i]);
            return;
        }
    printf("[-] Not found\n");
}

void freeList(List* list) {
    for (size_t i = 0; i < list->used; i++)
        free(list->first[i]);
    free(list);
}

void resizeList(List* list) {
    list->size <<= 1;
    list->first = (char**)realloc(list->first, sizeof(char*) * list->size);
}

void addToList(List* list, char* value) {
    size_t i = 0;
    if (list->used++) {
        if (list->used == list->size)
            resizeList(list);
        for (; i < list->used; i++)
            if (strcmp(value, list->first[i]) < 0)
                break;
        for (size_t j = list->used - 1 ; j > i; j--)
            list->first[j] = list->first[j-1];
    }
    list->first[i] = (char*)malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(list->first[i], value);
    return;
}

void addToListBefore(List* list, char* value, char* before) {
    if (list->used++) {
        if (list->used == list->size)
            resizeList(list);
        for (size_t i = 0; i < list->used; i++)
            if (!strcmp(before, list->first[i])) {
                for (size_t j = list->used - 1; j > i; j--)
                    list->first[j] = list->first[j-1];
                list->first[i] = (char*)malloc(sizeof(char) * (strlen(value) + 1));
                strcpy(list->first[i], value);
                return;
            }
    }
    list->first[list->used - 1] = (char*)malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(list->first[list->used - 1], value);
}

void removeFirst(List* list, char* value) {
    for (size_t i = 0; i < list->used; i++)
        if (!strcmp(list->first[i], value)) {
            free(list->first[i]);
            for (size_t j = i; j < list->used - 1; j++)
                list->first[j] = list->first[j+1];
            list->used--;
            return;
        }
}

void removeAll(List* list, char* value) {
    for (size_t i = 0; i < list->used; i++)
        if (!strcmp(list->first[i], value)) {
            free(list->first[i]);
            for (size_t j = i; j < list->used - 1; j++) {
                list->first[j] = list->first[j+1];
            }
            i--;
            list->used--;
        }
}

int main(int argc, char** argv) {
    List* list = newList();
    size_t maxStringLen = (argc == 2) ? atoi(argv[1]) : 256;
    char cmd[maxStringLen];
    puts("usage: a <string> - add\n" \
         "       s <string> <before> - add before\n"
         "       r <string> - remove first\n" \
         "       t <string> - remove all\n" \
         "       f <string> - find\n" \
         "       p - print list\n" \
         "       q - quit");
    while (1) {
        memset(cmd, 0, maxStringLen);
        fgets(cmd, maxStringLen-1, stdin);
        char* token = strtok(cmd, " ");
        switch (cmd[0]) {
            case 'a':
                addToList(list, strtok(NULL, " \n"));
                break;
            case 's': {
                char* value = strtok(NULL, " ");
                char* before = strtok(NULL, " \n");
                addToListBefore(list, value, before);
                break;
            }
            case 'r':
                removeFirst(list, strtok(NULL, " \n"));
                break;
            case 't':
                removeAll(list, strtok(NULL, " \n"));
                break;
            case 'f':
                find(list, strtok(NULL, " \n"));
                break;
            case 'p':
                printList(list);
                break;
            case 'q':
                freeList(list);
                return 0;
            default:
                break;
        }
    }
}

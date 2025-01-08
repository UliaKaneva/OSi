#ifndef TWIN_ALLOCATOR_H
#define TWIN_ALLOCATOR_H

#include <sys/mman.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct Block {
    size_t size;
    bool is_free;
    struct Block* next;
} Block;

typedef struct Allocator {
    void* memory;
    size_t size;
    Block* list;
} Allocator;

Allocator *allocator_create(void *const memory, const size_t size);
void *allocator_alloc(Allocator *const allocator, const size_t size);
void allocator_free(Allocator *const allocator, void *const memory);
void allocator_destroy(Allocator *const allocator);

#endif
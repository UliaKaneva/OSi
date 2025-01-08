#ifndef BLOCKS2N_ALLOCATOR_H
#define BLOCKS2N_ALLOCATOR_H

#include <stdbool.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>

typedef struct Block {
    size_t block_size;
    bool is_free;
    struct Block *next_free;
} Block;

typedef struct Allocator {
    void *memory;
    size_t size;
    Block *free_blocks;
} Allocator;

Allocator *allocator_create(void *const memory, const size_t size);
void *allocator_alloc(Allocator *const allocator, const size_t size);
void allocator_free(Allocator *const allocator, void *const memory);
void allocator_destroy(Allocator *const allocator);

#endif
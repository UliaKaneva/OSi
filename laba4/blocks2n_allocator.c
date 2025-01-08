#include "blocks2n_allocator.h"

Allocator *allocator_create(void *const memory, const size_t size) {
	if (memory == NULL || size < sizeof(Allocator) + sizeof(Block) ||
		((size - sizeof(Allocator) & (size - sizeof(Allocator) - 1)) != 0)) {
        return NULL;
    }

    Allocator *allocator = (Allocator *)memory;
    allocator->memory = (uint8_t *)memory + sizeof(Allocator);
    allocator->size = size - sizeof(Allocator);
    allocator->free_blocks = (Block *)allocator->memory;

    // Инициализация свободных блоков
    allocator->free_blocks->block_size = allocator->size;
    allocator->free_blocks->next_free = NULL; // Нет других свободных блоков
    allocator->free_blocks->is_free = true;

    return allocator;
}

void *allocator_alloc(Allocator *const allocator, const size_t size) {
    if (!allocator || size == 0) {
        return NULL;
    }

    // Определяем размер блока, который нужно выделить
    size_t block_size = 1;
    while (block_size < size + sizeof(Block)) {
        block_size *= 2;
    }

    Block *block = allocator->free_blocks;
    Block *prev = NULL;

    while (block != NULL) {
        if (block->block_size >= block_size && block->is_free) {
            // Выделяем блок
            if (prev == NULL) {
                allocator->free_blocks = block->next_free;
            } else {
                prev->next_free = block->next_free;
            }
            block->is_free = false;
            return (void *)((uint8_t *)block + sizeof(Block));
        }
        prev = block;
        // Переход к следующему блоку
        block = block->next_free;
    }

    return NULL;
}

void allocator_free(Allocator *const allocator, void *const memory) {
    if (!allocator || !memory) {
        return;
    }

    Block *block = (Block *)((uint8_t *)memory - sizeof(Block));
    block->is_free = true;

    // Добавляем блок обратно в список свободных блоков
    block->next_free = allocator->free_blocks;
    allocator->free_blocks = block;
}

void allocator_destroy(Allocator *const allocator) {
    if (allocator != NULL) {
    	munmap(allocator, allocator->size + sizeof(Allocator));
    }
}
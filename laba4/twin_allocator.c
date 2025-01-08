#include "twin_allocator.h"

Allocator* allocator_create(void *const memory, const size_t size) {
    if (memory == NULL || size < sizeof(Allocator) + sizeof(Block) ||
        ((size - sizeof(Allocator) & (size - sizeof(Allocator) - 1)) != 0)) {
        return NULL;
    }
    Allocator* allocator = (Allocator*) memory;
    allocator->memory = memory;
    allocator->size = size;

    Block* first = (Block*) (memory + sizeof(Allocator));
    // память под аллокатор 2^n + sizeof(Allocator)
    first->size = size - sizeof(Allocator) - sizeof(Block);
    first->next = NULL;
	first->is_free = true;
    allocator->list = first;
    return allocator;
}

void* allocator_alloc(Allocator *const allocator, const size_t size) {
    if (allocator == NULL || size <= 0) {
        return NULL;
    }

    Block* current = allocator->list;
    Block* best = NULL;
    // ищем наиболее подходящий блок
    while(current) {
        if (current->size >= size && current->is_free) {
            if (best == NULL || current->size - size < best->size - size) {
                best = current;
            }
        }
        current = current->next;
    }
    if (best == NULL) {
        return NULL;
    }

    // если новый выделенный блок больше нужного и есть место под еще один новый блок, то создадим еще один
    while ((best->size + sizeof(Block) > size + sizeof(Block)) && (size + sizeof(Block) <= (best->size + sizeof(Block)) / 2)) {
        // новый размер текущего блока
        best->size = (sizeof(Block) + best->size) / 2 - sizeof(Block);
        // блок двойник
        Block* buddy = (Block*) ((uintptr_t) best ^ (best->size + sizeof(Block)));
        buddy->size = best->size;
    	buddy->is_free = true;
        buddy->next = best->next;
        best->next = buddy;
    }

	best->is_free = false;
    // важно соблюдать арифметику указателей
    return (void*) best + sizeof(Block);
}

void allocator_free(Allocator *const allocator, void *const memory) {
	if (allocator == NULL || memory == NULL) {
		return;
	}
	Block* current = (Block*)(memory - sizeof(Block));
	current->is_free = true;
	// объединим свободные блоки
	// если buddy свободен(размер полученного buddy будет равен размеру текущего и is_free == true), то объединяем
	// у каждого блока есть buddy, кроме случая когда всего один блок
	Block* buddy = (Block*) ((uintptr_t) current ^ (current->size + sizeof(Block)));
	while(current->size != allocator->size - sizeof(Allocator) - sizeof(Block) &&
		  current->size == buddy->size && buddy->is_free) {
		// если current слева
		if (current->next == buddy) {
			current->next = buddy->next;
			current->size = current->size + sizeof(Block) + buddy->size;
		} else {
			buddy->next = current->next;
			buddy->size = buddy->size + sizeof(Block) + current->size;
			current = buddy;
		}
		buddy = (Block*) ((uintptr_t) current ^ (current->size + sizeof(Block)));
	}
}

void allocator_destroy(Allocator *const allocator) {
    if (allocator != NULL) {
        munmap(allocator->memory, allocator->size);
    }
}
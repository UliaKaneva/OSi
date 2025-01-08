#include <sys/mman.h>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


typedef struct Allocator Allocator;

// static привязывает allocator_{func} только для этого файла
static Allocator* (*allocator_create)(void* const memory, const size_t size);
static void* (*allocator_alloc)(Allocator* const allocator, const size_t size);
static void (*allocator_destroy)(Allocator* const allocator);
static void (*allocator_free)(Allocator* const allocator, void* const memory);

Allocator* allocator_create_stub(void *const memory, const size_t size) {
    (void) memory;
    (void) size;
    return NULL;
}

void* allocator_alloc_stub(Allocator *const allocator, const size_t size) {
    (void) allocator;
    (void) size;
    return NULL;
}

void allocator_free_stub(Allocator *const allocator, void *const memory) {
	(void) memory;
	(void) allocator;
}

void allocator_destroy_stub(Allocator *const allocator) {
    (void) allocator;
}

int main(int argc, char** argv) {
    void* library = NULL;
    char ans[100];
    if (argc < 2) {
        library = dlopen("/mnt/d/CLionProjects/OSi/cmake-build-debug/libblocks2n_allocator.so", RTLD_LOCAL | RTLD_NOW);
    }
    else {
        library = dlopen(argv[1], RTLD_LOCAL | RTLD_NOW);
    }

    if (library == NULL) {
        printf("Error: %s\n", dlerror());
        allocator_create = allocator_create_stub;
        allocator_destroy = allocator_destroy_stub;
        allocator_free = allocator_free_stub;
        allocator_alloc = allocator_alloc_stub;

    } else {
        allocator_create = dlsym(library, "allocator_create");
        if (allocator_create == NULL) {
            allocator_create = allocator_create_stub;
        }
        allocator_destroy = dlsym(library, "allocator_destroy");
        if (allocator_destroy == NULL) {
            allocator_destroy = allocator_destroy_stub;
        }
        allocator_free = dlsym(library, "allocator_free");
        if (allocator_free == NULL) {
            allocator_free = allocator_free_stub;
        }
        allocator_alloc = dlsym(library, "allocator_alloc");
        if (allocator_alloc == NULL) {
            allocator_alloc = allocator_alloc_stub;
        }
    }

    void* memory = mmap(0, 1048, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    Allocator* allocator = allocator_create(memory, 1048);
    if (allocator == NULL) {
        write(1, "Error creating allocator\n", strlen("Error creating allocator\n"));
        return 1;
    }

    int* numbers = allocator_alloc(allocator, sizeof(int));
    if (numbers == NULL) {
        write(1, "Allocation error\n", strlen("Allocation error\n"));
        return 1;
    }

    *numbers = 5;
    snprintf(ans, sizeof(ans), "%d\n", *numbers);
    write(1, ans, 2);

    allocator_free(allocator, numbers);

    char* str = allocator_alloc(allocator, 4000);
    if (str == NULL) {
        write(1, "Allocation error\n", strlen("Allocation error\n"));
        return 1;
    }
    str[0] = 'H';
    str[1] = 'e';
    str[2] = 'l';
    str[3] = 'l';
    str[4] = 'o';
    str[5] = '\n';
	str[6] = '\0';
    write(1, str, strlen(str));

    allocator_destroy(allocator);

    if (library != NULL) {
        dlclose(library);
    }

    return 0;
}
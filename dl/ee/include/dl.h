#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <elf.h>

struct module_t;

struct symbol_t {
    union {
        struct module_t* module; // for local symbol
        struct symbol_t* next;   // for global symbol
    };
    char* name;
    uint32_t info;
    void* address;
};

struct loader_context_t {
    size_t dynamic_count;
    Elf32_Dyn* dynamic;
    size_t dynsym_count;
    Elf32_Sym* dynsym;
    char* dynstr;

    uint32_t rld_version;
    uint32_t flags;
    uintptr_t base_address;
    uint32_t local_gotno;
    uint32_t symtabno;
    uint32_t gotsym;
    uintptr_t rld_map;
    uintptr_t pltgot;
};

struct module_t {
    struct module_t* next;
    struct symbol_t* symbols;

    char* name;
    char** dependencies;
    
    struct loader_context_t loader_context;

    size_t size;
    uint8_t base[];
};

typedef int (*_start_t)(int, char**);

void* dlopen(const char* filename, int flags);
void* dlsym(void* handle, const char* symbol);
int dlclose(void* handle);
const char* dlerror(void);

void dl_raise(const char* msg);

struct module_t* dl_load_module(FILE* file);
void dl_resolve_module(struct module_t* module);
struct module_t* dl_allocate_module(size_t size);
int dl_free_module(struct module_t* module);
struct symbol_t* dl_module_find_symbol(struct module_t* module, const char* name);

int dl_add_global_symbol(const char* name, void* address, uint32_t info);
void dl_remove_global_symbol(const char* name, uint32_t info);
struct symbol_t* dl_find_global_symbol(const char* name);
void dl_dump_global_symbols();
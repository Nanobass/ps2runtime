#include <dl.h>

#include <kernel.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

const char* dl_last_error = NULL;

struct module_t* modules = NULL, *modules_tail = NULL;
struct symbol_t* global_symbols = NULL, *global_symbols_tail = NULL;

void dl_raise(const char* msg) {
    if (msg) {
        dl_last_error = msg;
    } else {
        dl_last_error = "unknown error";
    }
}

struct module_t* dl_allocate_module(size_t size) {
    struct module_t* module = (struct module_t*)malloc(sizeof(struct module_t) + size);
    if (!module) {
        return NULL;
    }
    memset(module, 0, sizeof(struct module_t) + size);
    module->size = size;
    if(!modules) {
        modules = module;
        modules_tail = module;
    } else {
        modules_tail->next = module;
        modules_tail = module;
    }
    return module;
}

int dl_free_module(struct module_t* module) {
    if (!module) {
        dl_raise("invalid module");
        return -1;
    }

    struct module_t* current = modules;
    struct module_t* previous = NULL;

    while (current) {
        if (current == module) {
            if (previous) {
                previous->next = current->next;
            } else {
                modules = current->next;
            }
            if (modules_tail == current) {
                modules_tail = previous;
            }
            free(current->symbols);
            free(current->dependencies);
            free(current);
            return 0;
        }
        previous = current;
        current = current->next;
    }
    dl_raise("module not found");
    return -1;
}

int dl_add_global_symbol(const char* name, void* address, uint32_t info)
{
    if (!name || !address) {
        dl_raise("invalid name or address");
        return -1;
    }

    struct symbol_t* symbol = (struct symbol_t*)malloc(sizeof(struct symbol_t));
    if (!symbol) {
        dl_raise("out of memory for global symbol");
        return -1;
    }

    symbol->name = strdup(name);
    if (!symbol->name) {
        free(symbol);
        dl_raise("out of memory for global symbol name");
        return -1;
    }
    symbol->address = address;
    symbol->info = info;
    symbol->next = NULL;

    if (!global_symbols) {
        global_symbols = symbol;
        global_symbols_tail = symbol;
    } else {
        global_symbols_tail->next = symbol;
        global_symbols_tail = symbol;
    }

    printf("global symbol added: %s at %p (info: %ld)\n", name, address, info);

    return 0;
}

void dl_remove_global_symbol(const char* name, uint32_t info)
{
    if (!name) {
        dl_raise("invalid name");
        return;
    }

    struct symbol_t* current = global_symbols;
    struct symbol_t* previous = NULL;

    while (current) {
        if (strcmp(current->name, name) == 0 && current->info == info) {
            if (previous) {
                previous->next = current->next;
            } else {
                global_symbols = current->next;
            }
            if (global_symbols_tail == current) {
                global_symbols_tail = previous;
            }

            printf("global symbol removed: %s at %p (info: %ld)\n", current->name, current->address, current->info);

            free(current->name);
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
    dl_raise("global symbol not found");
}

struct symbol_t* dl_find_global_symbol(const char* name)
{
    if (!name) {
        dl_raise("invalid name");
        return NULL;
    }

    struct symbol_t* symbol = NULL;
    for(struct symbol_t* current = global_symbols; current; current = current->next) {
        if (strcmp(current->name, name) == 0) {
            if(!symbol) {
                symbol = current;
                continue;
            }
            if(ELF32_ST_BIND(symbol->info) == STB_WEAK && ELF32_ST_BIND(current->info) == STB_WEAK) {
                printf("warning: multiple weak symbols found for '%s', using first one\n", name);
                continue;
            }
            if(ELF32_ST_BIND(symbol->info) == STB_WEAK && ELF32_ST_BIND(current->info) == STB_GLOBAL) {
                symbol = current;
                return symbol;
            }
        }
    }
    return symbol;
}

void dl_dump_global_symbols() {
    struct symbol_t* current = global_symbols;
    printf("global symbols:\n");
    while (current) {
        printf("  %s: %p (info: %ld)\n", current->name, current->address, current->info);
        current = current->next;
    }
}
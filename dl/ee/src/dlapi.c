#include <dl.h>

#include <stdlib.h>
#include <stdio.h>

void* dlopen(const char* filename, int flags) {
    (void) flags;

    FILE* elf_handle = fopen(filename, "rb");
    if (!elf_handle) {
        dl_raise("cannot open file");
        return NULL;
    }
    struct module_t* module = dl_load_module(elf_handle);
    fclose(elf_handle);
    if(!module) {
        return NULL;
    }
    return module;
}

void* dlsym(void* handle, const char* symbol) {
    if(!symbol) {
        dl_raise("invalid symbol name");
        return NULL;
    }
    struct symbol_t* sym = NULL;
    if(!handle) {
        sym = dl_find_global_symbol(symbol);
    } else {
        sym = dl_module_find_symbol((struct module_t*)handle, symbol);
    }
    if(sym) {
        return sym->address;
    } else {
        dl_raise("symbol not found");
        return NULL;
    }
}

int dlclose(void* handle) {
    return dl_free_module((struct module_t*)handle);
}

const char* dlerror(void) {
    extern const char* dl_last_error;
    const char* error = dl_last_error;
    dl_last_error = NULL;
    return error;
}
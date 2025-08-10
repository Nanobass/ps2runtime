//=============================================================================
// _____     ___ ____  <insert cool logo here>
//  ____|   |    ____| <insert cool logo here>
// |     ___|   |____  <insert cool logo here>
// ---------------------------------------------------
//
// File:        main.cpp
//
// Description: ps2 operating system runtime
//
//=============================================================================

/* standard library */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

/* ps2sdk */
#include <elf.h>

#include <kernel.h>

#include <dl.h>


int main(int argc, char* argv[]) {

    dl_add_global_symbol("printf", (void*)printf, ELF32_ST_INFO(STB_GLOBAL, STT_FUNC));

    struct module_t* handle = dlopen("libfiler.so", 0);

    const char* error = dlerror();
    if (error) {
        printf("dlopen error: %s\n", error);
        SleepThread();
    }

    _start_t _start = (_start_t) dlsym(handle, "_start");

    error = dlerror();
    if (error) {
        printf("dlsym error: %s\n", error);
        SleepThread();
    }

    int result = _start(argc, argv);
    printf("start function: %d\n", result);

    dlclose(handle);

    SleepThread();
    return 0;
}
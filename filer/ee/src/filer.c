//=============================================================================
// _____     ___ ____  # insert another cool logo here
//  ____|   |    ____| # insert another cool logo here
// |     ___|   |____  # insert another cool logo here
// ---------------------------------------------------
//
// File:        filer.cpp
//
// Description: filesystem manager library
//
//=============================================================================

//========================================
// System Includes
//========================================

#include <stdio.h>

//========================================
// PS2SDK Includes
//========================================


//========================================
// Project Includes
//========================================

//========================================
// Definitions
//========================================

#define EXPORT __attribute__((visibility("default")))

EXPORT char* erl_id = "libfiler";
EXPORT char* erl_dependencies[] = { "libc", "libkernel", "liblua", 0 };
EXPORT char* erl_copyright = "Copyright (C) 2025 nanobass";
EXPORT char* erl_version = "1.0.0";

char data[256];

extern int printf(const char* format, ...);

#define mprintf(...) printf("filer: " __VA_ARGS__)

EXPORT int filer_reset_iop() {
    mprintf("resetting IOP...\n");
    mprintf("id %s\n", erl_id);
    return 0;
}

EXPORT int filer_device_control(const char* device_name, int cmd, int len, void* buf) {
    mprintf("controlling device %s with cmd %d, len %d\n", device_name, cmd, len);
    return 0;
}

EXPORT int filer_module_control(const char* module_name, int cmd, int len, void* buf) {
    mprintf("controlling module %s with cmd %d, len %d\n", module_name, cmd, len);
    return 0;
}

EXPORT int filer_shutdown() {
    mprintf("shutting down filer...\n");
    return 0;
}

EXPORT char* getid() {
    return erl_id;
}

/**
 * @brief Entry point for the filer library.
 * 
 * This function is called when the library is loaded. It initializes the library
 * and returns a status code.
 * 
 * default behavior is to load the library with all modules and devices
 * specified in the systems configuration file.
 * 
 * @param argc Number of arguments.
 * @param argv Array of argument strings.
 * @return int Status code (0 for success).
 */
EXPORT int _start(int argc, char *argv[]) {
    mprintf("version %s %s\n", erl_version, erl_copyright);
    for(int i = 0; i < argc; i++) {
        mprintf("  argument %d: %s\n", i, argv[i]);
    }
    char** dep = erl_dependencies;
    while(*dep) {
        mprintf("  dependency: %s\n", *dep);
        dep++;
    }
    return 69;
}
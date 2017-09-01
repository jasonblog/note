// gcc example.c -ldl -rdynamic
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

void foo() {
    printf("called foo()\n");
}

void bar() {
    printf("called bar()\n");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "USAGE: %s [func]\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Load the function/variable symbol table from executable. */
    void* handle = dlopen(NULL, RTLD_LAZY);

    if (!handle) {
        fprintf(stderr, "ERROR: Failed to load executable\n");
        return EXIT_FAILURE;
    }

    /* Find the function */
    void* func = dlsym(handle, argv[1]);

    if (!func) {
        fprintf(stderr, "ERROR: Function not found: %s\n", argv[1]);
    } else {
        ((void (*)())func)();
    }

    dlclose(handle);
    return EXIT_SUCCESS;
}

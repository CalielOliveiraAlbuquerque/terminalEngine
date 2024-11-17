#define REALLOC_OR_DIE(pointer, size, file, line)\
{\
    void* result = realloc(pointer, size);\
    if(!result){\
        printf("Failed to allocate memory in file %s in line %d", file, line);\
        exit(1);\
    }\
    pointer = result;\
}

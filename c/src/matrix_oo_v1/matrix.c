#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define SMART_VAR(type, name, initial) type name __attribute__((cleanup(freeVar))) = (initial)
#define PERSIST_VAR(type, name, initial) type name = initial; \
    persist_va_list *ls = getList(); ls->data = name
#define INIT_FLT(data, sz, eps, val) INIT_FLT_PHASE1(data, sz, eps, val, __LINE__)
#define INIT_FLT_PHASE1(data, sz, eps, val, seed) INIT_FLT_PHASE2(data, sz, eps, val, seed)
#define INIT_FLT_PHASE2(data, sz, eps, val, seed) SMART_VAR(float *, vl##seed, malloc(sizeof(float) * sz[0] * sz[1])); \
    do { \
        for(int i = 0; i < sz[0]; ++i) for(int j = 0; j < sz[1]; ++j) vl##seed[i*sz[1]+j] = val[i][j]; \
        data.magic = 1; \
        data.size[0] = sz[0]; \
        data.size[1] = sz[1]; \
        data.epsilon = eps; \
        data.values = vl##seed; \
    } while(0)

#define INIT_EQUAL(matrix, equ) bool equal##__LINE__(void *input){ \
        return equ(matrix.priv, input);} \
    matrix.equal = equal##__LINE__

#define INIT_MULTIPLY(matrix, ml) Matrix_Flt multiply##__LINE__(void *input){ \
        return ml(matrix.priv, input);} \
    matrix.mul = multiply##__LINE__

#define INIT_FUNCTION(matrix, equ, ml) INIT_EQUAL(matrix, equ); INIT_MULTIPLY(matrix, ml)

#define INIT_MATRIX(matrix, data, equ, ml) matrix.priv = &data; \
    INIT_FUNCTION(matrix, equ, ml)

#define CREATE_MATRIX(name, sz, eps, val, equ, ml) Matrix_Flt data##__LINE__; \
    INIT_FLT(data##__LINE__, sz, eps, val); \
    Matrix name; INIT_MATRIX(name, data##__LINE__, equ, ml)

typedef struct __persist_va_list persist_va_list;
struct __persist_va_list {
    void* data;
    persist_va_list* next;
};

/**
 * This structure is one possible implementation of matrix operation data.
 * It is used to carry necessary information in calculations.
 * The first byte is used to store type value and distinguish different implementations,
 * all the implementations should follow this format.
 */
typedef struct {
    char magic;     ///> nust be the first member to do RTTI
    int size[2];
    float epsilon;
    float* values;
} Matrix_Flt;

typedef struct __matrix_impl Matrix;
struct __matrix_impl {
    void* priv;

    /* operations */
    bool (*equal)(void* input);
    Matrix_Flt(*mul)(void* input);
};

static inline void freeVar(void* ptr)
{
    free(*(void**)ptr);
}

static inline char getMagic(void* ptr)
{
    return *(char*)ptr;
}

static persist_va_list* listHead = NULL;
static persist_va_list* getList()
{
    persist_va_list** list = &listHead;

    while (*list != NULL) {
        list = &((*list)->next);
    }

    *list = malloc(sizeof(persist_va_list));
    (*list)->data = NULL;
    (*list)->next = NULL;
    return *list;
}

static void delList(persist_va_list* list)
{
    if (list != NULL) {
        if (list->next) {
            delList(list->next);
        }

        free(list->data);
        free(list);
    }
}

/**
 * Do not assign this function to the interface directly,
 * Use INIT_MULTIPLY macro to init the interface so that you can call it more naturally
 * like bool = matA.equal(matB)
 */
static bool equal(void* self, void* input)
{
    if (getMagic(self) != getMagic(input)) { ///< different implementation
        return false;
    }

    Matrix_Flt* selfMat = (Matrix_Flt*)self;
    Matrix_Flt* inMat = (Matrix_Flt*)input;

    if (selfMat->size[0] != inMat->size[0] ||
        selfMat->size[1] != inMat->size[1]) {
        return false;
    }

    float tolerance = selfMat->epsilon; ///< this is the implementation of self

    for (int i = 0; i < selfMat->size[0] * selfMat->size[1]; ++i) {
        if (selfMat->values[i] + tolerance < inMat->values[i] ||
            inMat->values[i] + tolerance < selfMat->values[i]) {
            return false;
        }
    }

    return true;
}

/**
 * Do not assign this function to the interface directly,
 * Use INIT_EQUAL macro to init the interface so that you can call it more naturally
 * like matO = matA.mul(matB)
 */
static Matrix_Flt mul(void* self, void* input)
{
    if (getMagic(self) != getMagic(input))
        return (Matrix_Flt) {};

    Matrix_Flt* selfMat = (Matrix_Flt*)self;

    Matrix_Flt* inMat = (Matrix_Flt*)input;

    if (selfMat->size[1] != inMat->size[0])
        return (Matrix_Flt) {};

    PERSIST_VAR(float*, mat,
                malloc(sizeof(float) * selfMat->size[0] * inMat->size[1]));

    for (int i = 0; i < selfMat->size[0]; i++) {
        for (int j = 0; j < inMat->size[1]; j++) {
            mat[i * inMat->size[1] + j] = 0.0;

            for (int k = 0; k < selfMat->size[1]; k++)
                mat[i * inMat->size[1] + j] += ((float*)selfMat->values)[i * selfMat->size[1] +
                                               k] *
                                               ((float*)inMat->values)[k * inMat->size[1] + j];
        }
    }

    return (Matrix_Flt) {
        .values = mat, .size[0] = selfMat->size[0], .size[1] = inMat->size[1]
    };
}

__attribute__((destructor)) static void varDestruct()
{
    delList(listHead);
}

int main()
{
    int size[2] = {2, 2};
    /** TODO:make the initialization of matrix values easier */
    float value[2][2] = {{1.0, 2.0}, {3.0, 4.0}};
    float epsilon = 0.0001;
    CREATE_MATRIX(matrix, size, epsilon, value, equal, mul);

    int size1[2] = {2, 4};
    float value1[2][4] = {{1.0, 2.0, 3.0, 4.0}, {4.0, 5.0, 6.0, 7.0}};
    Matrix_Flt input;
    INIT_FLT(input, size1, epsilon, value1);

    printf("is equal:%d\n", matrix.equal(&input));

    Matrix_Flt result = matrix.mul(&input);

    for (int i = 0; i < result.size[0]; ++i) {
        printf("{");

        for (int j = 0; j < result.size[1]; ++j) {
            printf("%f  ", ((float*)result.values)[i * result.size[1] + j]);
        }

        printf("},\n");
    }

    return 0;
}

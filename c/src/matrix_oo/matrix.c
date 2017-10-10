#include <stdbool.h>

typedef struct __matrix_impl Matrix;
struct __matrix_impl {
    float values[4][4];

    /* operations */
    bool (*equal)(const Matrix, const Matrix);
    Matrix (*mul)(const Matrix, const Matrix);
};

static const float epsilon = 1 / 10000.0;

static bool equal(const Matrix a, const Matrix b)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (a.values[i][j] + epsilon < b.values[i][j]
                    || b.values[i][j] + epsilon < a.values[i][j])
                return false;
    return true;
}

static Matrix mul(const Matrix a, const Matrix b)
{
    Matrix matrix = { .values = {
            { 0, 0, 0, 0, },
            { 0, 0, 0, 0, },
            { 0, 0, 0, 0, },
            { 0, 0, 0, 0, },
        },
    };

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                matrix.values[i][j] += a.values[i][k] * b.values[k][j];

    return matrix;
}

int main()
{
    Matrix m = {
        .equal = equal,
        .mul = mul,
        .values = {
            { 1, 2, 3, 4, },
            { 5, 6, 7, 8, },
            { 1, 2, 3, 4, },
            { 5, 6, 7, 8, },
        },
    };

    Matrix n = {
        .equal = equal,
        .mul = mul,
        .values = {
            { 1, 2, 3, 4, },
            { 5, 6, 7, 8, },
            { 1, 2, 3, 4, },
            { 5, 6, 7, 8, },
        },
    };

    Matrix o = { .mul = mul, };
    o = o.mul(m, n);

    return m.equal(o, (Matrix) {
        .values = {
            { 34,  44,  54,  64, },
            { 82, 108, 134, 160, },
            { 34,  44,  54,  64, },
            { 82, 108, 134, 160, },
        },
    });
}

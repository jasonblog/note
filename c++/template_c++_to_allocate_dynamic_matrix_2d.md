# template c++ to allocate dynamic matrix 2D


```cpp
#include <vector>
#include <iostream>
using namespace std;

template <class T>
class SimpleMatrix
{
public:
    SimpleMatrix(int rows, int cols, const T& initVal = T())
        : m_data(rows * cols, initVal)
        , m_rows(rows)
        , m_cols(cols)
    {
    }

    // Direct vector access and indexing
    operator const vector<T>& () const
    {
        return m_data;
    }
    int Index(int row, int col) const
    {
        return row * m_cols + col;
    }

    // Get a single value
    T& Value(int row, int col)
    {
        return m_data[Index(row, col)];
    }
    const T& Value(int row, int col) const
    {
        return m_data[Index(row, col)];
    }

    // Proxy structure to allow [][] indexing
    struct Proxy {
    private:
        friend class SimpleMatrix<T>;
        SimpleMatrix<T>* m_matrix;
        int m_row;
        Proxy(SimpleMatrix<T>* m, int row) : m_matrix(m), m_row(row) {}
    public:
        T& operator[](int col)
        {
            return m_matrix->Value(m_row, col);
        }
        const T& operator[](int col) const
        {
            return m_matrix->Value(m_row, col);
        }
    };

    Proxy operator[](int row)
    {
        return Proxy(this, row);
    }
    const Proxy operator[](int row) const
    {
        return Proxy(const_cast<SimpleMatrix<T>*>(this), row);
    }

private:
    vector<T> m_data;
    int m_rows;
    int m_cols;
};

int main()
{
    SimpleMatrix<int> m(10, 2);
    const SimpleMatrix<int>& cm = m;
    m[1][1] = 1;

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 2; ++j) {
            m[i][j] = i*j;
        }
    }

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 2; ++j) {
            cout << cm[i][j] << ", ";
        }
        cout << endl;
    }
    
    return 0;
}
```

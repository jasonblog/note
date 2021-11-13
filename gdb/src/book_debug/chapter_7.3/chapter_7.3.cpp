#include <iostream>
template <class T>
class CMatrix
{
public:	   	
	CMatrix(int r, int c, T* value)
	{
		row = r;
		col = c;
		data = new T[r * c];
		for (int i = 0; i < r * c; i++)
		{
			data[i] = value[i];
		}
	}
	CMatrix(const CMatrix& mat)
	{
		row = mat.row;
		col = mat.col;
		data = new T[row * col];
		for (int i = 0; i < row * col; i++)
		{
			data[i] = mat.data[i];
		}
	}
	virtual ~CMatrix()
	{
		delete[]data;
	}
	static T getData(CMatrix& mat, int r, int c)
	{
		if (r < 0 || c < 0 || r > mat.row - 1 || c> mat.col - 1)
			return 0;
		return mat.data[r * mat.col + c];
	}
	static CMatrix<T> convMat(CMatrix& a, CMatrix& h)
	{
		int roffset = h.row / 2;
		int coffset = h.col / 2;
		CMatrix<T> c(a);

		for (int i = 0; i < a.row; i++)
		{
			for (int j = 0; j < a.col; j++)
			{
				c.data[i * c.col + j] = 0;

				T val = 0;
				for (int k = 0; k < h.row; k++)
				{
					for (int m = 0; m < h.col; m++)
					{
						T data = getData(a, k + i - roffset, m + j - coffset);
						T hdata = h.data[k * h.col + m];
						T temp = hdata * data;
						val += temp;
					}
				}
				c.data[i * c.col + j] = val;
			}
		}
		return c;
	}
	void show()
	{
		std::cout << "矩阵为:\n";
		for (int i = 0; i < row * col; i++)
		{
			std::cout << data[i] << "\t";
			if ((i + 1) % col == 0)
				std::cout << std::endl;
		}

	}
private:
	int row;
	int col;
	T* data;
};
int main()
{
	std::cout << "请选择:\n1:计算矩阵卷积2:退出程序!\n";
	int arr[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
	/*
	假设harr矩阵已经是旋转了180度后的矩阵
	*/
	int harr[] = { -1,-1,-1,-1,9,-1,-1,-1,-1 };
	CMatrix<int>hmat(3, 3, harr);
	CMatrix<int> conmat(4, 4, arr);
	while (true)
	{
		switch (getchar())
		{
		case '1':
			{
				CMatrix <int>conres = CMatrix<int>::convMat(conmat, hmat);
				conres.show();
			}
			break;
		case '2':
			return 0;
		}
	}
}


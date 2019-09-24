# Opencv 讀寫圖檔基本操作


```cpp
#ifndef CVX_IMAGE_300_H
#define CVX_IMAGE_300_H 1

#include "opencv2/core/core.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <iostream>

using std::ostream;
using std::endl;
using std::cout;
using cv::Mat;

namespace cvx
{
	template<class T> class Image
	{
		inline friend ostream& operator<<(ostream& os, Image<T>& image)
		{
			if(image.m_imgp->nChannels != 1)
			{
				return os;
			}
			int height = image.m_imgp->height;
			int width = image.m_imgp->width;
			int x, y;
			if(image.m_imgp->nChannels == 1)
			{
				for(y=0; y<height; ++y)
				{
					for(x=0; x<width; ++x)
					{
						os<<(T)image[y][x]<<" ";
					}
					os<<endl;
				}
			}			
			return os;
		}
	private:
		const IplImage* m_imgp;
		char   ** m_row;
	public:
		Image(const IplImage* img = 0)
		{
			m_imgp = img;
			m_row = new char *[m_imgp->height];
			for (int i = 0; i<m_imgp->height; ++i) {
				m_row[i] = m_imgp->imageData + i * m_imgp->widthStep;
			}
		}
		~Image(){m_imgp = 0; delete []m_row;}
		inline T* operator[](const int rowIndex)
		{
			return ((T*)(m_row[rowIndex]));
		}
	};

	template<class T> class MatImage
	{		
	private:
		const Mat* m_matp;
		unsigned char   ** m_row;
	public:
		MatImage(const Mat* mat = 0)
		{
			assert(mat);
			assert(!(mat->empty()));
			assert(mat->dims == 2);
			m_matp = mat;
			m_row = new unsigned char *[m_matp->rows];
			for (int i = 0; i<m_matp->rows; ++i) {
				m_row[i] = m_matp->data + i * (m_matp->step.p[0]);
			}
		}
		~MatImage(){m_matp = 0; delete []m_row;}
		inline T* operator[](const int rowIndex)
		{
			return ((T*)(m_row[rowIndex]));
		}
	};

	typedef struct{
		unsigned char b,g,r;
	}RgbPixel;

	typedef struct{
		unsigned char a,b,g,r;
	}RgbaPixel;

	typedef struct{
		unsigned char b,g,r,a;
	}ArgbPixel;

	
	typedef struct{
		short int b,g,r;
	}RgbPixelShortInt;

	typedef struct{
		unsigned short int b,g,r;
	}RgbPixelUShortInt;

	typedef struct{
		int b,g,r;
	}RgbPixelInt;

	typedef struct{
		float b,g,r;
	}RgbPixelFloat;

	typedef struct {
		unsigned char h,s,v;
	}HsvPixel;

	typedef struct {
		unsigned char y, u, v;
	}YuvPixel;

	typedef struct{
		float b,g,r,a;
	}RgbaPixelFloat;

	typedef struct{
		unsigned char l,a,b;
	}LabPixel;


	/*color image*/
	typedef Image<RgbPixel>      RgbImage;
	typedef Image<HsvPixel>      HsvImage;
	typedef Image<YuvPixel>      YuvImage;
	typedef Image<RgbPixelShortInt> RgbImageShortInt;
	typedef Image<RgbPixelInt>      RgbImageInt;
	typedef Image<RgbPixelFloat>    RgbImageFloat;
	typedef Image<RgbaPixelFloat>   RgbaImageFloat;
	typedef Image<LabPixel>         LabImage;
	
	/*gray image*/
	typedef Image<unsigned char> BwImage;
	typedef Image<short int> BwImageShortInt;
	typedef Image<float> BwImageFloat;
	typedef Image<int> BwImageInt;

	/*color mat image*/
	typedef MatImage<RgbPixelUShortInt>  Rgb16UImage;
	typedef MatImage<unsigned short int> Bw16UImage;
	typedef MatImage<short int>          Bw16SImage;
	typedef MatImage<RgbPixel>           Rgb8UImage;
	typedef MatImage<ArgbPixel>          Argb8UImage;
	typedef MatImage<unsigned char>      Bw8UImage;
	typedef MatImage<RgbPixelFloat>		 Rgb32FImage;
	typedef MatImage<float>				 Bw32FImage;
	typedef MatImage<double>	         Bw64FImage;
	typedef MatImage<int>				 Bw32SImage;
	typedef MatImage<HsvPixel>           Hsv8UImage;
//	typedef MatImage<RgbaPixel>          Rgba8UImage;


	/*6  3  7
	  2  x  0
	  5	 1  4*/
	typedef struct{
		int d[8];
	}ENeighborInt;

	typedef struct{
		int left;
		int down;
		int leftDown;
		int rightDown;
	}LrtdPixel;

	/*eight neighborhood */
	typedef Image<ENeighborInt> ENImageInt;
	typedef Image<LrtdPixel> FNImageInt;
}

#endif
```
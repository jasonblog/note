package ch14;

import java.util.ArrayList;
import java.util.List;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;

public class Ch14_1_1Dft
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        try {
            Mat source = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg",
                                          Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);

            /////////////part1 optimizeImageDim
            // optimize the dimension of the loaded image
            // init
            Mat padded = new Mat();
            // get the optimal rows size for dft
            int addPixelRows = Core.getOptimalDFTSize(source.rows());
            // get the optimal cols size for dft
            int addPixelCols = Core.getOptimalDFTSize(source.cols());
            // apply the optimal cols and rows size to the image

            //當影像的尺寸是2， 3，5的整數倍時，計算速度最快。為了達到快速計算，
            //將輸入圖像加框擴展到最佳的尺寸 , 添加的像素初始化為0
            Core.copyMakeBorder(source, padded, 0, addPixelRows - source.rows(), 0,
                                addPixelCols - source.cols(), 1, Scalar.all(0));
            // return padded;
            //end part1 optimizeImageDim
            //System.out.println(padded.dump());

            //main
            //分配傅立葉變換的實部和虛部存儲空間兩個圖像值. 傅立葉變換的結果是複數， 因此至少要將頻域儲存在 float 格式，並多加一個額外通道來儲存複數部分：
            padded.convertTo(padded, CvType.CV_32F);
            List<Mat> planes = new ArrayList<Mat>();
            Mat complexImage = new Mat();
            // prepare the image planes to obtain the complex image
            planes.add(padded);
            planes.add(Mat.zeros(padded.size(), CvType.CV_32F));

            // prepare a complex image for performing the dft
            ////為擴展後的圖像增添一個初始化為0的通道
            Core.merge(planes, complexImage);
            // dft
            //進行離散傅立葉變換, 變換結果存在原始Mat矩陣中
            Core.dft(complexImage, complexImage);
            //Mat magnitude = new Mat();

            // optimize the image resulting from the dft operation
            /// part2 createOptimizedMagnitude
            // init

            List<Mat> newPlanes = new ArrayList<Mat>();
            Mat mag = new Mat();
            // split the comples image in two planes
            //將複數轉換為弧度.
            Core.split(complexImage, newPlanes);
            // compute the magnitude
            Core.magnitude(newPlanes.get(0), newPlanes.get(1), mag);
            // move to a logarithmic scale
            //轉換到對數的尺度
            Core.add(mag, Scalar.all(1), mag);
            Core.log(mag, mag);
            // optionally reorder the 4 quadrants of the magnitude image
            ////////////////part3 shiftDFT
            //裁剪奇數行與列
            mag = mag.submat(new Rect(0, 0, mag.cols() & -2, mag.rows() & -2));

            //重分佈傅立葉幅度4個象限圖
            int cx = mag.cols() / 2;
            int cy = mag.rows() / 2;

            //左上角 - 為每一個象限創建ROI
            Mat q0 = new Mat(mag, new Rect(0, 0, cx, cy));
            //右上角
            Mat q1 = new Mat(mag, new Rect(cx, 0, cx, cy));
            // 左下角
            Mat q2 = new Mat(mag, new Rect(0, cy, cx, cy));
            //右下角
            Mat q3 = new Mat(mag, new Rect(cx, cy, cx, cy));

            Mat tmp = new Mat();
            q0.copyTo(tmp);
            q3.copyTo(q0);
            //交換象限:左上與右下對調
            tmp.copyTo(q3);

            q1.copyTo(tmp);
            q2.copyTo(q1);
            //交換象限:右上與左下對調
            tmp.copyTo(q2);
            //end part3 shiftDFT

            // normalize the magnitude image for the visualization since both JavaFX
            // and OpenCV need images with value between 0 and 255
            //正規化至[0,255]範圍
            Core.normalize(mag, mag, 0, 255, Core.NORM_MINMAX);
            //end part2 createOptimizedMagnitude


            //Imgproc.GaussianBlur(source1, processBlur,new Size(GaussianKernelSize,GaussianKernelSize),0,0);
            //Mat lastoutput = ConvertDFTToImage(Dft,gaussianFilter,output);
            //source.convertTo(destination, -1, alpha, beta);
            Imgcodecs.imwrite("C://opencv3.1//samples//lena-dft.jpg", mag);
            //main end
        } catch (Exception e) {
            System.out.println("error: " + e.getMessage());
        }
    }
}

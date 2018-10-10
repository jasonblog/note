package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

public class Ch13_14_2PCAForIris
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        IrisDatabase iris = new IrisDatabase();
        Mat mean = new Mat();
        Mat vectors = new Mat();
        Mat allIrisData = iris.getAllDataMat().t();
        //System.out.println(allIrisData.dump());
        //System.out.println(allIrisData.size());


        Core.PCACompute(allIrisData, mean, vectors, 2);

        //System.out.println(vectors.dump());
        //System.out.println(vectors.size());


        Mat image2d = new Mat(700, 700, CvType.CV_8UC3, new Scalar(255, 255, 255));
        Mat image1d = new Mat(200, 1000, CvType.CV_8UC3, new Scalar(255, 255, 255));

        for (int i = 0; i < 150; i++) {
            Mat tempData = vectors.col(i);
            // System.out.println("---"+tempData.dump()+","+tempData.get(0,0)[0]+","+tempData.get(1,0)[0]);


            //為了方便在XY平面顯示,每個特徵很公平加權
            double x = tempData.get(0, 0)[0] * 1000 + 200;
            double y = tempData.get(1, 0)[0] * 1000 + 200;

            double x1 = tempData.get(0, 0)[0] * 9000;
            //System.out.println("x="+x+",y="+y);
            // System.out.println("x1="+x1);

            //1~35,106~120是setosa,36~70,121~135是versicolor,71~105,136~150是virginica
            if ((i < 35) || (i >= 105 && i < 120)) {
                //Iris-setosa用blue
                Imgproc.circle(image2d, new Point(x, y), 3, new Scalar(255, 0, 0));
                Imgproc.circle(image1d, new Point(x, 100), 1, new Scalar(255, 0, 0), 1);
            } else if ((i >= 35 && i < 70) || (i >= 120 && i < 135)) {
                //Iris-versicolor用green
                Imgproc.circle(image2d, new Point(x, y), 3, new Scalar(0, 255, 0));
                Imgproc.circle(image1d, new Point(x, 100), 1, new Scalar(0, 255, 0), 1);
            } else if ((i >= 70 && i < 105) || (i >= 135 && i < 150)) {
                //Iris-virginica用red
                Imgproc.circle(image2d, new Point(x, y), 3, new Scalar(0, 0, 255));
                Imgproc.circle(image1d, new Point(x, 100), 1, new Scalar(0, 0, 255), 1);
            }
        }

        Imgcodecs.imwrite("C://opencv3.1//samples//PCA-IrisFor2DMap.jpg", image2d);
        Imgcodecs.imwrite("C://opencv3.1//samples//PCA-IrisFor1DMap.jpg", image1d);
    }
}

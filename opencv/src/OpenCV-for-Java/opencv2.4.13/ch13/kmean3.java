package ch13;

import java.util.Random;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Range;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;

public class kmean3
{
    public static void main(String[] args) throws Exception{
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);

        //1Dªº

        int MAX_CLUSTERS = 5;

        //5²Õ
        Scalar colorTab[] =
        {
            new Scalar(0, 0, 255),
            new Scalar(0, 255, 0),
            new Scalar(255, 100, 100),
            new Scalar(255, 0, 255),
            new Scalar(0, 255, 255)
        };

        Mat img = new Mat(500, 500, CvType.CV_8UC3);
        //Random rand = new Random();

        //
        //  System.out.println(clusterCount.dump());
        //System.out.println(clusterCount.dump());



        //while(true){


        Mat mat1 = new Mat(25, 4, CvType.CV_32FC1);
        Mat mat2 = new Mat(25, 4, CvType.CV_32FC1);

        Core.randn(mat1, 123, 21);
        Core.randn(mat2, 3, 12);
        int i;

        Mat points = new Mat(200, 1, CvType.CV_32FC1);
        Core.add(mat1.reshape(1, 100), mat2.reshape(1, 100), points);
        Mat labels = new Mat();
        //clusterCount=Math.min(clusterCount, sampleCount);
        //Mat centers=new Mat(clusterCount, 1, points.type());
        Mat centers = new Mat(2, 1, points.type());

        TermCriteria criteria = new TermCriteria(TermCriteria.COUNT, 10, 1);
        Core.kmeans(points, 2, labels, criteria, 3, Core.KMEANS_PP_CENTERS);

        System.out.println("points=" + points.dump());
        System.out.println("points-size=" + points.total());
        System.out.println("points-col=" + points.cols());
        System.out.println("points-row=" + points.rows());
        System.out.println("centers=" + centers.dump());
        System.out.println("labels=" + labels.dump());


    }

}

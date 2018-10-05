package ch13;

import java.util.Random;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Range;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;

public class kmean
{
    public static void main(String[] args) throws Exception{
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        int MAX_CLUSTERS = 5;
        int clusterCount = 2;
        int dimensions = 5;
        int sampleCount = 50;
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
        //  Mat clusterCount = new Mat(22,1,CvType.CV_8UC1);//,sampleCount;
        //  Core.randn(clusterCount, 2, MAX_CLUSTERS+1);
        //
        //  System.out.println(clusterCount.dump());
        //System.out.println(clusterCount.dump());

        Mat points = new Mat(sampleCount, dimensions, CvType.CV_32F, new Scalar(10));
        Mat labels = new Mat();;
        Mat centers = new Mat(clusterCount, 1, points.type());

        for (int i = 24; i < points.rows(); i++)
        {
            for (int j = 0; j < points.cols(); j++) {
                // points.at<float>(i,j)=20;
                points.put(i, j, 20);
            }
        }

        TermCriteria criteria = new TermCriteria(TermCriteria.COUNT, 10, 1);
        // Core.kmeans(samples32f, k, labels, criteria, 1, Core.KMEANS_PP_CENTERS, centers);
        Core.kmeans(points, clusterCount, labels, criteria, 3, Core.KMEANS_PP_CENTERS, centers);
        System.out.println("points=" + points.dump());
        System.out.println("centers=" + centers.dump());
        System.out.println("labels=" + labels.dump());

    }

}

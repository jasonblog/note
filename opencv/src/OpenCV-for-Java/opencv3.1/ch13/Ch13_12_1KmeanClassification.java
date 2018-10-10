package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;

public class Ch13_12_1KmeanClassification
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    //聚類
    public static void main(String[] args) throws Exception{
        int clusterCount = 2;
        int dimensions = 5;
        int sampleCount = 50;


        Mat points = new Mat(sampleCount, dimensions, CvType.CV_32F, new Scalar(10));
        Mat labels = new Mat();;
        Mat centers = new Mat(clusterCount, 1, points.type());

        for (int i = 24; i < points.rows(); i++)
        {
            for (int j = 0; j < points.cols(); j++) {
                points.put(i, j, 20);
            }
        }

        TermCriteria criteria = new TermCriteria(TermCriteria.COUNT, 10, 1);
        //底下註解即可看出分2類
        //Core.kmeans(points, clusterCount, labels, criteria, 3, Core.KMEANS_PP_CENTERS, centers);
        System.out.println("points=" + points.dump());
        System.out.println("centers=" + centers.dump());
        System.out.println("labels=" + labels.dump());

    }

}

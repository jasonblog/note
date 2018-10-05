package ch13;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;
import org.opencv.highgui.Highgui;

public class kmeans1
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    static Mat img = Mat.zeros(200, 200, CvType.CV_8UC3);
    public static void main(String[] args)
    {

        Core.rectangle(img, new Point(0, 0), new Point(100, 200), new Scalar(0, 255, 0),
                       -1);
        Core.rectangle(img, new Point(100, 0), new Point(200, 200), new Scalar(0, 0,
                       255), -1);

        Mat clusters = cluster(img, 2).get(0);

    }

    public static List<Mat> cluster(Mat cutout, int k)
    {
        Mat samples = cutout.reshape(1, cutout.cols() * cutout.rows());//轉1行多列的mat
        Mat samples32f = new Mat();//for kmeans使用
        samples.convertTo(samples32f, CvType.CV_32F, 1.0 / 255.0);//資料轉換成kmean使用

        Mat labels = new Mat();
        TermCriteria criteria = new TermCriteria(TermCriteria.COUNT, 100, 1);
        Mat centers = new Mat();
        Core.kmeans(samples32f, k, labels, criteria, 1, Core.KMEANS_PP_CENTERS,
                    centers);
        return showClusters(cutout, labels, centers);
    }

    private static List<Mat> showClusters(Mat cutout, Mat labels, Mat centers)
    {
        centers.convertTo(centers, CvType.CV_8UC1, 255.0);
        centers.reshape(3);

        List<Mat> clusters = new ArrayList<Mat>();

        for (int i = 0; i < centers.rows(); i++) {
            clusters.add(Mat.zeros(cutout.size(), cutout.type()));
        }

        Map<Integer, Integer> counts = new HashMap<Integer, Integer>();

        for (int i = 0; i < centers.rows(); i++) {
            counts.put(i, 0);
        }

        int rows = 0;

        for (int y = 0; y < cutout.rows(); y++) {
            for (int x = 0; x < cutout.cols(); x++) {
                int label = (int)labels.get(rows, 0)[0];
                int r = (int)centers.get(label, 2)[0];
                int g = (int)centers.get(label, 1)[0];
                int b = (int)centers.get(label, 0)[0];
                counts.put(label, counts.get(label) + 1);
                clusters.get(label).put(y, x, b, g, r);
                rows++;
                //Core.circle( img, new Point(y,x),1,new Scalar(0,0,0));

            }
        }

        System.out.println(counts);
        Highgui.imwrite("C://opencv3.1//kmeans-test.jpg", img);
        return clusters;
    }
}
package ch13;

import java.util.ArrayList;
import java.util.List;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.utils.Converters;

public class Ch13_12_4KmeanClassificationForImage
{
    private final static int MAX_ITER = 10;
    //顯示由16到2,具減色效果
    private final static int CLUSTERS = 16;

    public static Mat imageKMeans(Mat img, int K, int maxIterations)
    {

        Mat m = img.reshape(1, img.rows() * img.cols());
        m.convertTo(m, CvType.CV_32F);

        Mat bestLabels = new Mat(m.rows(), 1, CvType.CV_8U);
        Mat centroids = new Mat(K, 1, CvType.CV_32F);
        Core.kmeans(m, K, bestLabels,
                    new TermCriteria(TermCriteria.COUNT | TermCriteria.EPS, maxIterations, 1E-5),
                    1, Core.KMEANS_RANDOM_CENTERS, centroids);
        List<Integer> idx = new ArrayList<Integer>(m.rows());
        Converters.Mat_to_vector_int(bestLabels, idx);

        Mat imgMapped = new Mat(m.size(), m.type());

        for (int i = 0; i < idx.size(); i++) {
            Mat row = imgMapped.row(i);
            centroids.row(idx.get(i)).copyTo(row);
        }

        return imgMapped.reshape(3, img.rows());
    }

    public static void main(String[] args)
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        Mat img = Imgcodecs.imread("C://opencv3.1//samples//DSC_0027.jpg",
                                   Imgcodecs.CV_LOAD_IMAGE_COLOR);
        Mat result = imageKMeans(img, CLUSTERS, MAX_ITER);


        Imgcodecs.imwrite("C://opencv3.1//samples//kmean-reduceColor.jpg", result);

    }
}

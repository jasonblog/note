package ch09;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.opencv.video.Video;

public class Ch09_14_2FindTransformECC
{

    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }

    public static void main(String[] args)
    {

        Mat templste = Imgcodecs.imread("C://opencv3.1//samples//DSC_0675.jpg",
                                        Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        Mat needToBeAdjusted = Imgcodecs.imread("C://opencv3.1//samples//DSC_0663.jpg",
                                                Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);


        //定義運動模型的warp matrix
        Mat warp_matrix2 = Mat.eye(2, 3, CvType.CV_32F);

        int maxIterations = 5000;
        Mat mask = new Mat();

        //執行ECC算法。結果被存儲在warp matrix
        Video.findTransformECC(templste, needToBeAdjusted, warp_matrix2,
                               Video.MOTION_EUCLIDEAN, new TermCriteria(TermCriteria.COUNT | TermCriteria.EPS,
                                       maxIterations, 1E-10), mask);
        Mat aligned = new Mat();

        //for  Euclidean and Affine
        Imgproc.warpAffine(needToBeAdjusted, aligned, warp_matrix2, templste.size(),
                           Imgproc.INTER_LINEAR + Imgproc.WARP_INVERSE_MAP);
        Imgcodecs.imwrite("C://opencv3.1//samples//findTransformECC2.jpg", aligned);
    }
}

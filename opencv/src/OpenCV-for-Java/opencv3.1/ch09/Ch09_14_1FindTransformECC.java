package ch09;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.opencv.video.Video;

public class Ch09_14_1FindTransformECC
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        Mat template = Imgcodecs.imread("C://opencv3.1//samples//DSC_0682.jpg",
                                        Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        Mat needToBeAdjusted = Imgcodecs.imread("C://opencv3.1//samples//DSC_0683.jpg",
                                                Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);

        System.out.println("motionType == MOTION_AFFINE=" + Video.MOTION_AFFINE);
        System.out.println("motionType == MOTION_HOMOGRAPHY=" +
                           Video.MOTION_HOMOGRAPHY);
        System.out.println("motionType == MOTION_EUCLIDEAN=" + Video.MOTION_EUCLIDEAN);
        //定義運動模型的warp matrix
        Mat warp_matrix1 = Mat.eye(3, 3, CvType.CV_32F);

        int maxIterations = 5000;
        Mat mask = new Mat();
        //執行ECC算法。結果被存儲在warp matrix
        Video.findTransformECC(template, needToBeAdjusted, warp_matrix1,
                               Video.MOTION_HOMOGRAPHY, new TermCriteria(TermCriteria.COUNT | TermCriteria.EPS,
                                       maxIterations, 1E-10), mask);
        Mat aligned = new Mat();

        //使用透視扭曲時，轉型是MOTION_HOMOGRAPHY
        Imgproc.warpPerspective(needToBeAdjusted, aligned, warp_matrix1,
                                template.size(), Imgproc.INTER_LINEAR + Imgproc.WARP_INVERSE_MAP);

        Imgcodecs.imwrite("C://opencv3.1//samples//findTransformECC1.jpg", aligned);
    }
}

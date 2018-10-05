package ch09;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.photo.Photo;

public class Ch09_8_2SeamlessCloneTestBg
{
    public static void main(String[] args)
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);

        Mat src3 = Imgcodecs.imread("C://opencv3.1//samples//IloveOpencv-gbg.jpg",
                                    Imgcodecs.CV_LOAD_IMAGE_COLOR);
        Mat dst3 = Imgcodecs.imread("C://opencv3.1//samples//wood2.jpg",
                                    Imgcodecs.CV_LOAD_IMAGE_COLOR);

        // Create an all white mask
        Mat src_mask3 = new Mat(src3.rows(), src3.cols(), src3.depth());

        for (int i = 0; i < src_mask3.rows(); i++) {
            for (int j = 0; j < src_mask3.cols(); j++) {
                double[] data = new double[3];
                data = src_mask3.get(i, j);
                data[0] = 256;

                src_mask3.put(i, j, data);
            }

        }





        // The location of the center of the src in the dst
        Point center3 = new Point(dst3.cols() / 2, dst3.rows() / 2);

        // Seamlessly clone src into dst and put the results in output

        Mat mixed_clone3 = new Mat();



        Photo.seamlessClone(src3, dst3, src_mask3, center3, mixed_clone3,
                            Photo.MIXED_CLONE);

        System.out.println("Photo.NORMAL_CLONE=" + Photo.NORMAL_CLONE);
        System.out.println("Photo.MIXED_CLONE=" + Photo.MIXED_CLONE);

        // Save results

        Imgcodecs.imwrite("C://opencv3.1//samples//SeamlessClone-MixedClone-iloveCv-testBG.jpg",
                          mixed_clone3);


    }

}

package ch09;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.photo.Photo;

public class Ch09_8_1SeamlessClone
{
    public static void main(String[] args)
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        Mat src1 = Imgcodecs.imread("C://opencv3.1//samples//monster.jpg",
                                    Imgcodecs.CV_LOAD_IMAGE_COLOR);
        Mat dst1 = Imgcodecs.imread("C://opencv3.1//samples//DSCN3629s.jpg",
                                    Imgcodecs.CV_LOAD_IMAGE_COLOR);

        Mat src2 = Imgcodecs.imread("C://opencv3.1//samples//monster-bg-less.jpg",
                                    Imgcodecs.CV_LOAD_IMAGE_COLOR);

        Mat src3 = Imgcodecs.imread("C://opencv3.1//samples//IloveOpencv-s.jpg",
                                    Imgcodecs.CV_LOAD_IMAGE_COLOR);
        Mat dst3 = Imgcodecs.imread("C://opencv3.1//samples//wood2.jpg",
                                    Imgcodecs.CV_LOAD_IMAGE_COLOR);

        Mat src4 = Imgcodecs.imread("C://opencv3.1//samples//IloveOpencv-bg-gray.jpg",
                                    Imgcodecs.CV_LOAD_IMAGE_COLOR);

        // Create an all white mask
        Mat src_mask = new Mat(src1.size(), src1.depth(), new Scalar(255));

        //      for (int i=0;i<src_mask.rows();i++){
        //            for (int j=0;j<src_mask.cols();j++){
        //                double[] data=new double[3];
        //                data=src_mask.get(i, j);
        //                data[0]= 255;
        //
        //              src_mask.put(i, j, data);
        //            }
        //
        //        }

        Mat src_mask3 = new Mat(src3.size(), src3.depth(), new Scalar(255));
        //      for (int i=0;i<src_mask3.rows();i++){
        //            for (int j=0;j<src_mask3.cols();j++){
        //                double[] data=new double[3];
        //                data=src_mask3.get(i, j);
        //                data[0]= 255;
        //
        //              src_mask3.put(i, j, data);
        //            }
        //
        //          }





        // The location of the center of the src in the dst
        Point center = new Point(dst1.cols() / 2, dst1.rows() / 2);
        Point center3 = new Point(dst3.cols() / 2, dst3.rows() / 2);

        // Seamlessly clone src into dst and put the results in output
        Mat normal_clone1 = new Mat();
        Mat mixed_clone1 = new Mat();

        Mat normal_clone2 = new Mat();
        Mat mixed_clone2 = new Mat();

        Mat normal_clone3 = new Mat();
        Mat mixed_clone3 = new Mat();


        Mat normal_clone4 = new Mat();
        Mat mixed_clone4 = new Mat();

        Photo.seamlessClone(src1, dst1, src_mask, center, normal_clone1,
                            Photo.NORMAL_CLONE);
        Photo.seamlessClone(src1, dst1, src_mask, center, mixed_clone1,
                            Photo.MIXED_CLONE);

        Photo.seamlessClone(src2, dst1, src_mask, center, normal_clone2,
                            Photo.NORMAL_CLONE);
        Photo.seamlessClone(src2, dst1, src_mask, center, mixed_clone2,
                            Photo.MIXED_CLONE);

        Photo.seamlessClone(src3, dst3, src_mask3, center3, normal_clone3,
                            Photo.NORMAL_CLONE);
        Photo.seamlessClone(src3, dst3, src_mask3, center3, mixed_clone3,
                            Photo.MIXED_CLONE);

        Photo.seamlessClone(src4, dst3, src_mask3, center3, normal_clone4,
                            Photo.NORMAL_CLONE);
        Photo.seamlessClone(src4, dst3, src_mask3, center3, mixed_clone4,
                            Photo.MIXED_CLONE);

        System.out.println("Photo.NORMAL_CLONE=" + Photo.NORMAL_CLONE);
        System.out.println("Photo.MIXED_CLONE=" + Photo.MIXED_CLONE);

        // Save results
        Imgcodecs.imwrite("C://opencv3.1//samples//SeamlessClone-normalClone-LakeMonster.jpg",
                          normal_clone1);
        Imgcodecs.imwrite("C://opencv3.1//samples//SeamlessClone-MixedClone-LakeMonster.jpg",
                          mixed_clone1);

        Imgcodecs.imwrite("C://opencv3.1//samples//SeamlessClone-normalClone-LakeMonster-bgLess.jpg",
                          normal_clone2);
        Imgcodecs.imwrite("C://opencv3.1//samples//SeamlessClone-MixedClone-LakeMonster-bgLess.jpg",
                          mixed_clone2);

        Imgcodecs.imwrite("C://opencv3.1//samples//SeamlessClone-normalClone-iloveCv.jpg",
                          normal_clone3);
        Imgcodecs.imwrite("C://opencv3.1//samples//SeamlessClone-MixedClone-iloveCv.jpg",
                          mixed_clone3);

        Imgcodecs.imwrite("C://opencv3.1//samples//SeamlessClone-normalClone-iloveCv2.jpg",
                          normal_clone4);
        Imgcodecs.imwrite("C://opencv3.1//samples//SeamlessClone-MixedClone-iloveCv2.jpg",
                          mixed_clone4);
    }

}

package ch10;

import java.util.ArrayList;
import java.util.List;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;

public class Ch10_19_1MatchShapes
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }
    public static void main(String[] args)
    {
        try {
            Mat source = Highgui.imread("C://opencv3.1//samples//IloveOpencv-s.jpg",
                                        Highgui.CV_LOAD_IMAGE_GRAYSCALE);
            Mat target = Highgui.imread("C://opencv3.1//samples//IloveOpencv.jpg",
                                        Highgui.CV_LOAD_IMAGE_GRAYSCALE);

            Mat target2 =
                Highgui.imread("C://opencv3.1//samples//SeamlessClone-normalClone-iloveCv.jpg",
                               Highgui.CV_LOAD_IMAGE_GRAYSCALE);

            Mat target3 = Highgui.imread("C://opencv3.1//samples//lena.jpg",
                                         Highgui.CV_LOAD_IMAGE_GRAYSCALE);


            List<MatOfPoint> src_contours = new ArrayList<MatOfPoint>();
            List<MatOfPoint> trg_contours = new ArrayList<MatOfPoint>();
            List<MatOfPoint> trg2_contours = new ArrayList<MatOfPoint>();
            List<MatOfPoint> trg3_contours = new ArrayList<MatOfPoint>();


            Mat src_hierarchy = new Mat(source.rows(), source.cols(), CvType.CV_8UC1,
                                        new Scalar(0));
            Mat trg_hierarchy = new Mat(target.rows(), target.cols(), CvType.CV_8UC1,
                                        new Scalar(0));
            Mat trg2_hierarchy = new Mat(target2.rows(), target2.cols(), CvType.CV_8UC1,
                                         new Scalar(0));
            Mat trg3_hierarchy = new Mat(target3.rows(), target3.cols(), CvType.CV_8UC1,
                                         new Scalar(0));


            Imgproc.findContours(source, src_contours, src_hierarchy, Imgproc.RETR_TREE,
                                 Imgproc.CHAIN_APPROX_SIMPLE);
            Imgproc.findContours(target, trg_contours, trg_hierarchy, Imgproc.RETR_TREE,
                                 Imgproc.CHAIN_APPROX_SIMPLE);
            Imgproc.findContours(target2, trg2_contours, trg2_hierarchy, Imgproc.RETR_TREE,
                                 Imgproc.CHAIN_APPROX_SIMPLE);
            Imgproc.findContours(target3, trg3_contours, trg3_hierarchy, Imgproc.RETR_TREE,
                                 Imgproc.CHAIN_APPROX_SIMPLE);

            double result0 = Imgproc.matchShapes(src_contours.get(0), src_contours.get(0),
                                                 1, 0);
            double result1 = Imgproc.matchShapes(src_contours.get(0), trg_contours.get(0),
                                                 1, 0);
            double result2 = Imgproc.matchShapes(src_contours.get(0), trg2_contours.get(0),
                                                 1, 0);
            double result3 = Imgproc.matchShapes(src_contours.get(0), trg3_contours.get(0),
                                                 1, 0);

            System.out.println("result0= " + result0);
            System.out.println("result1= " + result1);
            System.out.println("result2= " + result2);
            System.out.println("result3= " + result3);

            System.out.println("CV_CONTOURS_MATCH_I1= " + Imgproc.CV_CONTOURS_MATCH_I1);
            System.out.println("CV_CONTOURS_MATCH_I2= " + Imgproc.CV_CONTOURS_MATCH_I2);
            System.out.println("CV_CONTOURS_MATCH_I3= " + Imgproc.CV_CONTOURS_MATCH_I3);
        } catch (Exception e) {
            System.out.println("error: " + e.getMessage());
        }
    }
}
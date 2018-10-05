package ch10;

import java.util.ArrayList;
import java.util.List;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfInt;
import org.opencv.core.MatOfInt4;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

public class Ch10_10_1ConvexityDefects
{
    public static void main(String[] args)
    {
        try {
            System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
            Mat source = Imgcodecs.imread("C://opencv3.1//samples//palm_p.jpg",
                                          Imgcodecs.CV_LOAD_IMAGE_COLOR);
            Mat target = new Mat(source.size(), CvType.CV_8U);
            Imgproc.cvtColor(source, target, Imgproc.COLOR_RGB2GRAY);
            Imgproc.GaussianBlur(target, target, new Size(21, 21), 0, 0);

            Mat threshold_output = new Mat(source.rows(), source.cols(),
                                           source.type());
            Imgproc.threshold(target, threshold_output, 40, 255,
                              Imgproc.THRESH_BINARY);// 背景太白,3rd用100
            Imgcodecs.imwrite("C://opencv3.1//samples//ConvexHull3.jpg",
                              threshold_output);
            Mat hierarchy = new Mat(target.rows(), target.cols(),
                                    CvType.CV_8UC1, new Scalar(0));
            List<MatOfPoint> contours = new ArrayList<MatOfPoint>();

            Imgproc.findContours(threshold_output, contours, hierarchy,
                                 Imgproc.RETR_TREE, Imgproc.CHAIN_APPROX_SIMPLE);

            int[] mConvexityDefectsIntArrayList;
            int count = 0;

            for (int i = 0; i < contours.size(); i++) {
                MatOfInt4 mConvexityDefectsMatOfInt4 = new MatOfInt4();
                MatOfInt hull = new MatOfInt();
                MatOfPoint tempContour = contours.get(i);
                Imgproc.convexHull(tempContour, hull, false);
                // System.out.println("hull1:"+hull.size());

                int index = (int) hull.get(((int) hull.size().height) - 1, 0)[0];
                Point pt, pt0 = new Point(tempContour.get(index, 0)[0],
                                          tempContour.get(index, 0)[1]);

                for (int j = 0; j < hull.size().height - 1; j++) {
                    // for(int j = 0; j < 3 ; j++){
                    index = (int) hull.get(j, 0)[0];
                    pt = new Point(tempContour.get(index, 0)[0],
                                   tempContour.get(index, 0)[1]);
                    Imgproc.line(source, pt0, pt, new Scalar(255, 0, 100),
                                 2);// 2是厚度,藍,凸包的輪廓
                    // double distance=Math.sqrt(Math.pow(pt0.x-pt.x,
                    // 2)+Math.pow(pt0.y-pt.y, 2));
                    // System.out.println("distance-hull-"+j+":" + distance);
                    pt0 = pt;
                }

                if (contours.size() > 0 && hull.rows() > 0) {
                    Imgproc.convexityDefects(tempContour, hull,
                                             mConvexityDefectsMatOfInt4);
                }

                System.out.println(mConvexityDefectsMatOfInt4.height());
                int index2 = (int) mConvexityDefectsMatOfInt4.toArray().length;
                int index3;
                Point pt3, pt1, old_pt1 = null, pt4 = null, pt2 = new Point(
                    tempContour.get(index2, 0)[0], tempContour.get(index2,
                            0)[1]);

                for (int j = 0; j < mConvexityDefectsMatOfInt4.size().height - 1; j++) {
                    // for(int j = 0; j < 3 ; j++){
                    index2 = (int) mConvexityDefectsMatOfInt4.get(j,
                             0)[0];// 0是top即指尖,2是down,兩指之底
                    index3 = (int) mConvexityDefectsMatOfInt4.get(j, 0)[2];
                    pt1 = new Point(tempContour.get(index2, 0)[0],
                                    tempContour.get(index2, 0)[1]);// 尖
                    pt3 = new Point(tempContour.get(index3, 0)[0],
                                    tempContour.get(index3, 0)[1]);// 底
                    Imgproc.line(source, pt1, pt1, new Scalar(0, 255, 0), 9);// 尖dot,綠
                    Imgproc.line(source, pt3, pt3, new Scalar(150, 155, 255), 12);// 底dot,粉紅
                    Imgproc.line(source, pt1, pt3, new Scalar(0, 0, 255), 2);// 紅,姆指左

                    if (pt4 != null && old_pt1 != null) {
                        Imgproc.line(source, pt1, pt4, new Scalar(200, 0, 200), 2);// 桃紅,姆指右
                        Imgproc.putText(source, getAngle(pt1, pt4, old_pt1) + "",
                                        pt4, 0, 0.3, new Scalar(0, 0, 0));// 顯示角度

                        if (getAngle(pt1, pt4, old_pt1) > 19
                            && getAngle(pt1, pt4, old_pt1) < 87) {

                            count++;
                        }

                    }

                    pt4 = pt3;
                    old_pt1 = pt1;
                }

                System.out.println("count-" + count);
            }

            Imgcodecs.imwrite("C://opencv3.1//samples//ConvexHull2.jpg", source);
        } catch (Exception e) {
            System.out.println("error: " + e.getMessage());
        }
    }

    public static double getAngle(Point a, Point b, Point c)
    {
        double line1 = Math.sqrt(Math.pow(a.x - b.x, 2)
                                 + Math.pow(a.y - b.y, 2));
        double line2 = Math.sqrt(Math.pow(c.x - b.x, 2)
                                 + Math.pow(c.y - b.y, 2));
        double dot = (a.x - b.x) * (c.x - b.x) + (a.y - b.y) * (c.y - b.y);
        double angle = Math.acos(dot / (line1 * line2));
        angle = angle * 180 / Math.PI;
        return Math.round(100 * angle) / 100;

    }
}
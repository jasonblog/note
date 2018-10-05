package ch13;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.opencv.objdetect.CascadeClassifier;
import org.opencv.videoio.VideoCapture;

public class Ch13_1_5EyeDetectorAddGlassesByWebCam
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }
    public static void main(String arg[])
    {
        VideoCapture capture = new VideoCapture();
        capture.open(0);
        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("人臉檢測");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);

        Mat eyeglasses = Imgcodecs.imread("C://opencv3.1//samples//eyeglasses.jpg",
                                          Imgcodecs.CV_LOAD_IMAGE_COLOR);
        //Mat eyeglasses = Imgcodecs.imread("C://opencv3.1//samples//eyeglasses-tran.jpg",Imgcodecs.CV_LOAD_IMAGE_COLOR);

        if (!capture.isOpened()) {
            System.out.println("Error");
        } else {
            Mat webcam_image = new Mat();
            capture.read(webcam_image);
            frame1.setSize(webcam_image.width() + 10, webcam_image.height() + 20);

            CascadeClassifier eyeDetector = new
            CascadeClassifier("C://opencv3.1//sources//data//haarcascades//haarcascade_mcs_eyepair_big.xml");
            MatOfRect eyeDetections = new MatOfRect();
            Mat chgGlassesSize = new Mat();

            while (true) {
                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    eyeDetector.detectMultiScale(webcam_image, eyeDetections);

                    for (Rect rect : eyeDetections.toArray()) {
                        Point center = new Point(rect.x, rect.y - rect.height * 0.3);
                        //Imgproc.rectangle(webcam_image, center, new Point(rect.x + rect.width*1.1, rect.y + rect.height*1.1 ),  new Scalar( 255, 255, 75 ),3 );

                        try {


                            int oWidth = (int)(rect.width * 1.4);
                            int oHeight = (int)(rect.height * 1.4);
                            //  System.out.println("oWidth="+oWidth+",oHeight="+oHeight);
                            //改變眼鏡大小
                            chgGlassesSize = resize(eyeglasses, oHeight, oWidth);
                            Mat fit = merge(chgGlassesSize);

                            //把小圖copy到大圖
                            Rect roi = new Rect((int)(rect.x - rect.width * 0.3),
                                                (int)(rect.y - rect.height * 0.3), oWidth, oHeight);
                            Mat destinationROI = webcam_image.submat(roi);
                            fit.copyTo(destinationROI, fit);
                        } catch (Exception e) {
                            // TODO: handle exception
                        }
                    }


                    panel1.setimagewithMat(webcam_image);
                    frame1.repaint();



                } else {
                    capture.release();
                    break;
                }
            }
        }
    }
    public static Mat resize(Mat source, int sizeX, int sizeY)
    {

        Mat destination = new Mat(sizeX, sizeY, source.type());
        Imgproc.resize(source, destination, destination.size(), sizeX, sizeY, 0);
        return destination;

    }

    public static Mat merge(Mat source)
    {

        Mat mask2 = new Mat();
        Mat dst = new Mat();

        Imgproc.cvtColor(source, mask2, Imgproc.COLOR_BGR2GRAY);
        Imgproc.threshold(mask2, mask2, 230, 255, Imgproc.THRESH_BINARY_INV);
        List<Mat> planes = new ArrayList<Mat>() ;
        List<Mat> result = new ArrayList<Mat>() ;
        Mat result1 = new Mat();
        Mat result2 = new Mat();
        Mat result3 = new Mat();

        Core.split(source, planes);

        Core.bitwise_and(planes.get(0), mask2, result1);
        Core.bitwise_and(planes.get(1), mask2, result2);
        Core.bitwise_and(planes.get(2), mask2, result3);

        result.add(result1);
        result.add(result2);
        result.add(result3);
        Core.merge(result, dst);
        //以上白色變透明
        return dst;

    }


}

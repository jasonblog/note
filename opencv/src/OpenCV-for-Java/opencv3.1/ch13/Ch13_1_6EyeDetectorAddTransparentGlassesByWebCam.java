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
import org.opencv.photo.Photo;
import org.opencv.videoio.VideoCapture;

public class Ch13_1_6EyeDetectorAddTransparentGlassesByWebCam
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
        //Mat eyeglasses = Imgcodecs.imread("C://opencv3//samples//eyeglasses-tran.jpg",Imgcodecs.CV_LOAD_IMAGE_COLOR);

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
                        Point center = new Point(rect.x + rect.width * 0.5, rect.y + rect.height * 0.5);
                        //Imgproc.rectangle(webcam_image, center, new Point(rect.x + rect.width*1.1, rect.y + rect.height*1.1 ),  new Scalar( 255, 255, 75 ),3 );

                        try {
                            int oWidth = (int)(rect.width * 1.4);
                            int oHeight = (int)(rect.height * 1.4);
                            //  System.out.println("oWidth="+oWidth+",oHeight="+oHeight);
                            //改變眼鏡大小
                            chgGlassesSize = resize(eyeglasses, oHeight, oWidth);

                            webcam_image = merge(webcam_image, chgGlassesSize, center);

                        } catch (Exception e) {
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

    public static Mat merge(Mat BigBg, Mat SmallFg, Point center)
    {


        Mat src_mask = new Mat(SmallFg.rows(), SmallFg.cols(), SmallFg.depth());

        for (int i = 0; i < src_mask.rows(); i++) {
            for (int j = 0; j < src_mask.cols(); j++) {
                double[] data = new double[3];
                data = src_mask.get(i, j);
                data[0] = 255; //白色變透明

                src_mask.put(i, j, data);
            }

        }

        Mat mixed_clone = new Mat();
        //把小圖copy到大圖
        Photo.seamlessClone(SmallFg, BigBg, src_mask, center, mixed_clone,
                            Photo.MIXED_CLONE);

        return mixed_clone;

    }


}

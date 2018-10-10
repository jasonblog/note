package ch10;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.highgui.VideoCapture;
import org.opencv.imgproc.Imgproc;
import org.opencv.core.CvType;
public class Ch10_16_2FindPalmByHSVDilate
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String arg[])
    {
        JFrame frame1 = new JFrame("WebCamera");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);
        JFrame frame2 = new JFrame("HSV色彩空間");
        frame2.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame2.setSize(640, 480);
        frame2.setBounds(300, 100, frame2.getWidth() + 300, 100 + frame2.getHeight());
        Panel panel2 = new Panel();
        frame2.setContentPane(panel2);
        frame2.setVisible(true);
        JFrame frame3 = new JFrame("Threshold二值化");
        frame3.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame3.setSize(640, 480);
        frame3.setBounds(600, 200, frame2.getWidth() + 900, 300 + frame2.getHeight());
        Panel panel3 = new Panel();
        frame3.setContentPane(panel3);
        frame3.setVisible(true);
        //讀入webcam影像
        VideoCapture capture = new VideoCapture(0);
        Mat webcam_image = new Mat();
        Mat hsv_image = new Mat();
        Mat thresholded = new Mat();
        capture.read(webcam_image);
        frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);
        frame2.setSize(webcam_image.width() + 40, webcam_image.height() + 60);
        frame3.setSize(webcam_image.width() + 40, webcam_image.height() + 60);
        Mat circles = new Mat();

        //抓到膚色ok
        Scalar hsv_min = new Scalar(0, 10, 60);
        Scalar hsv_max = new Scalar(20, 150, 255);

        if (capture.isOpened()) {
            while (true) {
                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    // 轉換
                    Imgproc.cvtColor(webcam_image, hsv_image, Imgproc.COLOR_BGR2HSV);
                    Core.inRange(hsv_image, hsv_min, hsv_max, thresholded);

                    // 除噪
                    Imgproc.GaussianBlur(thresholded, thresholded, new Size(15, 15), 0, 0);
                    Mat threshold_output = new Mat(webcam_image.rows(), webcam_image.cols(),
                                                   CvType.CV_8UC1);
                    //threshold
                    Imgproc.threshold(thresholded, threshold_output, 55, 255,
                                      Imgproc.THRESH_BINARY);
                    //Dilate(膨脹)處理
                    Mat element = Imgproc.getStructuringElement(Imgproc.MORPH_ELLIPSE, new Size(9,
                                  9));
                    Imgproc.dilate(threshold_output, threshold_output, element);
                    //繪出&顯示
                    panel1.setimagewithMat(webcam_image);
                    panel2.setimagewithMat(hsv_image);
                    panel3.setimagewithMat(threshold_output);
                    frame1.repaint();
                    frame2.repaint();
                    frame3.repaint();
                } else {
                    System.out.println(" Webcam沒有影像-異常!");
                    break;
                }
            }
        }

        return;
    }
}

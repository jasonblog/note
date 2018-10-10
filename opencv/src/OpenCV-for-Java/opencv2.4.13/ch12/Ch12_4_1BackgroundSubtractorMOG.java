package ch12;
import java.util.List;

import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.highgui.VideoCapture;

public class Ch12_4_1BackgroundSubtractorMOG
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }
    static org.opencv.video.BackgroundSubtractorMOG2 bg = new
    org.opencv.video.BackgroundSubtractorMOG2();
    public static void main(String arg[])
    {
        VideoCapture capture = new VideoCapture("C://opencv3.1//samples//768x576.avi");
        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("行人檢測");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);

        JFrame frame2 = new JFrame("Threshold");
        frame2.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame2.setSize(640, 480);
        frame2.setBounds(300, 100, frame1.getWidth() + 50,
                         50 + frame1.getHeight());
        Panel panel2 = new Panel();
        frame2.setContentPane(panel2);
        frame2.setVisible(true);

        if (!capture.isOpened()) {
            System.out.println("Error");
        } else {
            Mat webcam_image = new Mat();
            capture.read(webcam_image);
            frame1.setSize(webcam_image.width() + 10, webcam_image.height() + 10);
            frame2.setSize(webcam_image.width() + 40, webcam_image.height() + 60);

            while (true) {
                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    Mat source = webcam_image.clone();
                    webcam_image = detect(webcam_image);
                    panel1.setimagewithMat(webcam_image);
                    frame1.repaint();

                    panel2.setimagewithMat(source);
                    frame2.repaint();

                } else {
                    System.out.println("影片讀取結束!");
                    capture.release();
                    break;
                }
            }
        }
    }
    public static Mat detect(Mat inputframe)
    {

        Mat fgmask = new Mat();
        bg.apply(inputframe, fgmask, 0.00001);

        return fgmask;
    }


}

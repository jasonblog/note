package ch04;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.highgui.VideoCapture;

public class Ch04_3_1ReadAviWithPanel
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String arg[])
    {
        VideoCapture capture = new VideoCapture("C://opencv3.1//samples//tree.avi");
        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("從AVI檔讀取影像至Java Swing視窗");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);

        if (!capture.isOpened()) {
            System.out.println("Error");
        } else {
            Mat webcam_image = new Mat();
            capture.read(webcam_image);
            frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);

            while (true) {
                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    panel1.setimagewithMat(webcam_image);
                    frame1.repaint();
                } else {
                    System.out.println("影片讀取結束!");
                    capture.release();
                    break;
                }


            }
        }
    }
}

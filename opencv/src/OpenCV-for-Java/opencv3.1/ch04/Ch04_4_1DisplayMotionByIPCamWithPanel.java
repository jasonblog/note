package ch04;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.videoio.VideoCapture;

public class Ch04_4_1DisplayMotionByIPCamWithPanel
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String arg[])
    {
        VideoCapture capture = new VideoCapture();
        //without username & password
        //capture.open("http://192.168.1.4:8080/mjpg/video.mjpg");
        capture.open("http://username:password@192.168.1.4:8080/mjpg/video.mjpg");

        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("從IPCam讀取影像至Java Swing視窗");
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
                System.out.println("從IPCam讀取影像至Java Swing視窗");
                panel1.setimagewithMat(webcam_image);
                frame1.repaint();
            }
        }
    }
}

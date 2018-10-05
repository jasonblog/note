package ch13;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.highgui.VideoCapture;
import org.opencv.imgproc.Imgproc;
import org.opencv.objdetect.CascadeClassifier;

public class Ch13_1_7PedestriansDetector
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }
    public static void main(String arg[])
    {
        VideoCapture capture = new VideoCapture();
        capture.open("C://opencv3.1//samples//768x576.avi");
        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("¦æ¤HÀË´ú");
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
            frame1.setSize(webcam_image.width() + 10, webcam_image.height() + 20);

            CascadeClassifier pedestriansDetector = new
            CascadeClassifier("C://opencv3.1//sources//data//hogcascades//hogcascade_pedestrians.xml");
            MatOfRect pedestriansDetections = new MatOfRect();

            while (true) {
                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    pedestriansDetector.detectMultiScale(webcam_image, pedestriansDetections);

                    for (Rect rect : pedestriansDetections.toArray()) {
                        Core.rectangle(webcam_image, new Point(rect.x, rect.y),
                                       new Point(rect.x + rect.width, rect.y + rect.height),
                                       new Scalar(0, 255, 0), 3);
                    }

                    System.out.println(String.format("Detected %s pedestrians",
                                                     pedestriansDetections.toArray().length));
                    panel1.setimagewithMat(webcam_image);
                    frame1.repaint();



                } else {
                    capture.release();
                    break;
                }
            }
        }
    }


}

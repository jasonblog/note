package ch13;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;
import org.opencv.objdetect.CascadeClassifier;
import org.opencv.videoio.VideoCapture;

public class Ch13_1_11Rj45DetectorByWebCam
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }
    public static void main(String arg[])
    {
        VideoCapture capture = new VideoCapture();
        capture.open(0);
        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("RJ45ÀË´ú");
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

            CascadeClassifier rj45Detector = new
            CascadeClassifier("C://opencv3.1//samples//rj45cascade.xml");
            MatOfRect rj45Detections = new MatOfRect();



            while (true) {
                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    rj45Detector.detectMultiScale(webcam_image, rj45Detections);

                    for (Rect rect : rj45Detections.toArray()) {
                        Point center = new Point(rect.x + rect.width * 0.5, rect.y + rect.height * 0.5);
                        Imgproc.ellipse(webcam_image, center, new Size(rect.width * 0.5,
                                        rect.height * 0.5), 0, 0, 360, new Scalar(255, 0, 255), 4, 8, 0);
                    }

                    System.out.println(String.format("Detected %s rj45s",
                                                     rj45Detections.toArray().length));

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

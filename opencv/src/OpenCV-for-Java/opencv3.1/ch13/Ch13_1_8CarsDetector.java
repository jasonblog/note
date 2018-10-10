package ch13;
import java.util.List;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfDouble;
import org.opencv.core.MatOfFloat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;
import org.opencv.objdetect.CascadeClassifier;
import org.opencv.objdetect.HOGDescriptor;
import org.opencv.videoio.VideoCapture;

public class Ch13_1_8CarsDetector
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String arg[]) throws InterruptedException {
        VideoCapture vid = new VideoCapture();
        vid.open("C://opencv3.1//samples//video2.avi");
        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("車輛檢測");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(320, 240);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);
        int CountCar = 0;

        if (!vid.isOpened())
        {
            System.out.println("Error");
        } else
        {
            Mat webcam_image = new Mat();
            vid.read(webcam_image);
            frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);

            CascadeClassifier carsDetector = new
            CascadeClassifier("C://opencv3.1//samples//cars3.xml");
            MatOfRect carsDetections = new MatOfRect();

            while (true) {
                Thread.sleep(300);
                vid.read(webcam_image);

                if (!webcam_image.empty()) {


                    carsDetector.detectMultiScale(webcam_image, carsDetections);

                    for (Rect rect : carsDetections.toArray()) {
                        Imgproc.rectangle(webcam_image, new Point(rect.x, rect.y),
                                          new Point(rect.x + rect.width, rect.y + rect.height),
                                          new Scalar(0, 255, 0), 3);
                    }

                    CountCar = CountCar + carsDetections.toArray().length;
                    System.out.println("Sum Cars:" + CountCar);




                    panel1.setimagewithMat(webcam_image);
                    frame1.repaint();
                } else {
                    System.out.println("影片讀取結束!");
                    vid.release();
                    break;
                }
            }
        }
    }


}

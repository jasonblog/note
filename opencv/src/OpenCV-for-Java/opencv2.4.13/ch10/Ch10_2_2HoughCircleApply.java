package ch10;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.highgui.Highgui;
import org.opencv.highgui.VideoCapture;
import org.opencv.imgproc.Imgproc;
public class Ch10_2_2HoughCircleApply
{
    // Load the native library.
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String arg[]) throws Exception{

        JFrame frame1 = new JFrame("Camera");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);

        JFrame frame4 = new JFrame("Threshold");
        frame4.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame4.setSize(640, 480);
        frame4.setBounds(600, 100, frame1.getWidth(), frame1.getHeight()); //Jframe顯示位置
        Panel panel4 = new Panel();
        frame4.setContentPane(panel4);
        frame4.setVisible(true);

        //讀入webcam視訊
        VideoCapture capture = new VideoCapture(0);
        Mat webcam_image = new Mat();
        capture.read(webcam_image);
        frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);
        frame4.setSize(webcam_image.width() + 40, webcam_image.height() + 60);


        if (capture.isOpened())
        {
            while (true) {
                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    Mat distance = new Mat(webcam_image.height(), webcam_image.width(),
                                           Highgui.CV_LOAD_IMAGE_GRAYSCALE);
                    Imgproc.GaussianBlur(webcam_image, distance, new Size(9, 9), 20, 0);
                    Imgproc.Canny(distance, distance, 100, 120);
                    Mat circles = new Mat();
                    Imgproc.HoughCircles(distance, circles, Imgproc.CV_HOUGH_GRADIENT, 1.3,
                                         distance.height() / 2, 500.0, 50.0, 0, 0);

                    System.out.println("circle-size=" + circles.cols());
                    Imgproc.cvtColor(distance, distance, Imgproc.COLOR_GRAY2BGR);
                    Point p1 = new Point(0, 0);
                    Point p2 = new Point(0, 0);

                    //畫出所有圓
                    if (circles.cols() > 0) {

                        for (int i = 0; i < circles.cols(); i++) {
                            double vCircle[] = circles.get(0, i);

                            Point center = new Point(Math.round(vCircle[0]), Math.round(vCircle[1]));
                            int radius = (int)Math.round(vCircle[2]);

                            //draw the circle center
                            Core.circle(webcam_image, center, 3, new Scalar(255, 0, 255), -1, 8, 0);
                            Core.putText(webcam_image, "(" + center.x + "," + center.y + ")",
                                         new Point(center.x, center.y), 1, 1.0, new Scalar(100, 10, 10, 255), 1);

                            //draw the circle outline
                            Core.circle(webcam_image, center, radius, new Scalar(255, 0, 255), 2, 8, 0);

                            if (i == 0) {
                                p1 = center;
                            } else if (i == 1) {
                                p2 = center;
                            }
                        }

                        if (p1.x != 0.0 && p1.y != 0.0 && p2.x != 0.0 && p2.y != 0.0) {
                            Core.line(webcam_image, p1, p2, new Scalar(100, 10, 10), 3);
                            Core.putText(webcam_image, "L=" + getDistance(p1, p2),
                                         new Point((p1.x + p2.x) / 2 + 20, (p1.y + p2.y) / 2 + 20), 2, 1.2,
                                         new Scalar(255, 255, 0), 2);
                        }

                    }

                    //Display the image
                    panel1.setimagewithMat(webcam_image);
                    panel4.setimagewithMat(distance);
                    frame1.repaint();
                    frame4.repaint();
                } else {
                    System.out.println(" --(!) No captured frame -- Break!");
                    break;
                }
            }
        }

        return;
    }
    public static double getDistance(Point a, Point b)
    {
        return Math.round(Math.sqrt(Math.pow(a.x - b.x, 2) + Math.pow(a.y - b.y,
                                    2)) * 100 / 100);
    }
}

package ch12;
import java.util.ArrayList;
import java.util.List;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.highgui.VideoCapture;
import org.opencv.imgproc.Imgproc;
import org.opencv.core.CvType;

public class Ch12_1_1GetMotionByAbsdiff
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String arg[])
    {
        JFrame frame1 = new JFrame("Camera");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);

        JFrame frame2 = new JFrame("Threshold");
        frame2.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame2.setSize(640, 480);
        frame2.setBounds(300, 100, frame1.getWidth() + 50, 50 + frame1.getHeight());
        Panel panel2 = new Panel();
        frame2.setContentPane(panel2);
        frame2.setVisible(true);
        //-- 2. Read the video stream
        VideoCapture capture = new VideoCapture(0);
        Mat webcam_image = new Mat();
        capture.read(webcam_image);
        frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);
        frame2.setSize(webcam_image.width() + 40, webcam_image.height() + 60);

        Mat image = null;
        Mat prevImage = null;
        Mat diff = null;
        Mat mHierarchy = new Mat();

        if (capture.isOpened()) {
            while (true) {
                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    Imgproc.GaussianBlur(webcam_image, webcam_image, new Size(9, 9), 0, 0);

                    if (image == null) {
                        image = new Mat(webcam_image.size(), CvType.CV_8U);
                        Imgproc.cvtColor(webcam_image, image, Imgproc.COLOR_RGB2GRAY);
                    } else {
                        prevImage = new Mat(webcam_image.size(), CvType.CV_8U);

                        prevImage = image;

                        image = new Mat(webcam_image.size(), CvType.CV_8U);
                        Imgproc.cvtColor(webcam_image, image, Imgproc.COLOR_RGB2GRAY);

                    }

                    if (diff == null) {
                        diff = new Mat(webcam_image.size(), CvType.CV_8U);
                    }

                    if (prevImage != null) {

                        Core.absdiff(image, prevImage, diff);

                        //64:偵測小框框較多,但靈敏,127:偵測小框框較少,但不靈敏
                        Imgproc.threshold(diff, diff, 64, 255, Imgproc.THRESH_BINARY);
                        List<MatOfPoint> contours = new ArrayList<MatOfPoint>();

                        Imgproc.findContours(diff, contours, mHierarchy, Imgproc.RETR_LIST,
                                             Imgproc.CHAIN_APPROX_SIMPLE);

                        /**會有許多小框框
                         * **/
                        for (int i = 0; i < contours.size(); i++) {
                            Rect r = Imgproc.boundingRect(contours.get(i));
                            int ContourArea = r.height * r.width;

                            if (ContourArea > 500) {
                                Imgproc.drawContours(webcam_image, contours, i, new Scalar(255, 0, 0, 255), -1);
                                //補捉到使用藍色表示
                                Core.rectangle(webcam_image, new Point(r.x, r.y), new Point(r.x + r.width,
                                               r.y + r.height), new Scalar(0, 255, 0));
                            }

                        }

                    }

                    panel1.setimagewithMat(webcam_image);
                    panel2.setimagewithMat(diff);  //
                    frame1.repaint();
                    frame2.repaint();
                } else {
                    System.out.println(" 無補抓任何畫面!");
                    break;
                }
            }
        }

        return;
    }
}

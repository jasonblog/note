package ch10;

import java.util.ArrayList;
import java.util.List;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Point;
import org.opencv.core.Range;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.highgui.VideoCapture;
import org.opencv.imgproc.Imgproc;
import org.opencv.core.CvType;

public class Ch10_18_2FindPalmCenterByWebCam
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }
    static ArrayList<Double> HullList = new ArrayList<Double>();

    public static void main(String arg[]) throws Exception {
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
        frame2.setBounds(300, 100, frame1.getWidth() + 50,
                         50 + frame1.getHeight());
        Panel panel4 = new Panel();
        frame2.setContentPane(panel4);
        frame2.setVisible(true);
        // -- 2. Read the video stream
        VideoCapture capture = new VideoCapture(0);
        Mat webcam_image = new Mat();
        Mat hsv_image = new Mat();
        Mat thresholded = new Mat();
        capture.read(webcam_image);
        frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);
        frame2.setSize(webcam_image.width() + 40, webcam_image.height() + 60);
        Mat array255 = new Mat(webcam_image.height(), webcam_image.width(),
                               CvType.CV_8UC1);
        array255.setTo(new Scalar(255));
        Scalar hsv_min = new Scalar(0, 10, 60);
        Scalar hsv_max = new Scalar(20, 150, 255);

        Mat firstArea = webcam_image.colRange(new Range(10, 320)).rowRange(70,
                430);

        if (capture.isOpened())
        {
            while (true) {
                Thread.sleep(500);
                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    Imgproc.cvtColor(firstArea, hsv_image,
                                     Imgproc.COLOR_BGR2HSV);
                    Core.inRange(hsv_image, hsv_min, hsv_max, thresholded);
                    Imgproc.GaussianBlur(thresholded, thresholded, new Size(15,
                                         15), 0, 0);
                    Mat threshold_output = new Mat(webcam_image.rows(),
                                                   webcam_image.cols(), CvType.CV_8UC1);
                    Imgproc.threshold(thresholded, threshold_output, 50, 255,
                                      Imgproc.THRESH_BINARY);

                    List<MatOfPoint> contours = new ArrayList<MatOfPoint>();
                    Mat hierarchy = new Mat(webcam_image.rows(),
                                            webcam_image.cols(), CvType.CV_8UC1, new Scalar(0));

                    Mat destination1 = new Mat();

                    // 找輪廓
                    Imgproc.findContours(threshold_output, contours, hierarchy,
                                         Imgproc.RETR_TREE, Imgproc.CHAIN_APPROX_SIMPLE);

                    int s = findBiggestContour(contours);
                    Mat drawing = new Mat(threshold_output.size(),
                                          CvType.CV_8UC1);
                    Imgproc.drawContours(drawing, contours, s, new Scalar(255,
                                         0, 0, 255), -1);

                    Imgproc.distanceTransform(drawing, destination1,
                                              Imgproc.CV_DIST_L2, 3);
                    int temp = 0;
                    // 半徑
                    int R = 0;

                    // 圓心
                    int centerX = 0;
                    int centerY = 0;

                    // 距離
                    double d = 0;

                    // 遍歷,最適圓處理
                    for (int i = 0; i < drawing.rows(); i++) {
                        for (int j = 0; j < drawing.cols(); j++) {
                            temp = (int) destination1.get(i, j)[0];

                            if (temp > R) {
                                R = temp;
                                centerX = j;
                                centerY = i;
                            }

                            // }
                        }
                    }

                    // 半徑大於22才合理
                    if (R > 22) {
                        centerX = centerX + 30;
                        centerY = centerY + 90;
                        System.out.println("centerX=" + centerX + ",centerY=" + centerY + ",R=" + R);
                        Core.circle(webcam_image, new Point(centerX, centerY), R, new Scalar(255, 0,
                                    0));
                        Core.line(webcam_image, new Point(centerX, centerY), new Point(centerX,
                                  centerY), new Scalar(255, 0, 0), 5);

                        Core.putText(webcam_image, "(" + centerX + "," + centerY + ")",
                                     new Point(centerX, centerY), 0, 0.3, new Scalar(255, 0, 0)); //顯示掌心座標
                    }

                    // 繪出手掌判斷幾個手指頭感興趣區域,矩形
                    Core.rectangle(webcam_image, new Point(30, 90), new Point(
                                       300, 420), new Scalar(250, 250, 250), 3);
                    // 繪出掌心置放區,以十字表示
                    Core.line(webcam_image, new Point(130, 320), new Point(180,
                              320), new Scalar(250, 250, 250), 2);// 橫
                    Core.line(webcam_image, new Point(155, 295), new Point(155,
                              345), new Scalar(250, 250, 250), 2);// 豎

                    // -- 5. Display the image
                    panel1.setimagewithMat(webcam_image);
                    panel4.setimagewithMat(drawing);
                    frame1.repaint();
                    frame2.repaint();
                } else {
                    System.out.println(" --(!) No captured frame -- Break!");
                    break;
                }
            }
        }

        return;
    }

    public static int findBiggestContour(List<MatOfPoint> contours)
    {
        int indexOfBiggestContour = -1;
        int sizeOfBiggestContour = 0;

        for (int i = 0; i < contours.size(); i++) {
            if (contours.get(i).height() > sizeOfBiggestContour) {
                sizeOfBiggestContour = contours.get(i).height();
                indexOfBiggestContour = i;
            }
        }

        return indexOfBiggestContour;
    }



}

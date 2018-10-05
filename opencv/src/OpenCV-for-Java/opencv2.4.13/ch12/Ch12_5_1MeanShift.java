package ch12;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfFloat;
import org.opencv.core.MatOfInt;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;
import org.opencv.highgui.VideoCapture;
import org.opencv.imgproc.Imgproc;
import org.opencv.video.Video;
import org.opencv.core.CvType;

public class Ch12_5_1MeanShift
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String arg[]) throws Exception {
        JFrame frame1 = new JFrame("Camera");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);

        // -- 2. Read the video stream
        VideoCapture capture = new VideoCapture(0);// 0表第1支CCD,1是第2支
        Mat webcam_image = new Mat();
        Rect selection;
        Mat hsv, hue, hist, backproj, mask;
        List<Mat> hsvarray = new ArrayList<Mat>();
        List<Mat> huearray = new ArrayList<Mat>();
        capture.read(webcam_image);
        frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);

        if (capture.isOpened())
        {
            int vmin = 65, vmax = 256, smin = 55;
            // selection=new Rect(232,163,231,273);
            selection = new Rect(232, 163, 20, 25);
            MatOfInt from_to = new MatOfInt(0, 0);

            while (true) {
                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    hist = new Mat();
                    hsv = new Mat(webcam_image.size(), CvType.CV_8UC3);
                    mask = new Mat(webcam_image.size(), CvType.CV_8UC1);
                    hue = new Mat(webcam_image.size(), CvType.CV_8UC1);
                    backproj = new Mat(webcam_image.size(), CvType.CV_8UC1);
                    Imgproc.cvtColor(webcam_image, hsv, Imgproc.COLOR_BGR2HSV);
                    Core.inRange(hsv,
                                 new Scalar(0, smin, Math.min(vmin, vmax)),
                                 new Scalar(180, 256, Math.max(vmin, vmax)), mask);
                    // Core.inRange(hsv, new Scalar(0, 30, 10),new Scalar(180,
                    // 256, 256), mask);

                    hsvarray.clear();
                    huearray.clear();

                    hsvarray.add(hsv);
                    huearray.add(hue);
                    Core.mixChannels(hsvarray, huearray, from_to);
                    MatOfFloat ranges = new MatOfFloat(0f, 256f);
                    MatOfInt histSize = new MatOfInt(20);
                    List<Mat> images = Arrays.asList(huearray.get(0));

                    Imgproc.calcHist(images, new MatOfInt(0), mask, hist,
                                     histSize, ranges);
                    Core.normalize(hist, hist);
                    Imgproc.calcBackProject(huearray, new MatOfInt(0), hist,
                                            backproj, ranges, 255);
                    Core.bitwise_and(backproj, mask, backproj, new Mat());
                    // RotatedRect trackBox=Video.CamShift(backproj, selection,
                    // new TermCriteria(TermCriteria.EPS,10,1));
                    int trackBox = Video.meanShift(backproj, selection,
                                                   new TermCriteria(TermCriteria.EPS, 10, 1));
                    System.out.println("trackBox=" + trackBox);

                    // Core.ellipse( webcam_image, trackBox, new
                    // Scalar(255,0,255), 3 );
                    Core.rectangle(webcam_image, new Point(selection.x,
                                                           selection.y), new Point(selection.x
                                                                   + selection.width, selection.y + selection.height),
                                   new Scalar(255, 0, 0), 5);

                    panel1.setimagewithMat(webcam_image);
                    // panel2.setimagewithMat(outputFrame); //
                    frame1.repaint();
                    // frame2.repaint();
                } else {
                    System.out.println("無補抓任何畫面!");
                    break;
                }
            }
        }

        return;
    }

}

package ch12;
import static org.opencv.core.Core.LINE_AA;
import static org.opencv.core.Core.NORM_L1;
import static org.opencv.core.Core.absdiff;
import static org.opencv.core.Core.merge;
import static org.opencv.imgproc.Imgproc.COLOR_BGR2GRAY;
import static org.opencv.imgproc.Imgproc.THRESH_BINARY;
import static org.opencv.imgproc.Imgproc.cvtColor;
import static org.opencv.imgproc.Imgproc.threshold;
import static org.opencv.video.Video.calcGlobalOrientation;
import static org.opencv.video.Video.calcMotionGradient;
import static org.opencv.video.Video.segmentMotion;
import static org.opencv.video.Video.updateMotionHistory;

import java.util.ArrayList;
import java.util.List;

import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.Imgcodecs.VideoCapture;
import org.opencv.core.CvType;


public class Ch12_8_1UpdateMotionHistory
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String arg[]) throws Exception{
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


        VideoCapture capture = new VideoCapture(0); //0表第1支CCD,1是第2支
        Mat webcam_image = new Mat();
        //start
        int last = 0 ;
        // number of cyclic frame buffer used for motion detection
        // (should, probably, depend on FPS)
        final int N = 4 ;
        final double MHI_DURATION = 1 ;
        final double MAX_TIME_DELTA = 0.5 ;
        final double MIN_TIME_DELTA = 0.05 ;
        Mat image = new Mat(), motion, mhi, orient, mask, segmask;
        Mat [] buf;
        Size size;
        double magnitude, startTime = 0 ;



        capture.read(webcam_image);
        frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);
        frame2.setSize(webcam_image.width() + 40, webcam_image.height() + 60);


        size = new Size(webcam_image.width(), webcam_image.height());

        buf = new Mat [ N ];

        for (int i = 0 ; i < N ; i ++)
        {
            buf[i] = Mat . zeros(size, CvType . CV_8UC1);
        }

        motion = Mat . zeros(size, CvType . CV_8UC3);
        mhi = Mat . zeros(size, CvType . CV_32FC1);
        orient = Mat . zeros(size, CvType . CV_32FC1);
        segmask = Mat . zeros(size, CvType . CV_32FC1);
        mask = Mat . zeros(size, CvType . CV_8UC1);
        startTime = System . nanoTime();


        if (capture.isOpened())
        {
            while (true) {


                capture.read(webcam_image);

                if (!webcam_image.empty()) {

                    double timestamp = (System . nanoTime() - startTime) / 1e9 ;
                    int idx1 = last, idx2;
                    Mat silh;
                    cvtColor(webcam_image, buf[last], COLOR_BGR2GRAY);
                    double angle, count;

                    idx2 = (last + 1) % N ;  // index of (last - (N-1))th frame
                    last = idx2;

                    silh = buf[idx2];
                    absdiff(buf[idx1], buf[idx2], silh);
                    threshold(silh, silh, 30, 1, THRESH_BINARY);
                    updateMotionHistory(silh, mhi, timestamp, MHI_DURATION);
                    mhi . convertTo(mask, mask . type(), 255.0 / MHI_DURATION,
                                    (MHI_DURATION - timestamp) * 255.0 / MHI_DURATION);
                    motion.setTo(new Scalar(0));
                    List< Mat > list = new ArrayList< Mat > (3);
                    list . add(mask);
                    list . add(Mat . zeros(mask . size(), mask . type()));
                    list . add(Mat . zeros(mask . size(), mask . type()));
                    merge(list, motion);
                    calcMotionGradient(mhi, mask, orient, MAX_TIME_DELTA, MIN_TIME_DELTA, 3);
                    MatOfRect roi = new MatOfRect();
                    segmentMotion(mhi, segmask, roi, timestamp, MAX_TIME_DELTA);
                    int total = roi . toArray() . length;
                    Rect [] rois = roi . toArray();
                    Rect comp_rect;
                    Scalar color;

                    for (int i = - 1 ; i < total; i ++) {
                        if (i < 0) {
                            comp_rect = new Rect(0, 0, webcam_image.width(), webcam_image.height());
                            color = new Scalar(255, 255, 255);
                            magnitude = 100 ;
                        } else {
                            comp_rect = rois[i];

                            if (comp_rect . width + comp_rect . height <
                                100) { // reject very small components
                                continue ;
                            }

                            color = new Scalar(0, 0, 255);
                            magnitude = 30 ;
                        }

                        Mat silhROI = silh . submat(comp_rect);
                        Mat mhiROI = mhi . submat(comp_rect);
                        Mat orientROI = orient . submat(comp_rect);
                        Mat maskROI = mask . submat(comp_rect);

                        angle = calcGlobalOrientation(orientROI, maskROI, mhiROI, timestamp,
                                                      MHI_DURATION);
                        angle = 360.0 - angle;
                        count = Core . norm(silhROI, NORM_L1);

                        silhROI . release();
                        mhiROI . release();
                        orientROI . release();
                        maskROI . release();

                        if (count < comp_rect . height * comp_rect . width * 0.05) {
                            continue ;
                        }

                        Point center = new Point((comp_rect . x + comp_rect . width / 2),
                                                 (comp_rect . y + comp_rect . height / 2));
                        circle(motion, center, (int) Math . round(magnitude * 1.2), color, 3, LINE_AA,
                               0);
                        Core . line(motion, center, new Point(
                                        Math . round(center . x + magnitude * Math . cos(angle * Math.PI / 180)),
                                        Math . round(center . y - magnitude * Math . sin(angle * Math.PI / 180))),
                                    color, 3, LINE_AA, 0);
                    }


                    panel1.setimagewithMat(webcam_image);
                    frame1.repaint();
                    panel2.setimagewithMat(motion);  //
                    frame2.repaint();
                } else {
                    System.out.println("無補抓任何畫面!");
                    break;
                }
            }
        }

        return;
    }




}

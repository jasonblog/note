package ch14;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.List;
import javax.swing.JFrame;
import javax.swing.JPanel;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfInt;
import org.opencv.core.MatOfInt4;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Point;
import org.opencv.core.Range;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;
import org.opencv.videoio.VideoCapture;
import org.opencv.core.CvType;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.Synthesizer;
import javax.sound.midi.MidiChannel;


public class Ch14_3_1VirtualPiano
{
    public static void main(String arg[]) throws Exception{
        // Load the native library.
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        // It is better to group all frames together so cut and paste to
        // create more frames is easier
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
        frame4.setBounds(300, 100, frame1.getWidth() + 50, 50 + frame1.getHeight());
        Panel panel4 = new Panel();
        frame4.setContentPane(panel4);
        frame4.setVisible(true);
        //-- 2. Read the video stream
        VideoCapture capture = new VideoCapture();
        capture.open(0);
        Mat webcam_image = new Mat();
        Mat hsv_image = new Mat();
        Mat thresholded = new Mat();
        Mat thresholded2 = new Mat();
        capture.read(webcam_image);
        frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);
        frame4.setSize(webcam_image.width() + 40, webcam_image.height() + 60);
        Scalar hsv_min = new Scalar(0, 10, 60);
        Scalar hsv_max = new Scalar(20, 150, 255);

        Mat subArea = webcam_image.colRange(new Range(0, 184)).rowRange(0, 142);
        boolean first = false;

        if (capture.isOpened())
        {
            while (true) {
                //Thread.sleep(200);
                if (first == true) {
                    first = false;
                }


                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    // One way to select a range of colors by Hue
                    Imgproc.cvtColor(webcam_image, hsv_image, Imgproc.COLOR_BGR2HSV);
                    Core.inRange(hsv_image, hsv_min, hsv_max, thresholded);
                    Imgproc.GaussianBlur(thresholded, thresholded, new Size(15, 15), 0, 0);
                    //Imgproc.GaussianBlur(hsv_image, thresholded, new Size(25,35), 0, 0);
                    Mat threshold_output = new Mat(webcam_image.rows(), webcam_image.cols(),
                                                   CvType.CV_8UC1);
                    Imgproc.threshold(thresholded, threshold_output, 99, 255,
                                      Imgproc.THRESH_BINARY);
                    // Imgproc.GaussianBlur(thresholded, thresholded, new Size(15,15), 0, 0);
                    Mat hierarchy = new Mat(thresholded.rows(), thresholded.cols(), CvType.CV_8UC1,
                                            new Scalar(0));
                    List<MatOfPoint> contours = new ArrayList<MatOfPoint>();
                    Imgproc.findContours(threshold_output, contours, hierarchy, Imgproc.RETR_LIST,
                                         Imgproc.CHAIN_APPROX_SIMPLE);
                    //Imgproc.findContours(threshold_output, contours, hierarchy, Imgproc.RETR_TREE, Imgproc.CHAIN_APPROX_SIMPLE);
                    int s = findBiggestContour(contours);
                    //Mat drawing = Mat::zeros( src.size(), CV_8UC1 );
                    Mat drawing = new Mat(threshold_output.size(), CvType.CV_8UC1);
                    Imgproc.drawContours(drawing, contours, s, new Scalar(255, 0, 0, 255), -1);
                    int[] mConvexityDefectsIntArrayList;
                    int count = 0;

                    for (int i = 0; i < contours.size(); i++) {
                        MatOfInt4 mConvexityDefectsMatOfInt4 = new MatOfInt4();
                        MatOfInt hull = new MatOfInt();
                        MatOfPoint tempContour = contours.get(i);
                        Imgproc.convexHull(tempContour, hull, false);
                        //System.out.println("hull1:"+hull.size());

                        int index = (int)hull.get(((int)hull.size().height) - 1, 0)[0];
                        Point pt, pt0 = new Point(tempContour.get(index, 0)[0], tempContour.get(index,
                                                  0)[1]);

                        for (int j = 0; j < hull.size().height - 1 ; j++) {
                            //for(int j = 0; j < 3 ; j++){
                            index = (int) hull.get(j, 0)[0];
                            pt = new Point(tempContour.get(index, 0)[0], tempContour.get(index, 0)[1]);

                            double distance1 = Math.sqrt(Math.pow(pt0.x - pt.x, 2) + Math.pow(pt0.y - pt.y,
                                                         2));

                            if (distance1 >= 30) {
                                Imgproc.line(webcam_image, pt0, pt, new Scalar(255, 0, 100), 2);//2是厚度
                            }

                            // System.out.println("distance-hull-"+j+":" + distance);
                            pt0 = pt;
                        }

                        if (contours.size() > 0 && hull.rows() > 3) {
                            Imgproc.convexityDefects(tempContour, hull, mConvexityDefectsMatOfInt4);


                            //System.out.println( mConvexityDefectsMatOfInt4.height());
                            int index2 = (int)mConvexityDefectsMatOfInt4.toArray().length;
                            //System.out.println( "index2="+index2);
                            int index3;

                            if (tempContour.get(index2, 0) != null) {
                                Point pt3, pt1, old_pt1 = null, pt4 = null,
                                                pt2 = new Point(tempContour.get(index2, 0)[0], tempContour.get(index2, 0)[1]);

                                for (int j = 0; j < mConvexityDefectsMatOfInt4.size().height - 1 ; j++) {
                                    //for(int j = 0; j < 3 ; j++){
                                    index2 = (int) mConvexityDefectsMatOfInt4.get(j,
                                             0)[0];//0是top即指尖,2是down,兩指之底
                                    index3 = (int) mConvexityDefectsMatOfInt4.get(j, 0)[2];
                                    pt1 = new Point(tempContour.get(index2, 0)[0], tempContour.get(index2,
                                                    0)[1]);//尖
                                    pt3 = new Point(tempContour.get(index3, 0)[0], tempContour.get(index3,
                                                    0)[1]);//底




                                    Imgproc.line(webcam_image, pt1, pt1, new Scalar(0, 255, 0), 5);
                                    Imgproc.line(webcam_image, pt3, pt3, new Scalar(150, 155, 260), 5);
                                    //
                                    double distance1 = Math.sqrt(Math.pow(pt1.x - pt3.x,
                                                                          2) + Math.pow(pt1.y - pt3.y, 2));

                                    if (pt4 != null && old_pt1 != null) {
                                        double distance2 = Math.sqrt(Math.pow(pt1.x - pt4.x,
                                                                              2) + Math.pow(pt1.y - pt4.y, 2));

                                        if (distance2 > 15) {
                                            //count++;
                                            Imgproc.line(webcam_image, pt1, pt4, new Scalar(220, 0, 255), 2);
                                        }

                                        if (distance1 > 15) {
                                            //count++;
                                            Imgproc.line(webcam_image, pt1, pt3, new Scalar(220, 0, 255), 2);
                                        }

                                        // if (getAngle(pt1,pt4,pt3)>20 && getAngle(pt1,pt4,pt3)<81){
                                        if (getAngle(pt1, pt4, old_pt1) > 20 && getAngle(pt1, pt4, old_pt1) < 84) {
                                            //距離太短所構成的角度不算
                                            if (getDistance(pt1, pt4) > 25 && getDistance(pt4, old_pt1) > 25) {
                                                Imgproc.putText(webcam_image, getAngle(pt1, pt4, old_pt1) + "", pt4, 0, 0.3,
                                                                new Scalar(0, 0, 0));
                                                //System.out.println("There ang :"+getAngle(pt1,pt4,pt3));

                                                System.out.println("L1:" + getDistance(pt1, pt4) + ",L2:" + getDistance(old_pt1,
                                                                   pt4));
                                                count++;
                                            }
                                        }

                                        //System.out.println("There ang :"+getAngle(pt1,pt4,pt3));

                                    }

                                    //System.out.println("distance-"+j+":" + distance1);

                                    //System.out.println("There r :"+count);
                                    Imgproc.rectangle(webcam_image, new Point(5, 5), new Point(92, 136),
                                                      new Scalar(120, 0, 125), 3);
                                    Mat firstArea = webcam_image.colRange(new Range(1, 96)).rowRange(1, 142);
                                    Imgproc.rectangle(webcam_image, new Point(97, 5), new Point(184, 136),
                                                      new Scalar(120, 0, 125), 3);
                                    Mat secondArea = webcam_image.colRange(new Range(97, 186)).rowRange(1, 142);

                                    Rect firstRect = new Rect(0, 0, 92, 136);
                                    Rect secondRect = new Rect(97, 0, 184, 136);

                                    //Core.rectangle(webcam_image, new Point(100,300), new Point(250,450), new Scalar(250, 250, 250),2);
                                    if (firstRect.contains(pt1)) {
                                        System.out.println("YES1");

                                        if (!first) {
                                            playSound(60);
                                        }

                                        first = true;
                                        //new Thread(new PlaySound(60)).start();
                                    }

                                    if (secondRect.contains(pt1)) {
                                        System.out.println("YES2");

                                        if (!first) {
                                            playSound(64);
                                        }

                                        first = true;

                                    }


                                    pt4 = pt3;
                                    old_pt1 = pt1;



                                }

                            }


                        }




                    }




                    //Core.addWeighted(webcam_image, 0.3, firstArea, 0.7, 1.4, firstArea);
                    //Core.add(webcam_image, firstArea,  firstArea);//變很亮
                    panel1.setimagewithMat(webcam_image);
                    panel4.setimagewithMat(subArea);  //
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

    public static double getAngle(Point a, Point b, Point c)
    {
        double line1 = Math.sqrt(Math.pow(a.x - b.x, 2) + Math.pow(a.y - b.y, 2));
        double line2 = Math.sqrt(Math.pow(c.x - b.x, 2) + Math.pow(c.y - b.y, 2));
        double dot = (a.x - b.x) * (c.x - b.x) + (a.y - b.y) * (c.y - b.y);
        double angle = Math.acos(dot / (line1 * line2));
        angle = angle * 180 / Math.PI;
        return Math.round(100 * angle) / 100;

    }
    public static double getDistance(Point a, Point b)
    {
        //return Math.round(Math.sqrt(Math.pow(a.x-b.x, 2)+Math.pow(a.y-b.y, 2)*100)/100);
        return Math.sqrt(Math.pow(a.x - b.x, 2) + Math.pow(a.y - b.y, 2));
    }

    public static void playSound(int meldy)
    {
        int channel =
            0; // 0 is a piano, 9 is percussion, other channels are for other instruments

        int volume = 80; // between 0 et 127
        int duration = 200; // in milliseconds

        try {
            Synthesizer synth = MidiSystem.getSynthesizer();
            synth.open();
            MidiChannel[] channels = synth.getChannels();

            channels[channel].noteOn(meldy, volume);   // C note
            Thread.sleep(duration);
            channels[channel].noteOff(meldy);
            Thread.sleep(400);
            synth.close();
        } catch (Exception e) {
            e.printStackTrace();
        }



    }

}

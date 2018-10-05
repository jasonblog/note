package ch11;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;

import org.opencv.calib3d.Calib3d;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfByte;
import org.opencv.core.MatOfDMatch;
import org.opencv.core.MatOfKeyPoint;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.features2d.DMatch;
import org.opencv.features2d.DescriptorExtractor;
import org.opencv.features2d.DescriptorMatcher;
import org.opencv.features2d.FeatureDetector;
import org.opencv.features2d.Features2d;
import org.opencv.features2d.KeyPoint;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;
import java.util.LinkedList;
import java.util.List;

public class Ch11_6_3FlannAndAndHomographyCase
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmjavaSwing;
    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {

        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch11_6_3FlannAndAndHomographyCase window = new
                    Ch11_6_3FlannAndAndHomographyCase();
                    window.frmjavaSwing.setVisible(true);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }

    /**
     * Create the application.
     */
    public Ch11_6_3FlannAndAndHomographyCase()
    {
        initialize();
        System.out.println("--------------DescriptorExtractor-------------------");
        System.out.println("DescriptorExtractor.SURF=" + DescriptorExtractor.SURF);
        System.out.println("DescriptorExtractor.SIFT=" + DescriptorExtractor.SIFT);
        System.out.println("DescriptorExtractor.BRIEF=" + DescriptorExtractor.BRIEF);
        System.out.println("DescriptorExtractor.BRISK=" + DescriptorExtractor.BRISK);
        System.out.println("DescriptorExtractor.ORB=" + DescriptorExtractor.ORB);
        System.out.println("DescriptorExtractor.FREAK=" + DescriptorExtractor.FREAK);
        System.out.println("DescriptorExtractor.OPPONENT_BRIEF=" +
                           DescriptorExtractor.OPPONENT_BRIEF);
        System.out.println("DescriptorExtractor.OPPONENT_BRISK=" +
                           DescriptorExtractor.OPPONENT_BRISK);
        System.out.println("DescriptorExtractor.OPPONENT_FREAK=" +
                           DescriptorExtractor.OPPONENT_FREAK);
        System.out.println("DescriptorExtractor.OPPONENT_ORB=" +
                           DescriptorExtractor.OPPONENT_ORB);
        System.out.println("DescriptorExtractor.OPPONENT_SIFT=" +
                           DescriptorExtractor.OPPONENT_SIFT);
        System.out.println("DescriptorExtractor.OPPONENT_SURF=" +
                           DescriptorExtractor.OPPONENT_SURF);

        System.out.println("--------------DescriptorMatcher-------------------");
        System.out.println("DescriptorMatcher.FLANNBASED=" +
                           DescriptorMatcher.FLANNBASED);
        System.out.println("DescriptorMatcher.BRUTEFORCE=" +
                           DescriptorMatcher.BRUTEFORCE);
        System.out.println("DescriptorMatcher.BRUTEFORCE_HAMMING=" +
                           DescriptorMatcher.BRUTEFORCE_HAMMING);
        System.out.println("DescriptorMatcher.BRUTEFORCE_HAMMINGLUT=" +
                           DescriptorMatcher.BRUTEFORCE_HAMMINGLUT);
        System.out.println("DescriptorMatcher.BRUTEFORCE_L1=" +
                           DescriptorMatcher.BRUTEFORCE_L1);
        System.out.println("DescriptorMatcher.BRUTEFORCE_SL2=" +
                           DescriptorMatcher.BRUTEFORCE_SL2);
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//DSC_0864.jpg",
                                          Highgui.CV_LOAD_IMAGE_COLOR);
        final Mat object1 = Highgui.imread("C://opencv3.1//samples//goat-template5.jpg",
                                           Highgui.CV_LOAD_IMAGE_COLOR);
        final Mat object2 = Highgui.imread("C://opencv3.1//samples//goat-template3.jpg",
                                           Highgui.CV_LOAD_IMAGE_COLOR);
        BufferedImage image = matToBufferedImage(source);
        BufferedImage object_image1 = matToBufferedImage(object1);
        BufferedImage object_image2 = matToBufferedImage(object2);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("利用二維特徵點尋找物體");
        frmjavaSwing.setBounds(100, 100, 543, 502);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(13, 195, 499, 265);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider = new JSlider();

        slider.setValue(1);
        slider.setMinimum(1);
        slider.setMaximum(12);
        slider.setBounds(97, 10, 226, 25);
        frmjavaSwing.getContentPane().add(slider);

        JLabel lblNewLabel_1 = new JLabel("Extractor");
        lblNewLabel_1.setBounds(10, 10, 75, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);

        final JLabel lblNewLabel_2 = new JLabel("1");
        lblNewLabel_2.setBounds(353, 20, 46, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_2);

        final JSlider slider_1 = new JSlider();

        slider_1.setValue(1);
        slider_1.setMinimum(1);
        slider_1.setMaximum(6);
        slider_1.setBounds(97, 45, 226, 25);
        frmjavaSwing.getContentPane().add(slider_1);

        final JLabel label = new JLabel("1");
        label.setBounds(353, 45, 46, 15);
        frmjavaSwing.getContentPane().add(label);

        JLabel lblNewLabel_3 = new JLabel("");
        lblNewLabel_3.setBounds(439, 10, 61, 70);
        lblNewLabel_3.setIcon(new ImageIcon(object_image1));
        frmjavaSwing.getContentPane().add(lblNewLabel_3);

        JLabel lblNewLabel_4 = new JLabel("Matcher");
        lblNewLabel_4.setBounds(10, 45, 75, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_4);

        JLabel lblSearch = new JLabel("search");
        lblSearch.setBounds(396, 30, 46, 15);
        frmjavaSwing.getContentPane().add(lblSearch);

        JLabel label_1 = new JLabel("Extractor");
        label_1.setBounds(10, 115, 75, 15);
        frmjavaSwing.getContentPane().add(label_1);

        JLabel label_2 = new JLabel("Matcher");
        label_2.setBounds(10, 150, 75, 15);
        frmjavaSwing.getContentPane().add(label_2);

        JSlider slider_g2e = new JSlider();

        slider_g2e.setValue(1);
        slider_g2e.setMinimum(1);
        slider_g2e.setMaximum(12);
        slider_g2e.setBounds(97, 115, 226, 25);
        frmjavaSwing.getContentPane().add(slider_g2e);

        JSlider slider_g2m = new JSlider();

        slider_g2m.setValue(1);
        slider_g2m.setMinimum(1);
        slider_g2m.setMaximum(6);
        slider_g2m.setBounds(97, 150, 226, 25);
        frmjavaSwing.getContentPane().add(slider_g2m);

        JLabel label_g21 = new JLabel("1");
        label_g21.setBounds(353, 125, 46, 15);
        frmjavaSwing.getContentPane().add(label_g21);

        JLabel label_g22 = new JLabel("1");
        label_g22.setBounds(353, 150, 46, 15);
        frmjavaSwing.getContentPane().add(label_g22);

        JLabel label_5 = new JLabel("search");
        label_5.setBounds(396, 135, 46, 15);
        frmjavaSwing.getContentPane().add(label_5);

        JLabel label_6 = new JLabel("");
        label_6.setBounds(439, 115, 61, 70);
        label_6.setIcon(new ImageIcon(object_image2));
        frmjavaSwing.getContentPane().add(label_6);

        JSlider slider_2 = new JSlider();
        slider_2.setMinimum(1);

        slider_2.setValue(1);
        slider_2.setMaximum(44);
        slider_2.setMajorTickSpacing(1);
        slider_2.setBounds(123, 80, 200, 24);
        frmjavaSwing.getContentPane().add(slider_2);

        JLabel lblNewLabel_5 = new JLabel("FeatureDetector");
        lblNewLabel_5.setBounds(13, 80, 100, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_5);

        JLabel Label_FeatureDetector = new JLabel("1");
        Label_FeatureDetector.setBounds(333, 80, 46, 15);
        frmjavaSwing.getContentPane().add(Label_FeatureDetector);
        //group1
        slider.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                BufferedImage newImage;

                if (slider.getValue() < 7) {
                    lblNewLabel_2.setText(slider.getValue() + "");
                    //System.out.println("uuuu="+Integer.parseInt(Label_FeatureDetector.getText()));
                    newImage = matToBufferedImage(Features2D(object1, slider.getValue(),
                                                  slider_1.getValue(), Integer.parseInt(Label_FeatureDetector.getText())));
                } else {
                    lblNewLabel_2.setText((slider.getValue() - 6 + 1000) + "");
                    newImage = matToBufferedImage(Features2D(object1,
                                                  (slider.getValue() - 6 + 1000), slider_1.getValue(),
                                                  Integer.parseInt(Label_FeatureDetector.getText())));
                }


                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_1.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                BufferedImage newImage;
                label.setText(slider_1.getValue() + "");

                if (slider.getValue() < 7) {
                    newImage = matToBufferedImage(Features2D(object1, slider.getValue(),
                                                  slider_1.getValue(), Integer.parseInt(Label_FeatureDetector.getText())));
                } else {
                    newImage = matToBufferedImage(Features2D(object1,
                                                  (slider.getValue() - 6 + 1000), slider_1.getValue(),
                                                  Integer.parseInt(Label_FeatureDetector.getText())));
                }

                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        //group2
        slider_g2e.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                BufferedImage newImage;

                if (slider_g2e.getValue() < 7) {
                    label_g21.setText(slider_g2e.getValue() + "");
                    newImage = matToBufferedImage(Features2D(object2, slider_g2e.getValue(),
                                                  slider_g2m.getValue(), Integer.parseInt(Label_FeatureDetector.getText())));
                } else {
                    label_g21.setText((slider_g2e.getValue() - 6 + 1000) + "");
                    newImage = matToBufferedImage(Features2D(object2,
                                                  (slider_g2e.getValue() - 6 + 1000), slider_g2m.getValue(),
                                                  Integer.parseInt(Label_FeatureDetector.getText())));
                }

                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_g2m.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent e) {
                label_g22.setText(slider_g2m.getValue() + "");
                BufferedImage newImage;

                if (slider_g2e.getValue() < 7) {
                    newImage = matToBufferedImage(Features2D(object2, slider_g2e.getValue(),
                                                  slider_g2m.getValue(), Integer.parseInt(Label_FeatureDetector.getText())));
                } else {
                    newImage = matToBufferedImage(Features2D(object2,
                                                  (slider_g2e.getValue() - 6 + 1000), slider_g2m.getValue(),
                                                  Integer.parseInt(Label_FeatureDetector.getText())));
                }

                lblNewLabel.setIcon(new ImageIcon(newImage));

            }
        });
        slider_2.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent e) {


                if (slider_2.getValue() < 12) {
                    Label_FeatureDetector.setText(slider_2.getValue() + "");
                } else if (slider_2.getValue() >= 12 && slider_2.getValue() < 23) {
                    Label_FeatureDetector.setText((slider_2.getValue() - 11 + 1000) + "");
                } else if (slider_2.getValue() >= 23 && slider_2.getValue() < 34) {
                    Label_FeatureDetector.setText((slider_2.getValue() - 22 + 2000) + "");
                } else {
                    Label_FeatureDetector.setText((slider_2.getValue() - 33 + 3000) + "");

                }

            }
        });
    }
    public Mat Features2D(Mat object, int Extractor, int Matcher,
                          int featureDetectorType)
    {
        //尋找已知物,object

        //欲尋找影像
        String scene_filename = "C://opencv3.1//samples//DSC_0864.jpg";

        Mat img_scene = Highgui.imread(scene_filename, Highgui.CV_LOAD_IMAGE_GRAYSCALE);
        Mat img_sceneRGB = Highgui.imread(scene_filename, Highgui.CV_LOAD_IMAGE_COLOR);

        Mat objectGray = new Mat();
        Imgproc.cvtColor(object, objectGray, Imgproc.COLOR_BGR2GRAY);


        FeatureDetector detector = FeatureDetector.create(
                                       featureDetectorType); // 4 = FeatureDetector.SURF

        MatOfKeyPoint keypoints_object = new MatOfKeyPoint();
        MatOfKeyPoint keypoints_scene = new MatOfKeyPoint();

        detector.detect(object, keypoints_object);
        detector.detect(img_sceneRGB, keypoints_scene);

        //宣告所要用的extractor
        DescriptorExtractor extractor = DescriptorExtractor.create(Extractor);

        Mat descriptor_object = new Mat();
        Mat descriptor_scene = new Mat();

        extractor.compute(object, keypoints_object, descriptor_object);
        extractor.compute(img_sceneRGB, keypoints_scene, descriptor_scene);

        //宣告所要用的matcher
        DescriptorMatcher matcher = DescriptorMatcher.create(Matcher);
        MatOfDMatch matches = new MatOfDMatch();

        matcher.match(descriptor_object, descriptor_scene, matches);
        List<DMatch> matchesList = matches.toList();

        Double max_dist = 0.0;
        Double min_dist = 100.0;

        //計算關鍵點之間的最大及最小距離
        for (int i = 0; i < matchesList.size(); i++) {
            Double dist = (double) matchesList.get(i).distance;

            if (dist < min_dist) {
                min_dist = dist;
            }

            if (dist > max_dist) {
                max_dist = dist;
            }
        }

        System.out.println("-- Max dist : " + max_dist);
        System.out.println("-- Min dist : " + min_dist);

        LinkedList<DMatch> good_matches = new LinkedList<DMatch>();
        MatOfDMatch gm = new MatOfDMatch();

        //計算出有配對成功關鍵點,且該距離小於3*min_dist
        for (int i = 0; i < matchesList.size(); i++) {
            if (matchesList.get(i).distance < (3 * min_dist)) {
                good_matches.addLast(matchesList.get(i));
            }
        }


        gm.fromList(good_matches);

        Mat img_matches = new Mat();
        //繪出有配對成功的關鍵點
        Features2d.drawMatches(object, keypoints_object, img_sceneRGB,
                               keypoints_scene, gm, img_matches, new Scalar(255, 0, 0),
                               new Scalar(0, 0, 255), new MatOfByte(), 2);

        //-----------add Homography parts
        LinkedList<Point> objList = new LinkedList<Point>();
        LinkedList<Point> sceneList = new LinkedList<Point>();

        List<KeyPoint> keypoints_objectList = keypoints_object.toList();
        List<KeyPoint> keypoints_sceneList = keypoints_scene.toList();

        // 收集配對成功的關鍵點
        for (int i = 0; i < good_matches.size(); i++) {
            objList.addLast(keypoints_objectList.get(good_matches.get(i).queryIdx).pt);
            sceneList
            .addLast(keypoints_sceneList.get(good_matches.get(i).trainIdx).pt);
        }

        MatOfPoint2f obj = new MatOfPoint2f();
        obj.fromList(objList);

        MatOfPoint2f scene = new MatOfPoint2f();
        scene.fromList(sceneList);

        Mat H = Calib3d.findHomography(obj, scene);

        Mat obj_corners = new Mat(4, 1, CvType.CV_32FC2);
        Mat scene_corners = new Mat(4, 1, CvType.CV_32FC2);


        //尋找已知物的4個角,加上偏移值
        obj_corners.put(0, 0, new double[] {0 + object.cols(), 0});
        obj_corners.put(1, 0, new double[] {object.cols() + object.cols(), 0});
        obj_corners.put(2, 0, new double[] {object.cols() + object.cols(), object.rows()});
        obj_corners.put(3, 0, new double[] {0 + object.cols(), object.rows()});

        Core.perspectiveTransform(obj_corners, scene_corners, H);

        //繪出已知物的4周,使用青色
        Core.line(img_matches, new Point(scene_corners.get(0, 0)),
                  new Point(scene_corners.get(1, 0)), new Scalar(0, 255, 0), 4);
        Core.line(img_matches, new Point(scene_corners.get(1, 0)),
                  new Point(scene_corners.get(2, 0)), new Scalar(0, 255, 0), 4);
        Core.line(img_matches, new Point(scene_corners.get(2, 0)),
                  new Point(scene_corners.get(3, 0)), new Scalar(0, 255, 0), 4);
        Core.line(img_matches, new Point(scene_corners.get(3, 0)),
                  new Point(scene_corners.get(0, 0)), new Scalar(0, 255, 0), 4);

        return img_matches;
    }

    public BufferedImage matToBufferedImage(Mat matrix)
    {
        int cols = matrix.cols();
        int rows = matrix.rows();
        int elemSize = (int)matrix.elemSize();
        byte[] data = new byte[cols * rows * elemSize];
        int type;
        matrix.get(0, 0, data);

        switch (matrix.channels()) {
        case 1:
            type = BufferedImage.TYPE_BYTE_GRAY;
            break;

        case 3:
            type = BufferedImage.TYPE_3BYTE_BGR;
            // bgr to rgb
            byte b;

            for (int i = 0; i < data.length; i = i + 3) {
                b = data[i];
                data[i] = data[i + 2];
                data[i + 2] = b;
            }

            break;

        default:
            return null;
        }

        BufferedImage image2 = new BufferedImage(cols, rows, type);
        image2.getRaster().setDataElements(0, 0, cols, rows, data);
        return image2;
    }
}

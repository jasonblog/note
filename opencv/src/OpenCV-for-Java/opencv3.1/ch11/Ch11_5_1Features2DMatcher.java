package ch11;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.DMatch;
import org.opencv.core.Mat;
import org.opencv.core.MatOfByte;
import org.opencv.core.MatOfDMatch;
import org.opencv.core.MatOfKeyPoint;
import org.opencv.core.Scalar;
import org.opencv.features2d.DescriptorExtractor;
import org.opencv.features2d.DescriptorMatcher;
import org.opencv.features2d.FeatureDetector;
import org.opencv.features2d.Features2d;
import org.opencv.imgcodecs.Imgcodecs;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;
import java.util.LinkedList;
import java.util.List;

public class Ch11_5_1Features2DMatcher
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
                    Ch11_5_1Features2DMatcher window = new Ch11_5_1Features2DMatcher();
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
    public Ch11_5_1Features2DMatcher()
    {
        initialize();
        System.out.println("--------------DescriptorExtractor-------------------");
        System.out.println("DescriptorExtractor.SURF=" + DescriptorExtractor.SURF);
        System.out.println("DescriptorExtractor.SIFT=" + DescriptorExtractor.SIFT);
        System.out.println("DescriptorExtractor.BRIEF=" + DescriptorExtractor.BRIEF);
        System.out.println("DescriptorExtractor.BRISK=" + DescriptorExtractor.BRISK);
        System.out.println("DescriptorExtractor.ORB=" + DescriptorExtractor.ORB);
        System.out.println("DescriptorExtractor.FREAK=" + DescriptorExtractor.FREAK);
        System.out.println("DescriptorExtractor.AKAZE=" + DescriptorExtractor.AKAZE);
        System.out.println("DescriptorExtractor.OPPONENT_SURF=" +
                           DescriptorExtractor.OPPONENT_SURF);
        System.out.println("DescriptorExtractor.OPPONENT_SIFT=" +
                           DescriptorExtractor.OPPONENT_SIFT);
        System.out.println("DescriptorExtractor.OPPONENT_ORB=" +
                           DescriptorExtractor.OPPONENT_ORB);
        System.out.println("DescriptorExtractor.OPPONENT_BRIEF=" +
                           DescriptorExtractor.OPPONENT_BRIEF);
        System.out.println("DescriptorExtractor.OPPONENT_BRISK=" +
                           DescriptorExtractor.OPPONENT_BRISK);
        System.out.println("DescriptorExtractor.OPPONENT_AKAZE=" +
                           DescriptorExtractor.OPPONENT_AKAZE);
        System.out.println("DescriptorExtractor.OPPONENT_FREAK=" +
                           DescriptorExtractor.OPPONENT_FREAK);

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
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//stuff.jpg",
                                            Imgcodecs.CV_LOAD_IMAGE_COLOR);
        final Mat object = Imgcodecs.imread("C://opencv3.1//samples//stuff-b.jpg",
                                            Imgcodecs.CV_LOAD_IMAGE_COLOR);
        BufferedImage image = matToBufferedImage(source);
        BufferedImage object_image = matToBufferedImage(object);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("利用二維特徵點尋找物體");
        frmjavaSwing.setBounds(100, 100, 638, 602);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 99, 517, 459);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider = new JSlider();

        slider.setValue(1);
        slider.setMinimum(1);
        slider.setMaximum(14);
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
        lblNewLabel_3.setBounds(537, 45, 83, 222);
        lblNewLabel_3.setIcon(new ImageIcon(object_image));
        frmjavaSwing.getContentPane().add(lblNewLabel_3);

        JLabel lblNewLabel_4 = new JLabel("Matcher");
        lblNewLabel_4.setBounds(10, 45, 75, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_4);

        JLabel lblSearch = new JLabel("search");
        lblSearch.setBounds(549, 20, 46, 15);
        frmjavaSwing.getContentPane().add(lblSearch);

        slider.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                BufferedImage newImage;

                if (slider.getValue() < 8) {
                    lblNewLabel_2.setText(slider.getValue() + "");
                    newImage = matToBufferedImage(Features2D(slider.getValue(),
                                                  slider_1.getValue()));
                } else {
                    lblNewLabel_2.setText((slider.getValue() - 7 + 1000) + "");
                    newImage = matToBufferedImage(Features2D((slider.getValue() - 7 + 1000),
                                                  slider_1.getValue()));
                }

                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_1.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                label.setText(slider_1.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Features2D(slider.getValue(),
                                         slider_1.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat Features2D(int Extractor, int Matcher)
    {
        //尋找已知物
        String object_filename = "C://opencv3.1//samples//stuff-b.jpg";

        //欲尋找影像
        String scene_filename = "C://opencv3.1//samples//stuff.jpg";

        Mat img_object = Imgcodecs.imread(object_filename,
                                          Imgcodecs.CV_LOAD_IMAGE_COLOR); // 0 =
        // CV_LOAD_IMAGE_GRAYSCALE
        Mat img_scene = Imgcodecs.imread(scene_filename, Imgcodecs.CV_LOAD_IMAGE_COLOR);

        FeatureDetector detector = FeatureDetector.create(5); // 4 = SURF

        MatOfKeyPoint keypoints_object = new MatOfKeyPoint();
        MatOfKeyPoint keypoints_scene = new MatOfKeyPoint();

        detector.detect(img_object, keypoints_object);
        detector.detect(img_scene, keypoints_scene);

        //宣告所要用的extractor
        DescriptorExtractor extractor = DescriptorExtractor.create(Extractor);

        Mat descriptor_object = new Mat();
        Mat descriptor_scene = new Mat();

        extractor.compute(img_object, keypoints_object, descriptor_object);
        extractor.compute(img_scene, keypoints_scene, descriptor_scene);

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
        Features2d.drawMatches(img_object, keypoints_object, img_scene,
                               keypoints_scene, gm, img_matches, new Scalar(255, 0, 0),
                               new Scalar(0, 0, 255), new MatOfByte(), 2);

        //Features2d.drawMatches2(img1, keypoints1, img2, keypoints2, matches1to2, outImg, matchColor, singlePointColor, matchesMask, flags);

        //在終端機輸出有配對成功的關鍵點詳細資訊
        for (int kk = 0; kk < good_matches.size(); kk++) {
            System.out.println("Match-point-" + kk + ",keypoint-queryIdx=" +
                               good_matches.get(kk).queryIdx + ",keypoint-trainIdx=" + good_matches.get(
                                   kk).trainIdx + ",keypoint-distance=" + good_matches.get(kk).distance);
        }

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

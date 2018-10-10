package ch11;

import java.awt.EventQueue;
import java.awt.Graphics;
import java.awt.Panel;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.JLayeredPane;
import java.awt.Color;
import java.awt.image.BufferedImage;

import org.opencv.calib3d.Calib3d;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfByte;
import org.opencv.core.MatOfDMatch;
import org.opencv.core.MatOfKeyPoint;
import org.opencv.core.MatOfPoint;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.core.TermCriteria;
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
import javax.swing.JButton;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.LinkedList;
import java.util.List;

public class Ch11_6_1FlannBaseAndHomography
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
                    Ch11_6_1FlannBaseAndHomography window = new Ch11_6_1FlannBaseAndHomography();
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
    public Ch11_6_1FlannBaseAndHomography()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//stuff.jpg",
                                          Highgui.CV_LOAD_IMAGE_COLOR);
        final Mat object = Highgui.imread("C://opencv3.1//samples//stuff-b.jpg",
                                          Highgui.CV_LOAD_IMAGE_COLOR);
        BufferedImage image = matToBufferedImage(source);
        BufferedImage object_image = matToBufferedImage(object);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("利用二維特徵點(SURF+FLANN)及單映射尋找物體");
        frmjavaSwing.setBounds(100, 100, 638, 602);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 99, 517, 459);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        JLabel lblNewLabel_3 = new JLabel("");
        lblNewLabel_3.setBounds(537, 45, 83, 222);
        lblNewLabel_3.setIcon(new ImageIcon(object_image));
        frmjavaSwing.getContentPane().add(lblNewLabel_3);

        JLabel lblSearch = new JLabel("search");
        lblSearch.setBounds(549, 20, 46, 15);
        frmjavaSwing.getContentPane().add(lblSearch);

        JButton btnNewButton = new JButton("Process");
        btnNewButton.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent arg0) {
                BufferedImage newImage = matToBufferedImage(Features2D());
                lblNewLabel.setIcon(new ImageIcon(newImage));

            }
        });
        btnNewButton.setBounds(10, 10, 204, 53);
        frmjavaSwing.getContentPane().add(btnNewButton);
    }
    public Mat Features2D()
    {
        String object_filename = "C://opencv3.1//samples//stuff-b.jpg";
        String scene_filename = "C://opencv3.1//samples//stuff.jpg";

        Mat img_object = Highgui.imread(object_filename,
                                        Highgui.CV_LOAD_IMAGE_GRAYSCALE); // 0 =
        // CV_LOAD_IMAGE_GRAYSCALE
        Mat img_scene = Highgui.imread(scene_filename, Highgui.CV_LOAD_IMAGE_GRAYSCALE);

        FeatureDetector detector = FeatureDetector.create(
                                       FeatureDetector.SURF); // 4 = SURF

        MatOfKeyPoint keypoints_object = new MatOfKeyPoint();
        MatOfKeyPoint keypoints_scene = new MatOfKeyPoint();

        detector.detect(img_object, keypoints_object);
        detector.detect(img_scene, keypoints_scene);

        DescriptorExtractor extractor = DescriptorExtractor.create(
                                            DescriptorExtractor.SURF);

        Mat descriptor_object = new Mat();
        Mat descriptor_scene = new Mat();

        extractor.compute(img_object, keypoints_object, descriptor_object);
        extractor.compute(img_scene, keypoints_scene, descriptor_scene);

        DescriptorMatcher matcher = DescriptorMatcher.create(
                                        DescriptorMatcher.FLANNBASED);
        MatOfDMatch matches = new MatOfDMatch();

        matcher.match(descriptor_object, descriptor_scene, matches);
        List<DMatch> matchesList = matches.toList();

        Double max_dist = 0.0;
        Double min_dist = 100.0;

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

        for (int i = 0; i < matchesList.size(); i++) {
            if (matchesList.get(i).distance < (3 * min_dist)) {
                good_matches.addLast(matchesList.get(i));
            }
        }


        gm.fromList(good_matches);

        Mat img_matches = new Mat();
        Features2d.drawMatches(img_object, keypoints_object, img_scene,
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
            sceneList.addLast(keypoints_sceneList.get(good_matches.get(i).trainIdx).pt);
        }

        MatOfPoint2f obj = new MatOfPoint2f();
        obj.fromList(objList);

        MatOfPoint2f scene = new MatOfPoint2f();
        scene.fromList(sceneList);

        Mat H = Calib3d.findHomography(obj, scene);

        Mat obj_corners = new Mat(4, 1, CvType.CV_32FC2);
        Mat scene_corners = new Mat(4, 1, CvType.CV_32FC2);


        //尋找已知物的4個角,加上偏移值
        obj_corners.put(0, 0, new double[] {0 + img_object.cols(), 0});
        obj_corners.put(1, 0, new double[] {img_object.cols() + img_object.cols(), 0});
        obj_corners.put(2, 0, new double[] {img_object.cols() + img_object.cols(), img_object.rows()});
        obj_corners.put(3, 0, new double[] {0 + img_object.cols(), img_object.rows()});

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

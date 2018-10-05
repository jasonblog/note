package ch11;
import java.util.LinkedList;
import java.util.List;
import javax.swing.JFrame;

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
import org.opencv.highgui.VideoCapture;
import org.opencv.imgproc.Imgproc;

public class Ch11_6_2FlannAndHomographyByWebCam
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String arg[]) throws Exception{
        // It is better to group all frames together so cut and paste to
        // create more frames is easier
        JFrame frame1 = new JFrame("Camera");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);

        //-- 2. Read the video stream
        VideoCapture capture = new VideoCapture(0);
        Mat webcam_image = new Mat();

        Mat thresholded = new Mat();
        Mat thresholded2 = new Mat();
        capture.read(webcam_image);
        frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);

        if (capture.isOpened())
        {
            while (true) {
                Thread.sleep(200);


                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    Mat hsv_image = new Mat();
                    String object_filename = "C://opencv3.1//samples//template1.jpg";
                    Mat img_object = Highgui.imread(object_filename,
                                                    Highgui.CV_LOAD_IMAGE_GRAYSCALE); // 0 =
                    // CV_LOAD_IMAGE_GRAYSCALE
                    Imgproc.cvtColor(webcam_image, hsv_image, Imgproc.COLOR_BGR2GRAY);

                    FeatureDetector detector = FeatureDetector.create(
                        FeatureDetector.SURF); // 4 = SURF

                    MatOfKeyPoint keypoints_object = new MatOfKeyPoint();
                    MatOfKeyPoint keypoints_scene = new MatOfKeyPoint();

                    detector.detect(img_object, keypoints_object);
                    detector.detect(hsv_image, keypoints_scene);

                    DescriptorExtractor extractor = DescriptorExtractor.create(
                        DescriptorExtractor.SURF); // 2 =
                    // SURF;

                    Mat descriptor_object = new Mat();
                    Mat descriptor_scene = new Mat();

                    extractor.compute(img_object, keypoints_object, descriptor_object);
                    extractor.compute(hsv_image, keypoints_scene, descriptor_scene);

                    DescriptorMatcher matcher = DescriptorMatcher.create(
                        DescriptorMatcher.FLANNBASED); // 1 =
                    // FLANNBASED
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
                    Features2d.drawMatches(img_object, keypoints_object, hsv_image,
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




                    panel1.setimagewithMat(img_matches);
                    frame1.repaint();
                } else {
                    System.out.println(" 無補抓任何畫面!");
                    break;
                }
            }
        }

        return;
    }




}

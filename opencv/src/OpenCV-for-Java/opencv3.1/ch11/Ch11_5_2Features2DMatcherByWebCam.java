package ch11;
import java.util.LinkedList;
import java.util.List;
import javax.swing.JFrame;
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
import org.opencv.imgproc.Imgproc;
import org.opencv.videoio.VideoCapture;

public class Ch11_5_2Features2DMatcherByWebCam
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
        VideoCapture capture = new VideoCapture();
        capture.open(0);
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
                    String object_filename = "C://opencv3.1//samples//target1.jpg";
                    Mat img_object = Imgcodecs.imread(object_filename,
                                                      Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE); // 0 =
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

package ch12;
import java.util.List;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfDouble;
import org.opencv.core.MatOfFloat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.highgui.VideoCapture;
import org.opencv.objdetect.HOGDescriptor;

public class Ch12_2_1HOGDescriptor
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String arg[])
    {
        VideoCapture capture = new VideoCapture("C://opencv3.1//samples//768x576.avi");
        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("行人檢測");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);

        if (!capture.isOpened()) {
            System.out.println("Error");
        } else {
            Mat webcam_image = new Mat();
            capture.read(webcam_image);
            frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);

            while (true) {
                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    webcam_image = detect(webcam_image);
                    panel1.setimagewithMat(webcam_image);
                    frame1.repaint();
                } else {
                    System.out.println("影片讀取結束!");
                    capture.release();
                    break;
                }
            }
        }
    }
    public static Mat detect(Mat inputframe)
    {
        final Mat mat = inputframe;
        // final HOGDescriptor hog = new HOGDescriptor(new Size(128, 64),
        // new Size(16, 16), new Size(8, 8), new Size(8, 8), 9, 0, -1, 0,
        // 0.2, false, 64);
        final HOGDescriptor hog = new HOGDescriptor();
        final MatOfFloat descriptors = HOGDescriptor.getDefaultPeopleDetector();
        hog.setSVMDetector(descriptors);
        final MatOfRect foundLocations = new MatOfRect();
        final MatOfDouble foundWeights = new MatOfDouble();
        final Size winStride = new Size(8, 8);
        final Size padding = new Size(32, 32);
        final Point rectPoint1 = new Point();
        final Point rectPoint2 = new Point();
        final Point fontPoint = new Point();
        int frames = 0;
        int framesWithPeople = 0;
        //框著行人顏色
        final Scalar rectColor = new Scalar(0, 255, 0);
        //字顏色
        final Scalar fontColor = new Scalar(255, 255, 255);
        //
        hog.detectMultiScale(mat, foundLocations, foundWeights, 0.0,
                             winStride, padding, 1.05, 2.0, false);

        // CHECKSTYLE:ON MagicNumber
        if (foundLocations.rows() > 0) {
            framesWithPeople++;
            List<Double> weightList = foundWeights.toList();
            List<Rect> rectList = foundLocations.toList();
            int i = 0;

            for (Rect rect : rectList) {
                rectPoint1.x = rect.x;
                rectPoint1.y = rect.y;
                rectPoint2.x = rect.x + rect.width;
                rectPoint2.y = rect.y + rect.height;
                // Draw rectangle around fond object
                Core.rectangle(mat, rectPoint1, rectPoint2, rectColor, 2);
                fontPoint.x = rect.x;
                // CHECKSTYLE:OFF MagicNumber - Magic numbers here for
                // illustration
                fontPoint.y = rect.y - 4;
                // CHECKSTYLE:ON MagicNumber
                // Print weight
                // CHECKSTYLE:OFF MagicNumber - Magic numbers here for
                // illustration
                Core.putText(mat,
                             String.format("%1.2f", weightList.get(i)),
                             fontPoint, Core.FONT_HERSHEY_PLAIN, 1.5, fontColor,
                             2, Core.LINE_AA, false);
                // CHECKSTYLE:ON MagicNumber
                i++;
            }
        }

        frames++;
        return mat;
    }


}

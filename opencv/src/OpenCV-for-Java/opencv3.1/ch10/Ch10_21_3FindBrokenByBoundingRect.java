package ch10;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.List;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch10_21_3FindBrokenByBoundingRect
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
                    Ch10_21_3FindBrokenByBoundingRect window = new
                    Ch10_21_3FindBrokenByBoundingRect();
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
    public Ch10_21_3FindBrokenByBoundingRect()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 使用最適外框矩形面積判斷斷裂之方法");
        frmjavaSwing.setBounds(100, 100, 574, 391);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 10, 532, 325);
        frmjavaSwing.getContentPane().add(lblNewLabel);
        BufferedImage newImage = matToBufferedImage(BoundingRect());
        lblNewLabel.setIcon(new ImageIcon(newImage));

    }
    public Mat BoundingRect()
    {

        Mat source = Imgcodecs.imread("C://opencv3.1//samples//IMAG0424.jpg",
                                      Imgcodecs.CV_LOAD_IMAGE_COLOR);
        Mat HSVprocess = matConvertTo();

        List<MatOfPoint> contours = new ArrayList<MatOfPoint>();
        Mat hierarchy = new Mat(source.rows(), source.cols(), CvType.CV_8UC1,
                                new Scalar(0));
        Imgproc.findContours(HSVprocess, contours, hierarchy, Imgproc.RETR_TREE,
                             Imgproc.CHAIN_APPROX_SIMPLE);

        Mat drawing = Mat.zeros(HSVprocess.size(), CvType.CV_8UC3);

        for (int i = 0; i < contours.size(); i++) {
            Rect r = Imgproc.boundingRect(contours.get(i));
            Imgproc.drawContours(drawing, contours, i, new Scalar(255, 0, 0, 255), 3);

            int ContourArea = r.height * r.width;
            // if(ContourArea > 700)
            //{
            System.out.println("面積Rect-" + i + ":" + ContourArea);
            Imgproc.rectangle(source, new Point(r.x, r.y), new Point(r.x + r.width,
                              r.y + r.height), new Scalar(0, 255, 0), 3);
            //  }
        }

        return source;
    }
    public Mat matConvertTo()
    {
        Mat source2 = Imgcodecs.imread("C://opencv3.1//samples//IMAG0424.jpg");
        Mat hsv_image = new Mat();
        Imgproc.GaussianBlur(source2, source2, new Size(3, 3), 0, 0);
        Imgproc.cvtColor(source2, hsv_image, Imgproc.COLOR_BGR2HSV);
        Mat thresholded = new Mat();
        Core.inRange(hsv_image, new Scalar(24, 37, 121), new Scalar(69, 116, 255),
                     thresholded);
        return thresholded;

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

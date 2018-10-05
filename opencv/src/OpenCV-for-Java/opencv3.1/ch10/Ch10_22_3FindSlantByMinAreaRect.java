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
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.RotatedRect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch10_22_3FindSlantByMinAreaRect
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmjavaSwing;
    double alpha = 2;
    double beta = 50;
    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {

        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch10_22_3FindSlantByMinAreaRect window = new Ch10_22_3FindSlantByMinAreaRect();
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
    public Ch10_22_3FindSlantByMinAreaRect()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 使用最適外框四邊形判斷有歪斜線之方法");
        frmjavaSwing.setBounds(100, 100, 574, 383);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 10, 532, 325);
        frmjavaSwing.getContentPane().add(lblNewLabel);


        BufferedImage newImage = matToBufferedImage(MinAreaRect());
        lblNewLabel.setIcon(new ImageIcon(newImage));
    }
    public Mat MinAreaRect()
    {
        Mat source =  Imgcodecs.imread("C://opencv3.1//samples//IMAG0423.jpg",
                                       Imgcodecs.CV_LOAD_IMAGE_COLOR);
        Mat HSVprocess = matConvertTo();

        List<MatOfPoint> contours = new ArrayList<MatOfPoint>();
        Mat hierarchy = new Mat(source.rows(), source.cols(), CvType.CV_8UC1,
                                new Scalar(0));
        Imgproc.findContours(HSVprocess, contours, hierarchy, Imgproc.RETR_TREE,
                             Imgproc.CHAIN_APPROX_SIMPLE);

        Mat drawing = Mat.zeros(HSVprocess.size(), CvType.CV_8UC3);
        Point[] allPoint = new Point[4];

        for (int i = 0; i < contours.size(); i++) {
            RotatedRect r = Imgproc.minAreaRect(new MatOfPoint2f(contours.get(
                                                    i).toArray()));
            Imgproc.drawContours(drawing, contours, i, new Scalar(255, 0, 0, 255), 3);

            r.points(allPoint);
            System.out.println("第" + i + "組四邊形:");
            System.out.println("p0=" + allPoint[0].x + "," + allPoint[0].y);
            System.out.println("p1=" + allPoint[1].x + "," + allPoint[1].y);
            System.out.println("p2=" + allPoint[2].x + "," + allPoint[2].y);
            System.out.println("p3=" + allPoint[3].x + "," + allPoint[3].y);

            if (Math.abs(allPoint[2].x - allPoint[1].x) > 3) {
                for (int j = 0; j < 4; j++) {
                    Imgproc.line(source, allPoint[j], allPoint[(j + 1) % 4], new Scalar(0, 255, 0),
                                 2);
                }
            }
        }

        return source;
    }
    public Mat matConvertTo()
    {
        Mat source2 = Imgcodecs.imread("C://opencv3.1//samples//IMAG0423.jpg");
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

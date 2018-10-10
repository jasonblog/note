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
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch10_5_3MinEnclosingTriangle
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
                    Ch10_5_3MinEnclosingTriangle window = new Ch10_5_3MinEnclosingTriangle();
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
    public Ch10_5_3MinEnclosingTriangle()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//stuff.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 尋找輪廓並且繪出圓形");
        frmjavaSwing.setBounds(100, 100, 687, 639);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showShapeValue = new JLabel("1");
        showShapeValue.setBounds(135, 21, 25, 15);
        frmjavaSwing.getContentPane().add(showShapeValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(15, 60, 558, 535);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_cth1 = new JSlider();
        slider_cth1.setValue(1);
        slider_cth1.setMinimum(1);
        slider_cth1.setMaximum(300);
        slider_cth1.setBounds(15, 21, 110, 25);
        frmjavaSwing.getContentPane().add(slider_cth1);

        JLabel lblBeta = new JLabel("Canny_threshold1");
        lblBeta.setBounds(15, 10, 110, 15);
        frmjavaSwing.getContentPane().add(lblBeta);

        slider_cth1.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showShapeValue.setText(slider_cth1.getValue() + "");
                BufferedImage newImage = matToBufferedImage(FindAndDrawContours(
                                             slider_cth1.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }


    public Mat FindAndDrawContours(double threshold1)
    {
        Mat source = Imgcodecs.imread("C://opencv3.1//samples//stuff.jpg",
                                      Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.GaussianBlur(source, source, new Size(3, 3), 10, 0);
        Imgproc.Canny(source, destination, threshold1, 100);
        List<MatOfPoint> contours = new ArrayList<MatOfPoint>();

        Mat hierarchy = new Mat(source.rows(), source.cols(), CvType.CV_8UC1,
                                new Scalar(0));
        Imgproc.findContours(destination, contours, hierarchy, Imgproc.RETR_TREE,
                             Imgproc.CHAIN_APPROX_SIMPLE);
        MatOfPoint2f center = new MatOfPoint2f();
        float[] radius = new float[1];
        Point pt = new Point();
        Mat drawing = Mat.zeros(destination.size(), CvType.CV_8UC3);
        Mat triangle = new Mat();
        Point[] allPoint = new Point[3];

        for (int i = 0; i < contours.size(); i++) {


            Imgproc.minEnclosingTriangle(contours.get(i), triangle);
            Imgproc.drawContours(drawing, contours, i, new Scalar(255, 0, 0, 255), 3);


            for (int ii = 0; ii < triangle.rows(); ii++) {
                Point tmpPoint = new Point(triangle.get(ii, 0)[0], triangle.get(ii, 0)[1]);
                allPoint[ii] = tmpPoint;
            }

            if (getTriangleArea(allPoint) > 500) {
                for (int ii = 0; ii < 3; ii++) {
                    Imgproc.line(drawing, allPoint[ii], allPoint[(ii + 1) % 3], new Scalar(0, 0,
                                 255), 2);
                }
            }
        }

        return drawing;
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
    // 海倫公式
    public static double getTriangleArea(Point[] allPoint)
    {

        Point pt0 = allPoint[0];
        Point pt1 = allPoint[1];
        Point pt2 = allPoint[2];

        double Line1 = getDistance(pt0, pt1);
        double Line2 = getDistance(pt1, pt2);
        double Line3 = getDistance(pt0, pt2);

        double halfPerimeter = (Line1 + Line2 + Line3) / 2;
        //

        return Math.sqrt(halfPerimeter * (halfPerimeter - Line1) *
                         (halfPerimeter - Line2) * (halfPerimeter - Line3));

    }

    public static double getDistance(Point a, Point b)
    {
        //return Math.round(Math.sqrt(Math.pow(a.x-b.x, 2)+Math.pow(a.y-b.y, 2)*100)/100);
        return Math.sqrt(Math.pow(a.x - b.x, 2) + Math.pow(a.y - b.y, 2));
    }

}

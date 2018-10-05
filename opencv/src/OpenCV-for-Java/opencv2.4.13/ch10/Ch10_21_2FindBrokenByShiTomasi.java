package ch10;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch10_21_2FindBrokenByShiTomasi
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
                    Ch10_21_2FindBrokenByShiTomasi window = new Ch10_21_2FindBrokenByShiTomasi();
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
    public Ch10_21_2FindBrokenByShiTomasi()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//IMAG0424.jpg");
        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 使用Shi-Tomasi角點檢測判斷斷裂方法");
        frmjavaSwing.setBounds(100, 100, 631, 461);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showKsizeValue = new JLabel("1");
        showKsizeValue.setBounds(132, 35, 46, 15);
        frmjavaSwing.getContentPane().add(showKsizeValue);

        final JLabel showShapeValue = new JLabel("1");
        showShapeValue.setBounds(273, 35, 25, 15);
        frmjavaSwing.getContentPane().add(showShapeValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 70, 603, 347);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_shape = new JSlider();
        slider_shape.setMinimum(1);
        slider_shape.setValue(1);
        slider_shape.setMaximum(500);
        slider_shape.setBounds(175, 35, 74, 25);
        frmjavaSwing.getContentPane().add(slider_shape);

        final JSlider slider_ksize = new JSlider();
        slider_ksize.setMinimum(1);
        slider_ksize.setMaximum(300);
        slider_ksize.setValue(1);

        slider_ksize.setBounds(10, 35, 112, 25);
        frmjavaSwing.getContentPane().add(slider_ksize);

        JLabel lblAlpha = new JLabel("threshold");
        lblAlpha.setBounds(69, 10, 66, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("minLineLength");
        lblBeta.setBounds(175, 10, 98, 15);
        frmjavaSwing.getContentPane().add(lblBeta);


        final JLabel showOpValue = new JLabel("1");
        showOpValue.setBounds(375, 35, 36, 15);
        frmjavaSwing.getContentPane().add(showOpValue);

        final JSlider slider_op = new JSlider();
        slider_op.setMinimum(1);

        slider_op.setValue(1);
        slider_op.setMaximum(500);
        slider_op.setBounds(308, 35, 57, 25);
        frmjavaSwing.getContentPane().add(slider_op);

        JLabel lblNewLabel_1 = new JLabel("maxLineGap");
        lblNewLabel_1.setBounds(308, 10, 74, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);
        slider_shape.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showShapeValue.setText(slider_shape.getValue() + "");
                BufferedImage newImage = matToBufferedImage(ShiTomasi(slider_ksize.getValue(),
                                         slider_shape.getValue(), slider_op.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_ksize.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showKsizeValue.setText(slider_ksize.getValue() + "");
                BufferedImage newImage = matToBufferedImage(ShiTomasi(slider_ksize.getValue(),
                                         slider_shape.getValue(), slider_op.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_op.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showOpValue.setText(slider_op.getValue() + "");
                BufferedImage newImage = matToBufferedImage(ShiTomasi(slider_ksize.getValue(),
                                         slider_shape.getValue(), slider_op.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat ShiTomasi(int maxCorners, double qualityLevel, double minDistance)
    {

        Mat source = Highgui.imread("C://opencv3.1//samples//IMAG0424.jpg",
                                    Highgui.CV_LOAD_IMAGE_COLOR);

        Mat HSVprocess = matConvertTo();

        Mat destination = source.clone();
        Imgproc.cvtColor(destination, destination, Imgproc.COLOR_BGR2GRAY);
        MatOfPoint corners = new MatOfPoint();
        //3,1,221 5,1,1
        Imgproc.goodFeaturesToTrack(HSVprocess, corners, maxCorners, 0.01, minDistance);
        MatOfPoint2f Pts = new MatOfPoint2f();
        corners.convertTo(Pts, CvType.CV_32FC2);


        Point[] AllPoint = Pts.toArray();
        System.out.println("Point=" + AllPoint.length);

        for (Point px : AllPoint) {

            Core.circle(source, px, 6, new Scalar(0, 0, 255));
            System.out.println("location=(" + px.x + "," + px.y + ")");
        }

        return source;
    }
    public Mat matConvertTo()
    {
        Mat source2 = Highgui.imread("C://opencv3.1//samples//IMAG0424.jpg");
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

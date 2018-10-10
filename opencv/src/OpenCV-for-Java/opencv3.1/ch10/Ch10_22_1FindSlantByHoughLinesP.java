package ch10;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch10_22_1FindSlantByHoughLinesP
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmjavaSwing;
    Mat BufferMat = new Mat();
    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {

        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch10_22_1FindSlantByHoughLinesP window = new Ch10_22_1FindSlantByHoughLinesP();
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
    public Ch10_22_1FindSlantByHoughLinesP()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//IMAG0423.jpg");
        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 使用霍夫圓線變換找出有歪斜線");
        frmjavaSwing.setBounds(100, 100, 1121, 402);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showKsizeValue = new JLabel("1");
        showKsizeValue.setBounds(132, 35, 46, 15);
        frmjavaSwing.getContentPane().add(showKsizeValue);

        final JLabel showShapeValue = new JLabel("1");
        showShapeValue.setBounds(273, 35, 25, 15);
        frmjavaSwing.getContentPane().add(showShapeValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 70, 532, 295);
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

        final JLabel label = new JLabel("");
        label.setBounds(550, 70, 468, 295);
        frmjavaSwing.getContentPane().add(label);
        slider_shape.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showShapeValue.setText(slider_shape.getValue() + "");
                BufferedImage newImage = matToBufferedImage(HoughLine(slider_ksize.getValue(),
                                         slider_shape.getValue(), slider_op.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
                BufferedImage bufImg = matToBufferedImage(getBufferMat());
                label.setIcon(new ImageIcon(bufImg));

            }
        });

        slider_ksize.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showKsizeValue.setText(slider_ksize.getValue() + "");
                BufferedImage newImage = matToBufferedImage(HoughLine(slider_ksize.getValue(),
                                         slider_shape.getValue(), slider_op.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
                BufferedImage bufImg = matToBufferedImage(getBufferMat());
                label.setIcon(new ImageIcon(bufImg));
            }
        });

        slider_op.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showOpValue.setText(slider_op.getValue() + "");
                BufferedImage newImage = matToBufferedImage(HoughLine(slider_ksize.getValue(),
                                         slider_shape.getValue(), slider_op.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
                BufferedImage bufImg = matToBufferedImage(getBufferMat());
                label.setIcon(new ImageIcon(bufImg));
            }
        });
    }

    public Mat getBufferMat()
    {
        return BufferMat;
    }

    public void setBufferMat(Mat bufferMat)
    {
        BufferMat = bufferMat;
    }


    public Mat HoughLine(int threshold, double minLineLength, double maxLineGap)
    {

        Mat src = Imgcodecs.imread("C://opencv3.1//samples//IMAG0423.jpg");
        Mat HSVprocess = matConvertTo();


        Mat destination = new Mat(src.rows(), src.cols(), src.type());
        Mat element = Imgproc.getStructuringElement(0, new Size(8, 8));
        Imgproc.erode(HSVprocess, destination, element);
        setBufferMat(destination);

        Mat lines = new Mat();
        Mat destination1 = new Mat(src.rows(), src.cols(), src.type());
        src.copyTo(destination1);
        Imgproc.HoughLinesP(destination, lines, 1, Math.PI / 180, threshold,
                            minLineLength, maxLineGap);
        System.out.println("line=" + lines.cols());

        for (int x = 0; x < lines.cols(); x++) {
            double[] vec = lines.get(0, x);
            double x1 = vec[0],
                   y1 = vec[1],
                   x2 = vec[2],
                   y2 = vec[3];
            Point start = new Point(x1, y1);
            Point end = new Point(x2, y2);

            //System.out.println("line"+x+":x=("+x1+","+y1+"),y=("+x2+","+y2+")");
            if (x1 != x2) {
                Point temp = new Point(x1, y2);
                System.out.println("line" + x + ",Angle:" + getAngle(start, end, temp));
                Imgproc.line(destination1, start, end, new Scalar(255, 0, 0), 1);
            }

            // Core.line(destination1, start, end, new Scalar(255,0,0), 1);
        }

        return destination1;
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
    public static double getDistance(Point a, Point b)
    {
        return Math.sqrt(Math.pow(a.x - b.x, 2) + Math.pow(a.y - b.y, 2));
    }

    public static double getAngle(Point a, Point b, Point c)
    {
        double line1 = Math.sqrt(Math.pow(a.x - b.x, 2)
                                 + Math.pow(a.y - b.y, 2));
        double line2 = Math.sqrt(Math.pow(c.x - b.x, 2)
                                 + Math.pow(c.y - b.y, 2));
        double dot = (a.x - b.x) * (c.x - b.x) + (a.y - b.y) * (c.y - b.y);
        double angle = Math.acos(dot / (line1 * line2));
        angle = angle * 180 / Math.PI;
        return Math.round(100 * angle) / 100;

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

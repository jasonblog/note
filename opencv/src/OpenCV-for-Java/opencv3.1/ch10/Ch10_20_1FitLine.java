package ch10;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import java.awt.EventQueue;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.List;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch10_20_1FitLine
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
                    Ch10_20_1FitLine window = new Ch10_20_1FitLine();
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
    public Ch10_20_1FitLine()
    {
        initialize();
        System.out.println("Imgproc.DIST_L1=" + Imgproc.DIST_L1);
        System.out.println("Imgproc.DIST_L2=" + Imgproc.DIST_L2);
        System.out.println("Imgproc.DIST_L12=" + Imgproc.DIST_L12);
        System.out.println("Imgproc.DIST_FAIR=" + Imgproc.DIST_FAIR);
        System.out.println("Imgproc.DIST_HUBER=" + Imgproc.DIST_HUBER);
        System.out.println("Imgproc.DIST_WELSCH=" + Imgproc.DIST_WELSCH);

    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source =
            Imgcodecs.imread("C://opencv3.1//samples//fitline_test1.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("");
        frmjavaSwing.setBounds(100, 100, 489, 442);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showShapeValue = new JLabel("1");
        showShapeValue.setBounds(135, 21, 25, 15);
        frmjavaSwing.getContentPane().add(showShapeValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(15, 60, 339, 304);
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

        JLabel lblDisttype = new JLabel("distType");
        lblDisttype.setBounds(203, 10, 110, 15);
        frmjavaSwing.getContentPane().add(lblDisttype);

        final JSlider slider_distType = new JSlider();
        slider_distType.setValue(1);
        slider_distType.setMaximum(7);
        slider_distType.setMinimum(1);

        slider_distType.setBounds(189, 21, 128, 25);
        frmjavaSwing.getContentPane().add(slider_distType);

        final JLabel Disttype_val = new JLabel("1");
        Disttype_val.setBounds(338, 21, 46, 15);
        frmjavaSwing.getContentPane().add(Disttype_val);

        slider_cth1.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showShapeValue.setText(slider_cth1.getValue() + "");
                BufferedImage newImage = matToBufferedImage(FindFitLine(slider_cth1.getValue(),
                                         slider_distType.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_distType.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent e) {
                if (slider_distType.getValue() == 3) {
                    slider_distType.setValue(4);
                }

                Disttype_val.setText(slider_distType.getValue() + "");
                BufferedImage newImage = matToBufferedImage(FindFitLine(slider_cth1.getValue(),
                                         slider_distType.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));

            }
        });
    }


    public Mat FindFitLine(double threshold1, int distType)
    {
        Mat source = Imgcodecs.imread("C://opencv3.1//samples//fitline_test1.jpg",
                                      Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        Mat srcClone = Imgcodecs.imread("C://opencv3.1//samples//fitline_test1.jpg");
        Mat line = new Mat();

        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.GaussianBlur(source, destination, new Size(3, 3), 10, 0);
        Imgproc.Canny(destination, destination, threshold1, 100);
        List<MatOfPoint> contours = new ArrayList<MatOfPoint>();

        Mat hierarchy = new Mat(source.rows(), source.cols(), CvType.CV_8UC1,
                                new Scalar(0));
        Imgproc.findContours(destination, contours, hierarchy, Imgproc.RETR_TREE,
                             Imgproc.CHAIN_APPROX_SIMPLE);
        Imgproc.fitLine(contours.get(0), line, distType, 0, 0.01, 0.01);
        // System.out.println("dump:"+line.size());//1x4
        double v0 = line.get(0, 0)[0];
        double v1 = line.get(1, 0)[0];
        double v2 = line.get(2, 0)[0];
        double v3 = line.get(3, 0)[0];

        //System.out.println(v0 + "," + v1 + "," + v2 + "," + v3);

        try {
            if (v0 != 0) {
                int left = (int)((-v2 * v1 / v0) + v3);
                int right = (int)(((source.cols() - v2) * v1 / v0) + v3);

                Imgproc.line(srcClone, new Point(source.cols() - 1, right), new Point(0, left),
                             new Scalar(255, 255, 50));
            }

        } catch (Exception e) {
        }

        return srcClone;
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

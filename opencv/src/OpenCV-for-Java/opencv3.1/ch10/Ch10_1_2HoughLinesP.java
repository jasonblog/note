package ch10;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch10_1_2HoughLinesP
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
                    Ch10_1_2HoughLinesP window = new Ch10_1_2HoughLinesP();
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
    public Ch10_1_2HoughLinesP()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//building.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv ÀN¤Ò½uÅÜ´«2");
        frmjavaSwing.setBounds(100, 100, 930, 720);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showKsizeValue = new JLabel("1");
        showKsizeValue.setBounds(155, 35, 46, 15);
        frmjavaSwing.getContentPane().add(showKsizeValue);

        final JLabel showShapeValue = new JLabel("1");
        showShapeValue.setBounds(309, 35, 25, 15);
        frmjavaSwing.getContentPane().add(showShapeValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 70, 874, 602);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_shape = new JSlider();
        slider_shape.setMinimum(1);
        slider_shape.setValue(1);
        slider_shape.setMaximum(300);
        slider_shape.setBounds(189, 35, 110, 25);
        frmjavaSwing.getContentPane().add(slider_shape);

        final JSlider slider_ksize = new JSlider();
        slider_ksize.setMinimum(1);
        slider_ksize.setMaximum(300);
        slider_ksize.setValue(1);

        slider_ksize.setBounds(10, 35, 146, 25);
        frmjavaSwing.getContentPane().add(slider_ksize);

        JLabel lblAlpha = new JLabel("threshold");
        lblAlpha.setBounds(69, 10, 66, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("minLineLength");
        lblBeta.setBounds(201, 10, 98, 15);
        frmjavaSwing.getContentPane().add(lblBeta);


        final JLabel showOpValue = new JLabel("1");
        showOpValue.setBounds(471, 35, 36, 15);
        frmjavaSwing.getContentPane().add(showOpValue);

        final JSlider slider_op = new JSlider();
        slider_op.setMinimum(1);

        slider_op.setValue(1);
        slider_op.setMaximum(500);
        slider_op.setBounds(332, 35, 129, 25);
        frmjavaSwing.getContentPane().add(slider_op);

        JLabel lblNewLabel_1 = new JLabel("maxLineGap");
        lblNewLabel_1.setBounds(361, 10, 74, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);

        JLabel lblNewLabel_2 = new JLabel("Canny-threshold1");
        lblNewLabel_2.setBounds(520, 10, 108, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_2);

        final JSlider slider_CT1 = new JSlider();

        slider_CT1.setMinimum(1);
        slider_CT1.setMaximum(400);
        slider_CT1.setBounds(506, 35, 122, 25);
        frmjavaSwing.getContentPane().add(slider_CT1);

        final JLabel Label_CT1 = new JLabel("1");
        Label_CT1.setBounds(638, 35, 46, 15);
        frmjavaSwing.getContentPane().add(Label_CT1);

        JLabel lblCannythreshold = new JLabel("Canny-threshold2");
        lblCannythreshold.setBounds(690, 10, 110, 15);
        frmjavaSwing.getContentPane().add(lblCannythreshold);

        final JSlider slider_CT2 = new JSlider();

        slider_CT2.setMinimum(1);
        slider_CT2.setMaximum(300);
        slider_CT2.setBounds(669, 35, 113, 25);
        frmjavaSwing.getContentPane().add(slider_CT2);

        final JLabel Label_CT2 = new JLabel("1");
        Label_CT2.setBounds(789, 35, 46, 15);
        frmjavaSwing.getContentPane().add(Label_CT2);

        slider_CT1.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                Label_CT1.setText(slider_CT1.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Canny(source, slider_CT1.getValue(),
                                         slider_CT2.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_CT2.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                Label_CT2.setText(slider_CT2.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Canny(source, slider_CT1.getValue(),
                                         slider_CT2.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_shape.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showShapeValue.setText(slider_shape.getValue() + "");
                BufferedImage newImage = matToBufferedImage(HoughLine(source,
                                         slider_ksize.getValue(), slider_shape.getValue(), slider_op.getValue(),
                                         slider_CT1.getValue(), slider_CT2.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_ksize.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showKsizeValue.setText(slider_ksize.getValue() + "");
                BufferedImage newImage = matToBufferedImage(HoughLine(source,
                                         slider_ksize.getValue(), slider_shape.getValue(), slider_op.getValue(),
                                         slider_CT1.getValue(), slider_CT2.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_op.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showOpValue.setText(slider_op.getValue() + "");
                BufferedImage newImage = matToBufferedImage(HoughLine(source,
                                         slider_ksize.getValue(), slider_shape.getValue(), slider_op.getValue(),
                                         slider_CT1.getValue(), slider_CT2.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat HoughLine(Mat source, int threshold, double minLineLength,
                         double maxLineGap, double Canny_threshold1, double Canny_threshold2)
    {
        Mat mRgba = new Mat();
        Mat thresholdImage = new Mat(source.size(), CvType.CV_8UC1);
        Imgproc.cvtColor(mRgba, thresholdImage, Imgproc.COLOR_GRAY2BGR, 0);
        Imgproc.Canny(source, thresholdImage, Canny_threshold1, Canny_threshold2);
        Mat lines = new Mat();
        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        source.copyTo(destination);
        Imgproc.HoughLinesP(thresholdImage, lines, 1, Math.PI / 180, threshold,
                            minLineLength, maxLineGap);

        for (int x = 0; x < lines.cols(); x++) {
            double[] vec = lines.get(0, x);
            double x1 = vec[0],
                   y1 = vec[1],
                   x2 = vec[2],
                   y2 = vec[3];
            Point start = new Point(x1, y1);
            Point end = new Point(x2, y2);
            //System.out.println("line"+x+":x=("+x1+","+y1+"),y=("+x2+","+y2+")");
            Imgproc.line(destination, start, end, new Scalar(255, 0, 0), 3);

        }

        return destination;
    }
    public Mat Canny(Mat source, double threshold1, double threshold2)
    {
        Mat mRgba = new Mat();
        Mat thresholdImage = new Mat(source.size(), CvType.CV_8UC1);
        Imgproc.cvtColor(mRgba, thresholdImage, Imgproc.COLOR_GRAY2BGR, 0);
        Imgproc.Canny(source, thresholdImage, threshold1, threshold2);

        return thresholdImage;
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

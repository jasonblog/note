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
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch10_2_1HoughCircle
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
                    Ch10_2_1HoughCircle window = new Ch10_2_1HoughCircle();
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
    public Ch10_2_1HoughCircle()
    {
        initialize();
        System.out.println("Imgproc.CV_HOUGH_GRADIENT=" + Imgproc.CV_HOUGH_GRADIENT);
        System.out.println("Imgproc.CV_HOUGH_MULTI_SCALE=" +
                           Imgproc.CV_HOUGH_MULTI_SCALE);
        System.out.println("Imgproc.CV_HOUGH_PROBABILISTIC=" +
                           Imgproc.CV_HOUGH_PROBABILISTIC);
        System.out.println("Imgproc.CV_HOUGH_STANDARD=" + Imgproc.CV_HOUGH_STANDARD);
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//coin.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 霍夫圓變換");
        frmjavaSwing.setBounds(100, 100, 815, 582);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showKsizeValue = new JLabel("9");
        showKsizeValue.setBounds(149, 35, 46, 15);
        frmjavaSwing.getContentPane().add(showKsizeValue);

        final JLabel showShapeValue = new JLabel("32");
        showShapeValue.setBounds(297, 35, 25, 15);
        frmjavaSwing.getContentPane().add(showShapeValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(15, 112, 649, 426);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_cth1 = new JSlider();
        slider_cth1.setValue(35);
        slider_cth1.setMinimum(1);
        slider_cth1.setMaximum(300);
        slider_cth1.setBounds(176, 35, 110, 25);
        frmjavaSwing.getContentPane().add(slider_cth1);

        final JSlider slider_GBK = new JSlider();
        slider_GBK.setMinimum(1);
        slider_GBK.setMaximum(30);
        slider_GBK.setValue(9);

        slider_GBK.setBounds(10, 35, 129, 25);
        frmjavaSwing.getContentPane().add(slider_GBK);

        JLabel lblAlpha = new JLabel("GaussianBlur_Ksize");
        lblAlpha.setBounds(10, 10, 146, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("Canny_threshold1");
        lblBeta.setBounds(188, 10, 110, 15);
        frmjavaSwing.getContentPane().add(lblBeta);


        final JLabel showOpValue = new JLabel("100");
        showOpValue.setBounds(471, 35, 36, 15);
        frmjavaSwing.getContentPane().add(showOpValue);

        final JSlider slider_param2 = new JSlider();
        slider_param2.setMinimum(1);

        slider_param2.setValue(100);
        slider_param2.setMaximum(500);
        slider_param2.setBounds(332, 35, 129, 25);
        frmjavaSwing.getContentPane().add(slider_param2);

        JLabel lblNewLabel_1 = new JLabel("param2");
        lblNewLabel_1.setBounds(361, 10, 74, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);

        JLabel lblNewLabel_2 = new JLabel("minRadius");
        lblNewLabel_2.setBounds(520, 10, 108, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_2);

        final JSlider slider_minR = new JSlider();
        slider_minR.setValue(0);
        slider_minR.setMaximum(400);
        slider_minR.setBounds(506, 35, 122, 25);
        frmjavaSwing.getContentPane().add(slider_minR);

        final JLabel Label_CT1 = new JLabel("0");
        Label_CT1.setBounds(638, 35, 46, 15);
        frmjavaSwing.getContentPane().add(Label_CT1);

        JLabel lblCannythreshold = new JLabel("maxRadius");
        lblCannythreshold.setBounds(690, 10, 110, 15);
        frmjavaSwing.getContentPane().add(lblCannythreshold);

        final JSlider slider_maxR = new JSlider();
        slider_maxR.setValue(0);
        slider_maxR.setMaximum(300);
        slider_maxR.setBounds(669, 35, 96, 25);
        frmjavaSwing.getContentPane().add(slider_maxR);

        final JLabel Label_CT2 = new JLabel("0");
        Label_CT2.setBounds(771, 35, 36, 15);
        frmjavaSwing.getContentPane().add(Label_CT2);

        JLabel lblMethod = new JLabel("method");
        lblMethod.setBounds(40, 70, 110, 15);
        frmjavaSwing.getContentPane().add(lblMethod);

        final JSlider slider_method = new JSlider();

        slider_method.setValue(3);
        slider_method.setMaximum(3);
        slider_method.setBounds(15, 81, 96, 25);
        frmjavaSwing.getContentPane().add(slider_method);

        final JLabel label_method = new JLabel("3");
        label_method.setBounds(121, 89, 36, 15);
        frmjavaSwing.getContentPane().add(label_method);

        JLabel lblDp = new JLabel("dp");
        lblDp.setBounds(186, 70, 25, 15);
        frmjavaSwing.getContentPane().add(lblDp);

        final JSlider slider_dp = new JSlider();

        slider_dp.setMinimum(1);
        slider_dp.setValue(1);
        slider_dp.setMaximum(20);
        slider_dp.setBounds(149, 81, 96, 25);
        frmjavaSwing.getContentPane().add(slider_dp);

        final JLabel label_dp = new JLabel("1");
        label_dp.setBounds(250, 89, 36, 15);
        frmjavaSwing.getContentPane().add(label_dp);

        JLabel lblMindist = new JLabel("minDist");
        lblMindist.setBounds(336, 70, 110, 15);
        frmjavaSwing.getContentPane().add(lblMindist);

        final JSlider slider_minDist = new JSlider();

        slider_minDist.setValue(3);
        slider_minDist.setMinimum(3);
        slider_minDist.setMaximum(20);
        slider_minDist.setBounds(308, 81, 96, 25);
        frmjavaSwing.getContentPane().add(slider_minDist);

        final JLabel lblImageheight = new JLabel("image.height()/3");
        lblImageheight.setBounds(414, 81, 130, 15);
        frmjavaSwing.getContentPane().add(lblImageheight);

        final JSlider slider_param1 = new JSlider();

        slider_param1.setValue(250);
        slider_param1.setMinimum(1);
        slider_param1.setMaximum(500);
        slider_param1.setBounds(516, 78, 129, 25);
        frmjavaSwing.getContentPane().add(slider_param1);

        final JLabel label_param1 = new JLabel("250");
        label_param1.setBounds(655, 78, 62, 15);
        frmjavaSwing.getContentPane().add(label_param1);

        JLabel lblParam = new JLabel("param1");
        lblParam.setBounds(564, 60, 74, 15);
        frmjavaSwing.getContentPane().add(lblParam);

        slider_minR.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                Label_CT1.setText(slider_minR.getValue() + "");
                BufferedImage newImage = matToBufferedImage(HoughCircle(slider_GBK.getValue(),
                                         slider_cth1.getValue(), slider_method.getValue(), slider_dp.getValue(),
                                         slider_minDist.getValue(), slider_param1.getValue(), slider_param2.getValue(),
                                         slider_minR.getValue(), slider_maxR.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_maxR.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                Label_CT2.setText(slider_maxR.getValue() + "");
                BufferedImage newImage = matToBufferedImage(HoughCircle(slider_GBK.getValue(),
                                         slider_cth1.getValue(), slider_method.getValue(), slider_dp.getValue(),
                                         slider_minDist.getValue(), slider_param1.getValue(), slider_param2.getValue(),
                                         slider_minR.getValue(), slider_maxR.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_cth1.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showShapeValue.setText(slider_cth1.getValue() + "");
                BufferedImage newImage = matToBufferedImage(HoughCircle(slider_GBK.getValue(),
                                         slider_cth1.getValue(), slider_method.getValue(), slider_dp.getValue(),
                                         slider_minDist.getValue(), slider_param1.getValue(), slider_param2.getValue(),
                                         slider_minR.getValue(), slider_maxR.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_GBK.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                if (slider_GBK.getValue() % 2 == 0) {
                    slider_GBK.setValue(slider_GBK.getValue() + 1);
                }

                showKsizeValue.setText(slider_GBK.getValue() + "");
                BufferedImage newImage = matToBufferedImage(HoughCircle(slider_GBK.getValue(),
                                         slider_cth1.getValue(), slider_method.getValue(), slider_dp.getValue(),
                                         slider_minDist.getValue(), slider_param1.getValue(), slider_param2.getValue(),
                                         slider_minR.getValue(), slider_maxR.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_param2.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showOpValue.setText(slider_param2.getValue() + "");
                BufferedImage newImage = matToBufferedImage(HoughCircle(slider_GBK.getValue(),
                                         slider_cth1.getValue(), slider_method.getValue(), slider_dp.getValue(),
                                         slider_minDist.getValue(), slider_param1.getValue(), slider_param2.getValue(),
                                         slider_minR.getValue(), slider_maxR.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_method.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                label_method.setText(slider_method.getValue() + "");
            }
        });
        slider_dp.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                label_dp.setText(slider_dp.getValue() + "");
            }
        });

        slider_minDist.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                lblImageheight.setText("image.height()/" + slider_minDist.getValue());
                BufferedImage newImage = matToBufferedImage(HoughCircle(slider_GBK.getValue(),
                                         slider_cth1.getValue(), slider_method.getValue(), slider_dp.getValue(),
                                         slider_minDist.getValue(), slider_param1.getValue(), slider_param2.getValue(),
                                         slider_minR.getValue(), slider_maxR.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_param1.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                label_param1.setText(slider_param1.getValue() + "");
                BufferedImage newImage = matToBufferedImage(HoughCircle(slider_GBK.getValue(),
                                         slider_cth1.getValue(), slider_method.getValue(), slider_dp.getValue(),
                                         slider_minDist.getValue(), slider_param1.getValue(), slider_param2.getValue(),
                                         slider_minR.getValue(), slider_maxR.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));

            }
        });

    }
    public Mat HoughCircle(double GBK, double threshold1,  int method, double dp,
                           double minDist, double param1, double param2, int minRadius, int maxRadius)
    {
        Mat source1 = Imgcodecs.imread("C://opencv3.1//samples//coin.jpg");
        Mat source = Imgcodecs.imread("C://opencv3.1//samples//coin.jpg",
                                      Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.GaussianBlur(source, source, new Size(GBK, GBK), 20, 0);
        Imgproc.Canny(source, destination, threshold1, 100);
        Mat circles = new Mat();
        Imgproc.HoughCircles(destination, circles, method, dp,
                             destination.height() / minDist, param1, param2, minRadius, maxRadius);
        Imgproc.cvtColor(destination, destination, Imgproc.COLOR_GRAY2BGR);

        //畫出所有圓
        if (circles.cols() > 0) {

            for (int i = 0; i < circles.cols(); i++) {
                double vCircle[] = circles.get(0, i);
                Point center = new Point(Math.round(vCircle[0]), Math.round(vCircle[1]));
                int radius = (int)Math.round(vCircle[2]);

                //draw the circle center
                Imgproc.circle(source1, center, 3, new Scalar(255, 0, 0), 3, 8, 0);

                //draw the circle outline
                Imgproc.circle(source1, center, radius, new Scalar(255, 0, 255), 4, 8, 0);
            }

        }

        return source1;
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

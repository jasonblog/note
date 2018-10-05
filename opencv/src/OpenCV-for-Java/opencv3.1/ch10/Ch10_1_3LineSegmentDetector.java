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

public class Ch10_1_3LineSegmentDetector
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
                    Ch10_1_3LineSegmentDetector window = new Ch10_1_3LineSegmentDetector();
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
    public Ch10_1_3LineSegmentDetector()
    {
        initialize();
        System.out.println("Imgproc.LSD_REFINE_STD=" + Imgproc.LSD_REFINE_STD);
        System.out.println("Imgproc.LSD_REFINE_ADV=" + Imgproc.LSD_REFINE_ADV);
        System.out.println("Imgproc.LSD_REFINE_NONE=" + Imgproc.LSD_REFINE_NONE);
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//DSC_0763.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv LineSegmentDetector");
        frmjavaSwing.setBounds(100, 100, 1016, 460);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showRefineValue = new JLabel("0");
        showRefineValue.setBounds(76, 35, 46, 15);
        frmjavaSwing.getContentPane().add(showRefineValue);

        final JLabel showScaleValue = new JLabel("0.8");
        showScaleValue.setBounds(207, 35, 25, 15);
        frmjavaSwing.getContentPane().add(showScaleValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 88, 480, 320);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_scale = new JSlider();
        slider_scale.setMinimum(1);
        slider_scale.setValue(8);
        slider_scale.setMaximum(30);
        slider_scale.setBounds(99, 35, 98, 25);
        frmjavaSwing.getContentPane().add(slider_scale);

        final JSlider slider_refine = new JSlider();
        slider_refine.setMaximum(2);
        slider_refine.setValue(0);

        slider_refine.setBounds(10, 35, 66, 25);
        frmjavaSwing.getContentPane().add(slider_refine);

        JLabel lblAlpha = new JLabel("refine");
        lblAlpha.setBounds(10, 10, 66, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("scale");
        lblBeta.setBounds(123, 10, 98, 15);
        frmjavaSwing.getContentPane().add(lblBeta);


        final JLabel showSSValue = new JLabel("0.6");
        showSSValue.setBounds(330, 35, 36, 15);
        frmjavaSwing.getContentPane().add(showSSValue);

        final JSlider slider_sigma_scale = new JSlider();
        slider_sigma_scale.setMinimum(1);

        slider_sigma_scale.setValue(6);
        slider_sigma_scale.setMaximum(30);
        slider_sigma_scale.setBounds(242, 35, 78, 25);
        frmjavaSwing.getContentPane().add(slider_sigma_scale);

        JLabel lblNewLabel_1 = new JLabel("sigma_scale");
        lblNewLabel_1.setBounds(254, 10, 83, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);

        JLabel lblNewLabel_2 = new JLabel("quant");
        lblNewLabel_2.setBounds(361, 10, 108, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_2);

        final JSlider slider_quant = new JSlider();
        slider_quant.setValue(20);

        slider_quant.setMinimum(1);
        slider_quant.setMaximum(40);
        slider_quant.setBounds(351, 35, 78, 25);
        frmjavaSwing.getContentPane().add(slider_quant);

        final JLabel Label_quant = new JLabel("2.0");
        Label_quant.setBounds(441, 35, 46, 15);
        frmjavaSwing.getContentPane().add(Label_quant);

        JLabel lblCannythreshold = new JLabel("ang_th");
        lblCannythreshold.setBounds(479, 10, 110, 15);
        frmjavaSwing.getContentPane().add(lblCannythreshold);

        final JSlider slider_AngTh = new JSlider();
        slider_AngTh.setValue(225);

        slider_AngTh.setMinimum(1);
        slider_AngTh.setMaximum(400);
        slider_AngTh.setBounds(472, 35, 66, 25);
        frmjavaSwing.getContentPane().add(slider_AngTh);

        final JLabel Label_AngTh = new JLabel("22.5");
        Label_AngTh.setBounds(548, 35, 46, 15);
        frmjavaSwing.getContentPane().add(Label_AngTh);

        JLabel lblLogeps = new JLabel("log_eps");
        lblLogeps.setBounds(582, 10, 66, 15);
        frmjavaSwing.getContentPane().add(lblLogeps);

        final JSlider slider_log_eps = new JSlider();
        slider_log_eps.setValue(0);
        slider_log_eps.setMaximum(10);
        slider_log_eps.setBounds(568, 35, 66, 25);
        frmjavaSwing.getContentPane().add(slider_log_eps);

        final JLabel label_log_eps = new JLabel("0");
        label_log_eps.setBounds(644, 35, 46, 15);
        frmjavaSwing.getContentPane().add(label_log_eps);

        JLabel lblDensityth = new JLabel("density_th");
        lblDensityth.setBounds(687, 10, 66, 15);
        frmjavaSwing.getContentPane().add(lblDensityth);

        final JSlider slider_density_th = new JSlider();
        slider_density_th.setValue(7);

        slider_density_th.setMinimum(1);
        slider_density_th.setMaximum(30);
        slider_density_th.setBounds(673, 35, 66, 25);
        frmjavaSwing.getContentPane().add(slider_density_th);

        final JLabel label_density_th = new JLabel("0.7");
        label_density_th.setBounds(754, 35, 46, 15);
        frmjavaSwing.getContentPane().add(label_density_th);

        JLabel lblNbits = new JLabel("n_bits");
        lblNbits.setBounds(818, 10, 66, 15);
        frmjavaSwing.getContentPane().add(lblNbits);

        final JSlider slider_n_bits = new JSlider();
        slider_n_bits.setValue(1024);

        slider_n_bits.setMinimum(1);
        slider_n_bits.setMaximum(2000);
        slider_n_bits.setBounds(799, 35, 66, 25);
        frmjavaSwing.getContentPane().add(slider_n_bits);

        final JLabel label_n_bits = new JLabel("1024");
        label_n_bits.setBounds(875, 35, 46, 15);
        frmjavaSwing.getContentPane().add(label_n_bits);

        JLabel label = new JLabel("");
        label.setBounds(514, 88, 480, 320);
        label.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(label);

        slider_quant.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                Label_quant.setText((float)slider_quant.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(LineSegmentDetector(
                                             slider_refine.getValue(), (float)slider_scale.getValue() / 10,
                                             (float)slider_sigma_scale.getValue() / 10, (float)slider_quant.getValue() / 10,
                                             (float)slider_AngTh.getValue() / 10, slider_log_eps.getValue(),
                                             (float)slider_density_th.getValue() / 10, slider_n_bits.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_AngTh.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                Label_AngTh.setText((float)slider_AngTh.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(LineSegmentDetector(
                                             slider_refine.getValue(), (float)slider_scale.getValue() / 10,
                                             (float)slider_sigma_scale.getValue() / 10, (float)slider_quant.getValue() / 10,
                                             (float)slider_AngTh.getValue() / 10, slider_log_eps.getValue(),
                                             (float)slider_density_th.getValue() / 10, slider_n_bits.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_scale.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showScaleValue.setText((float)slider_scale.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(LineSegmentDetector(
                                             slider_refine.getValue(), (float)slider_scale.getValue() / 10,
                                             (float)slider_sigma_scale.getValue() / 10, (float)slider_quant.getValue() / 10,
                                             (float)slider_AngTh.getValue() / 10, slider_log_eps.getValue(),
                                             (float)slider_density_th.getValue() / 10, slider_n_bits.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_refine.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showRefineValue.setText(slider_refine.getValue() + "");
                BufferedImage newImage = matToBufferedImage(LineSegmentDetector(
                                             slider_refine.getValue(), (float)slider_scale.getValue() / 10,
                                             (float)slider_sigma_scale.getValue() / 10, (float)slider_quant.getValue() / 10,
                                             (float)slider_AngTh.getValue() / 10, slider_log_eps.getValue(),
                                             (float)slider_density_th.getValue() / 10, slider_n_bits.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_sigma_scale.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showSSValue.setText((float)slider_sigma_scale.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(LineSegmentDetector(
                                             slider_refine.getValue(), (float)slider_scale.getValue() / 10,
                                             (float)slider_sigma_scale.getValue() / 10, (float)slider_quant.getValue() / 10,
                                             (float)slider_AngTh.getValue() / 10, slider_log_eps.getValue(),
                                             (float)slider_density_th.getValue() / 10, slider_n_bits.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });


        slider_log_eps.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                label_log_eps.setText(slider_log_eps.getValue() + "");
                BufferedImage newImage = matToBufferedImage(LineSegmentDetector(
                                             slider_refine.getValue(), (float)slider_scale.getValue() / 10,
                                             (float)slider_sigma_scale.getValue() / 10, (float)slider_quant.getValue() / 10,
                                             (float)slider_AngTh.getValue() / 10, slider_log_eps.getValue(),
                                             (float)slider_density_th.getValue() / 10, slider_n_bits.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_density_th.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                label_density_th.setText((float)slider_density_th.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(LineSegmentDetector(
                                             slider_refine.getValue(), (float)slider_scale.getValue() / 10,
                                             (float)slider_sigma_scale.getValue() / 10, (float)slider_quant.getValue() / 10,
                                             (float)slider_AngTh.getValue() / 10, slider_log_eps.getValue(),
                                             (float)slider_density_th.getValue() / 10, slider_n_bits.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });


        slider_n_bits.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                label_n_bits.setText(slider_n_bits.getValue() + "");
                BufferedImage newImage = matToBufferedImage(LineSegmentDetector(
                                             slider_refine.getValue(), (float)slider_scale.getValue() / 10,
                                             (float)slider_sigma_scale.getValue() / 10, (float)slider_quant.getValue() / 10,
                                             (float)slider_AngTh.getValue() / 10, slider_log_eps.getValue(),
                                             (float)slider_density_th.getValue() / 10, slider_n_bits.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat LineSegmentDetector(int refine, float scale, float sigma_scale,
                                   float quant, float ang_th, int log_eps, float density_th, int n_bits)
    {
        Mat source = Imgcodecs.imread("C://opencv3.1//samples//DSC_0763.jpg",
                                      Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        Mat Line = new Mat();
        Mat draw = new Mat(source.size(), source.type());
        org.opencv.imgproc.LineSegmentDetector lsd = new
        org.opencv.imgproc.Imgproc().createLineSegmentDetector(refine, scale,
                sigma_scale, quant, ang_th, log_eps,  density_th,  n_bits);
        lsd.detect(source, Line);
        //System.out.println(Line.size());
        lsd.drawSegments(draw, Line);
        return draw;
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

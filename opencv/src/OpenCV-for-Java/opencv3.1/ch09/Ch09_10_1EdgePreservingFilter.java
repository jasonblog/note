package ch09;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.photo.Photo;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch09_10_1EdgePreservingFilter
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
                    Ch09_10_1EdgePreservingFilter window = new Ch09_10_1EdgePreservingFilter();
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
    public Ch09_10_1EdgePreservingFilter()
    {
        initialize();
        System.out.println("Photo.RECURS_FILTER=" + Photo.RECURS_FILTER);
        System.out.println("Photo.NORMCONV_FILTER =" + Photo.NORMCONV_FILTER);

    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv edgePreservingFilter³B²z½m²ß");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showFlagsValue = new JLabel("1");
        showFlagsValue.setBounds(84, 32, 46, 15);
        frmjavaSwing.getContentPane().add(showFlagsValue);

        final JLabel showSigmaSValue = new JLabel("1");
        showSigmaSValue.setBounds(256, 32, 46, 15);
        frmjavaSwing.getContentPane().add(showSigmaSValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_sigmaS = new JSlider();
        slider_sigmaS.setMinimum(1);
        slider_sigmaS.setValue(1);
        slider_sigmaS.setMaximum(500);
        slider_sigmaS.setBounds(220, 10, 75, 25);
        frmjavaSwing.getContentPane().add(slider_sigmaS);

        final JSlider slider_flags = new JSlider();
        slider_flags.setMinimum(1);
        slider_flags.setMaximum(2);
        slider_flags.setValue(1);

        slider_flags.setBounds(57, 10, 86, 25);
        frmjavaSwing.getContentPane().add(slider_flags);

        JLabel lblAlpha = new JLabel("flags");
        lblAlpha.setBounds(10, 10, 56, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblsigma_s = new JLabel("sigma_s");
        lblsigma_s.setBounds(155, 10, 67, 15);
        frmjavaSwing.getContentPane().add(lblsigma_s);

        JLabel lblNewLabel_1 = new JLabel("sigma_r");
        lblNewLabel_1.setBounds(315, 10, 85, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);

        final JSlider slider_sigmaR = new JSlider();

        slider_sigmaR.setValue(1);
        slider_sigmaR.setMinimum(1);
        slider_sigmaR.setMaximum(20);
        slider_sigmaR.setBounds(373, 8, 75, 25);
        frmjavaSwing.getContentPane().add(slider_sigmaR);

        final JLabel showSigmaRValue = new JLabel("1");
        showSigmaRValue.setBounds(417, 32, 46, 15);
        frmjavaSwing.getContentPane().add(showSigmaRValue);

        slider_sigmaR.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showSigmaRValue.setText((float)slider_sigmaR.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(EdgePreservingFilter(
                                             slider_flags.getValue(), slider_sigmaS.getValue(),
                                             (float)slider_sigmaR.getValue() / 10));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_flags.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showFlagsValue.setText(slider_flags.getValue() + "");
                BufferedImage newImage = matToBufferedImage(EdgePreservingFilter(
                                             slider_flags.getValue(), slider_sigmaS.getValue(),
                                             (float)slider_sigmaR.getValue() / 10));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_sigmaS.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showSigmaSValue.setText(slider_sigmaS.getValue() + "");
                BufferedImage newImage = matToBufferedImage(EdgePreservingFilter(
                                             slider_flags.getValue(), slider_sigmaS.getValue(),
                                             (float)slider_sigmaR.getValue() / 10));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat EdgePreservingFilter(int flags, float sigma_s, float sigma_r)
    {
        Mat src = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg",
                                   Imgcodecs.CV_LOAD_IMAGE_COLOR);
        Mat destination = new Mat();
        Photo.edgePreservingFilter(src, destination, flags, sigma_s, sigma_r);
        return destination;

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

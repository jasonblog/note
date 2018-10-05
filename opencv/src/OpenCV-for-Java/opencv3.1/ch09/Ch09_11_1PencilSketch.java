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

public class Ch09_11_1PencilSketch
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
                    Ch09_11_1PencilSketch window = new Ch09_11_1PencilSketch();
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
    public Ch09_11_1PencilSketch()
    {
        initialize();

    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//DSC_0704.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv pencilSketch ³B²z½m²ß");
        frmjavaSwing.setBounds(100, 100, 703, 601);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showShadeFactorValue = new JLabel("0.01");
        showShadeFactorValue.setBounds(83, 53, 46, 15);
        frmjavaSwing.getContentPane().add(showShadeFactorValue);

        final JLabel showSigmaSValue = new JLabel("1");
        showSigmaSValue.setBounds(258, 43, 46, 15);
        frmjavaSwing.getContentPane().add(showSigmaSValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 322, 474);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_sigmaS = new JSlider();
        slider_sigmaS.setMinimum(1);
        slider_sigmaS.setValue(1);
        slider_sigmaS.setMaximum(200);
        slider_sigmaS.setBounds(229, 10, 75, 25);
        frmjavaSwing.getContentPane().add(slider_sigmaS);

        final JSlider slider_shadeFactor = new JSlider();
        slider_shadeFactor.setMinimum(1);
        slider_shadeFactor.setMaximum(17);
        slider_shadeFactor.setValue(1);

        slider_shadeFactor.setBounds(46, 22, 86, 25);
        frmjavaSwing.getContentPane().add(slider_shadeFactor);

        JLabel lblAlpha = new JLabel("shade_factor ");
        lblAlpha.setBounds(51, 10, 95, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblsigma_s = new JLabel("sigma_s");
        lblsigma_s.setBounds(171, 10, 67, 15);
        frmjavaSwing.getContentPane().add(lblsigma_s);

        JLabel lblNewLabel_1 = new JLabel("sigma_r");
        lblNewLabel_1.setBounds(363, 10, 85, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);

        final JSlider slider_sigmaR = new JSlider();

        slider_sigmaR.setValue(1);
        slider_sigmaR.setMinimum(1);
        slider_sigmaR.setMaximum(30);
        slider_sigmaR.setBounds(415, 10, 75, 25);
        frmjavaSwing.getContentPane().add(slider_sigmaR);

        final JLabel showSigmaRValue = new JLabel("1");
        showSigmaRValue.setBounds(438, 43, 46, 15);
        frmjavaSwing.getContentPane().add(showSigmaRValue);

        final JLabel label = new JLabel("");
        label.setBounds(363, 68, 322, 474);
        frmjavaSwing.getContentPane().add(label);

        slider_sigmaR.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showSigmaRValue.setText((float)slider_sigmaR.getValue() / 10 + "");

                Mat[] temp = new Mat[2];
                temp = (PencilSketch(slider_sigmaS.getValue(),
                                     (float)slider_sigmaR.getValue() / 10,
                                     (float)slider_shadeFactor.getValue() / 100));
                BufferedImage tmp1 = matToBufferedImage(temp[0]);
                BufferedImage tmp2 = matToBufferedImage(temp[1]);
                lblNewLabel.setIcon(new ImageIcon(tmp1));
                label.setIcon(new ImageIcon(tmp2));
            }
        });
        slider_shadeFactor.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showShadeFactorValue.setText((float)slider_shadeFactor.getValue() / 100 + "");
                Mat[] temp = new Mat[2];
                temp = (PencilSketch(slider_sigmaS.getValue(),
                                     (float)slider_sigmaR.getValue() / 10,
                                     (float)slider_shadeFactor.getValue() / 100));
                BufferedImage tmp1 = matToBufferedImage(temp[0]);
                BufferedImage tmp2 = matToBufferedImage(temp[1]);
                lblNewLabel.setIcon(new ImageIcon(tmp1));
                label.setIcon(new ImageIcon(tmp2));
            }
        });

        slider_sigmaS.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showSigmaSValue.setText(slider_sigmaS.getValue() + "");
                Mat[] temp = new Mat[2];
                temp = (PencilSketch(slider_sigmaS.getValue(),
                                     (float)slider_sigmaR.getValue() / 10,
                                     (float)slider_shadeFactor.getValue() / 100));
                BufferedImage tmp1 = matToBufferedImage(temp[0]);
                BufferedImage tmp2 = matToBufferedImage(temp[1]);
                lblNewLabel.setIcon(new ImageIcon(tmp1));
                label.setIcon(new ImageIcon(tmp2));
            }
        });
    }
    public Mat[] PencilSketch(float sigma_s, float sigma_r, float shade_factor)
    {
        Mat[] output = new Mat[2];
        Mat src = Imgcodecs.imread("C://opencv3.1//samples//DSC_0704.jpg",
                                   Imgcodecs.CV_LOAD_IMAGE_COLOR);
        Mat dst2 = new Mat();
        Mat dst1 = new Mat();
        Photo.pencilSketch(src, dst1, dst2, sigma_s, sigma_r, shade_factor);
        output[0] = dst1;
        output[1] = dst2;
        return output;
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

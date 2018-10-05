package ch05;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch05_12_1SalonUseBlurAddWeighted
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
                    Ch05_12_1SalonUseBlurAddWeighted window = new
                    Ch05_12_1SalonUseBlurAddWeighted();
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
    public Ch05_12_1SalonUseBlurAddWeighted()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source1 = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");
        final Mat source2 = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");

        BufferedImage image = matToBufferedImage(BlurAndmerge(source1, source2, 1, 1, 1,
                              1));

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 夢幻沙龍處理練習");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showAlphaValue = new JLabel("0");
        showAlphaValue.setBounds(260, 10, 29, 15);
        frmjavaSwing.getContentPane().add(showAlphaValue);

        final JLabel showBetaValue = new JLabel("0");
        showBetaValue.setBounds(259, 35, 29, 15);
        frmjavaSwing.getContentPane().add(showBetaValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_Blur = new JSlider();
        slider_Blur.setValue(0);
        slider_Blur.setMaximum(101);
        slider_Blur.setBounds(57, 35, 200, 25);
        frmjavaSwing.getContentPane().add(slider_Blur);

        final JSlider slider_Gamma = new JSlider();
        slider_Gamma.setMaximum(200);
        slider_Gamma.setValue(0);
        slider_Gamma.setBounds(290, 35, 115, 25);
        frmjavaSwing.getContentPane().add(slider_Gamma);


        final JSlider slider_Alpha = new JSlider();
        slider_Alpha.setMaximum(20);
        slider_Alpha.setValue(0);
        slider_Alpha.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_Alpha.getValue()/10);
                showAlphaValue.setText((float)slider_Alpha.getValue() / 10 + "");

                if (slider_Blur.getValue() % 2 == 0) {
                    slider_Blur.setValue(slider_Blur.getValue() + 1);
                }

                BufferedImage newImage = matToBufferedImage(BlurAndmerge(source1, source2,
                                         (float)slider_Alpha.getValue() / 10, 1, -slider_Gamma.getValue(),
                                         slider_Blur.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_Alpha.setBounds(57, 10, 200, 25);
        frmjavaSwing.getContentPane().add(slider_Alpha);

        JLabel lblAlpha = new JLabel("透明度");
        lblAlpha.setBounds(10, 20, 46, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBetaval = new JLabel("模糊度");
        lblBetaval.setBounds(10, 45, 46, 15);
        frmjavaSwing.getContentPane().add(lblBetaval);

        JLabel lblGamma = new JLabel("明亮度");
        lblGamma.setBounds(290, 10, 46, 15);
        frmjavaSwing.getContentPane().add(lblGamma);

        final JLabel showTypeValue = new JLabel("0");
        showTypeValue.setBounds(415, 35, 36, 15);
        frmjavaSwing.getContentPane().add(showTypeValue);

        slider_Gamma.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showTypeValue.setText(slider_Gamma.getValue() + "");
                BufferedImage newImage = matToBufferedImage(BlurAndmerge(source1, source2,
                                         (float)slider_Alpha.getValue() / 10, 1, -slider_Gamma.getValue(),
                                         slider_Blur.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));

            }
        });

        slider_Blur.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                if (slider_Blur.getValue() % 2 == 0) {
                    slider_Blur.setValue(slider_Blur.getValue() + 1);
                }

                showBetaValue.setText(slider_Blur.getValue() + "");
                BufferedImage newImage = matToBufferedImage(BlurAndmerge(source1, source2,
                                         (float)slider_Alpha.getValue() / 10, 1, -slider_Gamma.getValue(),
                                         slider_Blur.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat BlurAndmerge(Mat source1, Mat source2, double alpha, double beta,
                            double gamma, int GaussianKernelSize)
    {
        Mat processBlur = new Mat(source1.rows(), source1.cols(), source1.type());
        Mat destination = new Mat(source1.rows(), source1.cols(), source1.type());
        Imgproc.GaussianBlur(source1, processBlur, new Size(GaussianKernelSize,
                             GaussianKernelSize), 0, 0);
        Core.addWeighted(processBlur, alpha, source2, beta, gamma, destination);
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

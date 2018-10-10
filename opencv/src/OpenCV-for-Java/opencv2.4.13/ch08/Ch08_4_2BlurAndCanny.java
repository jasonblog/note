package ch08;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch08_4_2BlurAndCanny
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
                    Ch08_4_2BlurAndCanny window = new Ch08_4_2BlurAndCanny();
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
    public Ch08_4_2BlurAndCanny()
    {
        initialize();

    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//lena.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv Canny√‰Ωt¿À¥˙Ωm≤ﬂ2");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showKsizeValue = new JLabel("0");
        showKsizeValue.setBounds(235, 33, 31, 15);
        frmjavaSwing.getContentPane().add(showKsizeValue);

        final JLabel showShapeValue = new JLabel("0");
        showShapeValue.setBounds(466, 33, 46, 15);
        frmjavaSwing.getContentPane().add(showShapeValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(8, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_th2 = new JSlider();
        slider_th2.setValue(0);
        slider_th2.setMaximum(900);
        slider_th2.setBounds(330, 33, 137, 25);
        frmjavaSwing.getContentPane().add(slider_th2);

        final JSlider slider_ksize = new JSlider();
        slider_ksize.setValue(1);
        slider_ksize.setMinimum(1);
        slider_ksize.setMaximum(203);
        slider_ksize.setBounds(152, 0, 200, 25);
        frmjavaSwing.getContentPane().add(slider_ksize);


        final JSlider slider_th1 = new JSlider();
        slider_th1.setMaximum(900);
        slider_th1.setValue(0);
        slider_th1.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showKsizeValue.setText(slider_th1.getValue() + "");
                BufferedImage newImage = matToBufferedImage(BlurAndCanny(source,
                                         slider_th1.getValue(), slider_th2.getValue(), slider_ksize.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_th1.setBounds(72, 33, 164, 25);
        frmjavaSwing.getContentPane().add(slider_th1);

        JLabel lblAlpha = new JLabel("threshold1");
        lblAlpha.setBounds(8, 33, 64, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("threshold2");
        lblBeta.setBounds(264, 33, 64, 15);
        frmjavaSwing.getContentPane().add(lblBeta);

        JLabel label = new JLabel("Gaussian kernel size:");
        label.setBounds(10, 10, 144, 15);
        frmjavaSwing.getContentPane().add(label);

        final JLabel showKernalValue = new JLabel("1");
        showKernalValue.setBounds(362, 8, 46, 15);
        frmjavaSwing.getContentPane().add(showKernalValue);


        slider_ksize.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                if (slider_ksize.getValue() % 2 == 0) {
                    slider_ksize.setValue(slider_ksize.getValue() + 1);
                }

                showKernalValue.setText(slider_ksize.getValue() + "");
                BufferedImage newImage = matToBufferedImage(BlurAndCanny(source,
                                         slider_th1.getValue(), slider_th2.getValue(), slider_ksize.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_th2.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showShapeValue.setText(slider_th2.getValue() + "");
                BufferedImage newImage = matToBufferedImage(BlurAndCanny(source,
                                         slider_th1.getValue(), slider_th2.getValue(), slider_ksize.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat BlurAndCanny(Mat source, double threshold1, double threshold2,
                            int GaussianKernelSize)
    {
        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.GaussianBlur(source, destination, new Size(GaussianKernelSize,
                             GaussianKernelSize), 0, 0);
        Imgproc.Canny(destination, destination, threshold1, threshold2);
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

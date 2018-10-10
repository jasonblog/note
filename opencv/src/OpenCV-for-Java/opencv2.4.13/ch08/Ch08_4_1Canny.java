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

public class Ch08_4_1Canny
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
                    Ch08_4_1Canny window = new Ch08_4_1Canny();
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
    public Ch08_4_1Canny()
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
        frmjavaSwing.setTitle("opencv Canny√‰Ωt¿À¥˙Ωm≤ﬂ");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showKsizeValue = new JLabel("0");
        showKsizeValue.setBounds(235, 10, 31, 15);
        frmjavaSwing.getContentPane().add(showKsizeValue);

        final JLabel showShapeValue = new JLabel("0");
        showShapeValue.setBounds(466, 10, 46, 15);
        frmjavaSwing.getContentPane().add(showShapeValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(8, 45, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_th2 = new JSlider();
        slider_th2.setValue(0);
        slider_th2.setMaximum(900);
        slider_th2.setBounds(330, 10, 137, 25);
        frmjavaSwing.getContentPane().add(slider_th2);

        final JSlider slider_th1 = new JSlider();
        slider_th1.setMaximum(900);
        slider_th1.setValue(0);
        slider_th1.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showKsizeValue.setText(slider_th1.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Canny(source, slider_th1.getValue(),
                                         slider_th2.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_th1.setBounds(72, 10, 164, 25);
        frmjavaSwing.getContentPane().add(slider_th1);

        JLabel lblAlpha = new JLabel("threshold1");
        lblAlpha.setBounds(8, 10, 64, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("threshold2");
        lblBeta.setBounds(264, 10, 64, 15);
        frmjavaSwing.getContentPane().add(lblBeta);


        slider_th2.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showShapeValue.setText(slider_th2.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Canny(source, slider_th1.getValue(),
                                         slider_th2.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat Canny(Mat source, double threshold1, double threshold2)
    {
        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.Canny(source, destination, threshold1, threshold2);
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

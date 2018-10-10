package ch08;

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

public class Ch08_1_1Erode
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
                    Ch08_1_1Erode window = new Ch08_1_1Erode();
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
    public Ch08_1_1Erode()
    {
        initialize();
        System.out.println("Imgproc.MORPH_RECT=" + Imgproc.MORPH_RECT);
        System.out.println("Imgproc.MORPH_ELLIPSE=" + Imgproc.MORPH_ELLIPSE);
        System.out.println("Imgproc.MORPH_CROSS=" + Imgproc.MORPH_CROSS);

    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv Erode(腐蝕)處理練習");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showKsizeValue = new JLabel("0");
        showKsizeValue.setBounds(250, 10, 46, 15);
        frmjavaSwing.getContentPane().add(showKsizeValue);

        final JLabel showShapeValue = new JLabel("0");
        showShapeValue.setBounds(402, 10, 46, 15);
        frmjavaSwing.getContentPane().add(showShapeValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_shape = new JSlider();
        slider_shape.setValue(0);
        slider_shape.setMaximum(2);
        slider_shape.setBounds(326, 10, 75, 25);
        frmjavaSwing.getContentPane().add(slider_shape);

        final JSlider slider_ksize = new JSlider();
        slider_ksize.setMaximum(50);
        slider_ksize.setValue(0);
        slider_ksize.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showKsizeValue.setText(slider_ksize.getValue() + "");
                BufferedImage newImage = matToBufferedImage(erode(source,
                                         slider_ksize.getValue(), slider_shape.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_ksize.setBounds(47, 10, 200, 25);
        frmjavaSwing.getContentPane().add(slider_ksize);

        JLabel lblAlpha = new JLabel("Ksize");
        lblAlpha.setBounds(10, 20, 46, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("shape");
        lblBeta.setBounds(283, 10, 46, 15);
        frmjavaSwing.getContentPane().add(lblBeta);


        slider_shape.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showShapeValue.setText(slider_shape.getValue() + "");
                BufferedImage newImage = matToBufferedImage(erode(source,
                                         slider_ksize.getValue(), slider_shape.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat erode(Mat source, int ksize, int shape)
    {

        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Mat element = Imgproc.getStructuringElement(shape, new Size(2 * ksize + 1,
                      2 * ksize + 1));
        Imgproc.erode(source, destination, element);

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

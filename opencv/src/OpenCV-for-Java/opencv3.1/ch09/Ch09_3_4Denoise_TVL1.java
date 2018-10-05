package ch09;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.List;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.photo.Photo;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch09_3_4Denoise_TVL1
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
                    Ch09_3_4Denoise_TVL1 window = new Ch09_3_4Denoise_TVL1();
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
    public Ch09_3_4Denoise_TVL1()
    {
        initialize();

    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg",
                                            Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv denoise_TVL1³B²z½m²ß");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showLambdaValue = new JLabel("0");
        showLambdaValue.setBounds(267, 10, 46, 15);
        frmjavaSwing.getContentPane().add(showLambdaValue);

        final JLabel showNitersValue = new JLabel("1");
        showNitersValue.setBounds(456, 10, 46, 15);
        frmjavaSwing.getContentPane().add(showNitersValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_niters = new JSlider();
        slider_niters.setMinimum(1);
        slider_niters.setValue(1);
        slider_niters.setMaximum(20);
        slider_niters.setBounds(371, 10, 75, 25);
        frmjavaSwing.getContentPane().add(slider_niters);

        final JSlider slider_lambda = new JSlider();
        slider_lambda.setMinimum(1);
        slider_lambda.setMaximum(50);
        slider_lambda.setValue(1);
        slider_lambda.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showLambdaValue.setText(slider_lambda.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Denoise(source,
                                         slider_lambda.getValue(), slider_niters.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_lambda.setBounds(66, 10, 200, 25);
        frmjavaSwing.getContentPane().add(slider_lambda);

        JLabel lblAlpha = new JLabel("lambda");
        lblAlpha.setBounds(10, 20, 46, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("niters");
        lblBeta.setBounds(323, 10, 46, 15);
        frmjavaSwing.getContentPane().add(lblBeta);


        slider_niters.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showNitersValue.setText(slider_niters.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Denoise(source,
                                         slider_lambda.getValue(), slider_niters.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat Denoise(Mat source, int lambda, int niters)
    {
        Mat im = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg",
                                  Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        Mat im1 = Imgcodecs.imread("C://opencv3.1//samples//lena1.jpg",
                                   Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        Mat im2 = Imgcodecs.imread("C://opencv3.1//samples//lena2.jpg",
                                   Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        List<Mat> srcList = new ArrayList<Mat>();
        srcList.add(im);
        //      srcList.add(im1);
        //      srcList.add(im2);
        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Photo.denoise_TVL1(srcList, destination, lambda, niters);
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

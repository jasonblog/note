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

public class Ch09_7_1TextureFlattening
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
                    Ch09_7_1TextureFlattening window = new Ch09_7_1TextureFlattening();
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
    public Ch09_7_1TextureFlattening()
    {
        initialize();

    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//DSC_0679.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv Texture Flattening³B²z½m²ß");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showlowTValue = new JLabel("1");
        showlowTValue.setBounds(82, 54, 46, 15);
        frmjavaSwing.getContentPane().add(showlowTValue);

        final JLabel showhighTValue = new JLabel("1");
        showhighTValue.setBounds(222, 54, 46, 15);
        frmjavaSwing.getContentPane().add(showhighTValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_highT = new JSlider();
        slider_highT.setMaximum(200);
        slider_highT.setMinimum(1);
        slider_highT.setValue(1);
        slider_highT.setBounds(165, 21, 132, 25);
        frmjavaSwing.getContentPane().add(slider_highT);

        final JSlider slider_lowT = new JSlider();
        slider_lowT.setMinimum(1);
        slider_lowT.setValue(1);

        slider_lowT.setBounds(20, 21, 125, 25);
        frmjavaSwing.getContentPane().add(slider_lowT);

        JLabel lblAlpha = new JLabel("low_threshold");
        lblAlpha.setBounds(10, 5, 100, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("high_threshold");
        lblBeta.setBounds(179, 5, 100, 15);
        frmjavaSwing.getContentPane().add(lblBeta);

        JLabel lblNewLabel_1 = new JLabel("kernel_size");
        lblNewLabel_1.setBounds(354, 5, 85, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);

        final JSlider slider_ksize = new JSlider();

        slider_ksize.setValue(3);
        slider_ksize.setMinimum(3);
        slider_ksize.setMaximum(7);
        slider_ksize.setBounds(339, 21, 109, 25);
        frmjavaSwing.getContentPane().add(slider_ksize);

        final JLabel showKsizeValue = new JLabel("3");
        showKsizeValue.setBounds(389, 54, 46, 15);
        frmjavaSwing.getContentPane().add(showKsizeValue);

        slider_ksize.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                if (slider_ksize.getValue() % 2 == 0) {
                    slider_ksize.setValue(slider_ksize.getValue() + 1);
                }

                showKsizeValue.setText(slider_ksize.getValue() + "");
                BufferedImage newImage = matToBufferedImage(TextureFlattening(
                                             slider_lowT.getValue(), slider_highT.getValue(), slider_ksize.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_lowT.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());


                showlowTValue.setText(slider_lowT.getValue() + "");
                BufferedImage newImage = matToBufferedImage(TextureFlattening(
                                             slider_lowT.getValue(), slider_highT.getValue(), slider_ksize.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_highT.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {

                showhighTValue.setText(slider_highT.getValue() + "");
                BufferedImage newImage = matToBufferedImage(TextureFlattening(
                                             slider_lowT.getValue(), slider_highT.getValue(), slider_ksize.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat TextureFlattening(float low_threshold, float high_threshold,
                                 int kernel_size)
    {
        System.out.println("low_threshold=" + low_threshold + ",high_threshold=" +
                           high_threshold + ",kernel_size=" + kernel_size);
        Mat src = Imgcodecs.imread("C://opencv3.1//samples//DSC_0679.jpg",
                                   Imgcodecs.CV_LOAD_IMAGE_COLOR);
        Mat destination = new Mat();
        Mat src_mask = new Mat(src.rows(), src.cols(), src.depth());

        for (int i = 0; i < src_mask.rows(); i++) {
            for (int j = 0; j < src_mask.cols(); j++) {
                double[] data = new double[3];
                data = src_mask.get(i, j);
                data[0] = 255;
                src_mask.put(i, j, data);
            }
        }

        Photo.textureFlattening(src, src_mask, destination, low_threshold,
                                high_threshold, kernel_size);
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

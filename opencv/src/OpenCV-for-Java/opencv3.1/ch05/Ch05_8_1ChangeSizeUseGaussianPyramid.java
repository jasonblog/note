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
import javax.swing.JButton;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

public class Ch05_8_1ChangeSizeUseGaussianPyramid
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmjavaSwing;
    private Mat source = new Mat();
    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {

        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch05_8_1ChangeSizeUseGaussianPyramid window = new
                    Ch05_8_1ChangeSizeUseGaussianPyramid();
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
    public Ch05_8_1ChangeSizeUseGaussianPyramid()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat src = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");
        setSource(src);
        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 放大縮小練習2");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        JButton btnNewButton = new JButton("放大2倍");
        btnNewButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(pyrUp(getSource()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnNewButton.setBounds(63, 21, 87, 23);
        frmjavaSwing.getContentPane().add(btnNewButton);

        JButton button = new JButton("縮小2倍");
        button.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(pyrDown(getSource()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        button.setBounds(221, 21, 87, 23);
        frmjavaSwing.getContentPane().add(button);
    }
    public Mat pyrUp(Mat source)
    {

        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.pyrUp(source, destination, new Size(source.rows() * 2,
                      source.cols() * 2));
        setSource(destination);
        return destination;

    }
    public Mat pyrDown(Mat source)
    {

        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.pyrDown(source, destination, new Size(source.rows() * 0.5,
                        source.cols() * 0.5));
        setSource(destination);
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

    public Mat getSource()
    {
        return source;
    }

    public void setSource(Mat source)
    {
        this.source = source;
    }
}

package ch05;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import javax.swing.JButton;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

public class Ch05_9_2RotateUseFlip
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
                    Ch05_9_2RotateUseFlip window = new Ch05_9_2RotateUseFlip();
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
    public Ch05_9_2RotateUseFlip()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");
        Mat destination = new Mat(source.rows(), source.cols(), source.type());

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv翻轉練習2,使用Core.flip");
        frmjavaSwing.setBounds(100, 100, 560, 620);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(5, 60, image.getHeight() + 10, image.getWidth() + 10);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        JButton btnX = new JButton("水平翻轉");
        btnX.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(chgDirection(source, 1));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnX.setBounds(30, 10, 87, 23);
        frmjavaSwing.getContentPane().add(btnX);

        JButton btnNewButton = new JButton("垂直翻轉");
        btnNewButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(chgDirection(source, 0));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnNewButton.setBounds(178, 10, 87, 23);
        frmjavaSwing.getContentPane().add(btnNewButton);

        JButton btnNewButton_1 = new JButton("水平及垂直翻轉");
        btnNewButton_1.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(chgDirection(source, -1));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnNewButton_1.setBounds(316, 10, 134, 23);
        frmjavaSwing.getContentPane().add(btnNewButton_1);
    }

    public Mat chgDirection(Mat source, int direction)
    {

        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Core.flip(source, destination, direction);
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

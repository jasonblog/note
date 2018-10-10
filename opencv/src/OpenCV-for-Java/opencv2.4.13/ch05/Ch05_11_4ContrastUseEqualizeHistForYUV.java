package ch05;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.List;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;

public class Ch05_11_4ContrastUseEqualizeHistForYUV
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
                    Ch05_11_4ContrastUseEqualizeHistForYUV window = new
                    Ch05_11_4ContrastUseEqualizeHistForYUV();
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
    public Ch05_11_4ContrastUseEqualizeHistForYUV()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//lena.jpg");

        BufferedImage image = matToBufferedImage(equalizeHist(source));

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv彩色影像對比練習3");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 10, 438, 455);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);
    }

    public Mat equalizeHist(Mat source)
    {
        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Mat tempYUV = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.cvtColor(source, tempYUV, Imgproc.COLOR_RGB2YUV);
        List<Mat> yuvList = new ArrayList<Mat>(3);
        Core.split(tempYUV, yuvList);
        Imgproc.equalizeHist(yuvList.get(0), yuvList.get(0));
        //Imgproc.equalizeHist(yuvList.get(1), yuvList.get(1));
        //Imgproc.equalizeHist(yuvList.get(2), yuvList.get(2));
        Core.merge(yuvList, destination);
        Imgproc.cvtColor(destination, destination, Imgproc.COLOR_YUV2RGB);
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

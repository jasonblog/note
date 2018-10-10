package ch04;

import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;

public class Ch04_1_3SwingReadImgWithPanel
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String arg[])
    {
        Mat source = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");

        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("讀取影像至Java Swing視窗");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 600);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);

        panel1.setimagewithMat(source);
        frame1.repaint();
    }
}

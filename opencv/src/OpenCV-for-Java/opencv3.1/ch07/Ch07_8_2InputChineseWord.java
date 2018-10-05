package ch07;

import java.awt.Color;
import java.awt.EventQueue;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.GraphicsEnvironment;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import java.io.IOException;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;

public class Ch07_8_2InputChineseWord
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
                    Ch07_8_2InputChineseWord window = new Ch07_8_2InputChineseWord();
                    window.frmjavaSwing.setVisible(true);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }

    public Ch07_8_2InputChineseWord() throws IOException {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     * @throws IOException
     */
    private void initialize() throws IOException {
        Mat source =  paint();
        BufferedImage image = matToBufferedImage(source);
        showAllFontName();

        Graphics2D g2d = image.createGraphics();
        g2d.setColor(new Color(0, 0, 0)); //字顏色
        g2d.setFont(new Font("王漢宗鋼筆行楷繁", 36, 20));//字型,字型style,大小
        //g2d.setFont(new Font("王漢宗勘亭流繁", 36, 20));//字型,字型style,大小
        g2d.drawString("我愛OpenCV,加油!", 10, 230); //字及位置



        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 輸入中文練習");
        frmjavaSwing.setBounds(100, 100, 377, 356);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 37, 296, 255);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);


    }
    public Mat paint()
    {

        Mat source = new Mat(250, 250, CvType.CV_8UC3, new Scalar(255, 255, 255));

        Imgproc.putText(source, new String("I love Opencv"), new Point(125, 25), 0, 0.5,
                        new Scalar(255, 0, 0));
        Imgproc.putText(source, new String("I love Opencv"), new Point(125, 50), 1, 0.5,
                        new Scalar(255, 0, 0));
        Imgproc.putText(source, new String("I love Opencv"), new Point(125, 75), 2, 0.5,
                        new Scalar(255, 0, 0));
        Imgproc.putText(source, new String("I love Opencv"), new Point(125, 100), 3,
                        0.5, new Scalar(255, 0, 0));
        Imgproc.putText(source, new String("I love Opencv"), new Point(125, 125), 4,
                        0.5, new Scalar(255, 0, 0));
        Imgproc.putText(source, new String("I love Opencv"), new Point(125, 150), 5,
                        0.5, new Scalar(255, 0, 0));
        Imgproc.putText(source, new String("I love Opencv"), new Point(125, 175), 6,
                        0.5, new Scalar(255, 0, 0));
        Imgproc.putText(source, new String("I love Opencv"), new Point(125, 200), 7,
                        0.5, new Scalar(255, 0, 0));
        return source;
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

    //顯示所有字型名稱
    public void showAllFontName()
    {
        GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
        Font[] allFont =    ge.getAllFonts();

        for (int i = 0; i < allFont.length; i++) {
            System.out.println("顯示所有字型名稱:" + allFont[i].getFontName());
        }
    }



}

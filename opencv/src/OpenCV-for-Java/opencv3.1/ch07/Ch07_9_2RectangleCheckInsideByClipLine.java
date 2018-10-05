package ch07;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch07_9_2RectangleCheckInsideByClipLine
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
                    Ch07_9_2RectangleCheckInsideByClipLine window = new
                    Ch07_9_2RectangleCheckInsideByClipLine();
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
    public Ch07_9_2RectangleCheckInsideByClipLine()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        // Mat source =  paint();
        //BufferedImage image=matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 判斷A矩形是否有在B矩形內.");
        frmjavaSwing.setBounds(100, 100, 401, 378);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 64, 281, 270);
        //lblNewLabel.setIcon(new ImageIcon(image));


        BufferedImage image = matToBufferedImage(paint());
        lblNewLabel.setIcon(new ImageIcon(image));

        frmjavaSwing.getContentPane().add(lblNewLabel);
    }
    public Mat paint()
    {

        Mat source = new Mat(300, 300, CvType.CV_8UC3, new Scalar(255, 255, 255));

        //B
        Imgproc.rectangle(source, new Point(100, 100), new Point(200, 200),
                          new Scalar(255, 0, 0), -1);
        Rect B = new Rect(100, 100, 200, 200);

        //A1
        Imgproc.rectangle(source, new Point(50, 50), new Point(250, 250), new Scalar(0,
                          255, 0), 1);
        Rect A1 = new Rect(50, 50, 250, 250);
        boolean bA1 = Imgproc.clipLine(B, new Point(50, 50), new Point(250, 250));

        System.out.println("A1矩形是否在B矩形內=" + bA1);


        //A2
        Imgproc.rectangle(source, new Point(120, 120), new Point(180, 180),
                          new Scalar(0, 255, 0), 3);
        Rect A2 = new Rect(120, 120, 180, 180);

        boolean bA2 = Imgproc.clipLine(B, new Point(120, 120), new Point(180, 180));
        System.out.println("A2矩形是否在B矩形內=" + bA2);


        //A3
        Imgproc.rectangle(source, new Point(20, 20), new Point(80, 80), new Scalar(0, 0,
                          255), 2);
        Rect A3 = new Rect(20, 20, 80, 80);

        boolean bA3 = Imgproc.clipLine(B, new Point(20, 20), new Point(80, 80));
        System.out.println("A3矩形是否在B矩形內=" + bA3);

        //A4
        Imgproc.rectangle(source, new Point(190, 190), new Point(240, 240),
                          new Scalar(0, 255, 255), 2);
        Rect A4 = new Rect(240, 240, 280, 280);

        boolean bA4 = Imgproc.clipLine(B,  new Point(190, 190), new Point(240, 240));
        System.out.println("A4矩形是否在B矩形內=" + bA4);

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
}

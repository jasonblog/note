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
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;

public class Ch07_15_1ArrowedLine
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
                    Ch07_15_1ArrowedLine window = new Ch07_15_1ArrowedLine();
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
    public Ch07_15_1ArrowedLine()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        Mat source =  paint();
        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv µeΩb¿YΩuΩm≤ﬂ");
        frmjavaSwing.setBounds(100, 100, 401, 378);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(5, 10, 281, 270);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);
    }
    public Mat paint()
    {

        Mat source = new Mat(250, 250, CvType.CV_8UC3, new Scalar(255, 255, 255));
        Point center = new Point(250 / 2, 250 / 2);
        int lineType = 8;
        int thickness = 1;
        double tipLength = 3;

        for (int angle = 0; angle < 360; angle += 15) {
            double angleRad = angle * Math.PI / 180.0; // convert angle to radians

            int length = 150;
            Point direction = new Point(length * Math.cos(angleRad),
                                        length * Math.sin(angleRad)); // calculate direction
            tipLength = .01 + 0.4 * (angle % 180) / 360;

            Core.arrowedLine(source, new Point(center.x + direction.x * 0.5,
                                               center.y + direction.y * 0.5), new Point(center.x + direction.x,
                                                       center.y + direction.y), new Scalar(255, angle, 0), thickness, lineType, 0,
                             tipLength); // draw arrow!

            ++thickness;

            if (0 == angle % 45) {
                thickness = 0;
            }

        }


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

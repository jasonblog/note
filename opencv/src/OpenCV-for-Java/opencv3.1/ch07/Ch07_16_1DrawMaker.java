package ch07;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch07_16_1DrawMaker
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
                    Ch07_16_1DrawMaker window = new Ch07_16_1DrawMaker();
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
    public Ch07_16_1DrawMaker()
    {
        initialize();
        System.out.println("Imgproc.MARKER_CROSS=" + Imgproc.MARKER_CROSS);
        System.out.println("Imgproc.MARKER_DIAMOND=" + Imgproc.MARKER_DIAMOND);
        System.out.println("Imgproc.MARKER_SQUARE=" + Imgproc.MARKER_SQUARE);
        System.out.println("Imgproc.MARKER_STAR=" + Imgproc.MARKER_STAR);
        System.out.println("Imgproc.MARKER_TILTED_CROSS=" +
                           Imgproc.MARKER_TILTED_CROSS);
        System.out.println("Imgproc.MARKER_TRIANGLE_DOWN=" +
                           Imgproc.MARKER_TRIANGLE_DOWN);
        System.out.println("Imgproc.MARKER_TRIANGLE_UP=" + Imgproc.MARKER_TRIANGLE_UP);
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        // Mat source =  paint();
        //BufferedImage image=matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv µe¯S®í²Å¸¹½m²ß");
        frmjavaSwing.setBounds(100, 100, 401, 378);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 64, 281, 270);
        //lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        JLabel lblNewLabel_1 = new JLabel("Maker");
        lblNewLabel_1.setBounds(10, 10, 46, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);

        final JSlider slider = new JSlider();

        slider.setValue(1);
        slider.setMaximum(7);
        slider.setMinimum(1);
        slider.setBounds(53, 10, 200, 25);
        frmjavaSwing.getContentPane().add(slider);

        JLabel lblMarkerValue = new JLabel("1");
        lblMarkerValue.setBounds(263, 10, 46, 15);
        frmjavaSwing.getContentPane().add(lblMarkerValue);


        slider.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {

                BufferedImage image = matToBufferedImage(paint(slider.getValue()));
                lblNewLabel.setIcon(new ImageIcon(image));
            }
        });
    }
    public Mat paint(int marker)
    {

        Mat source = Imgcodecs.imread("C://opencv3.1//samples//IMG_1017.jpg");
        Imgproc.drawMarker(source, new Point(230, 250), new Scalar(20, 105, 66),
                           marker, 20, 3, 8);
        //Imgcodecs.imwrite("C://opencv3.1//samples//drawMaker_test.jpg",source);

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

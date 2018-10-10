package ch08;

import java.awt.EventQueue;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch08_11_1ApplyColorMap
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
                    Ch08_11_1ApplyColorMap window = new Ch08_11_1ApplyColorMap();
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
    public Ch08_11_1ApplyColorMap()
    {
        initialize();
        System.out.println("Imgproc.COLORMAP_AUTUMN=" + Imgproc.COLORMAP_AUTUMN);
        System.out.println("Imgproc.COLORMAP_BONE=" + Imgproc.COLORMAP_BONE);
        System.out.println("Imgproc.COLORMAP_COOL=" + Imgproc.COLORMAP_COOL);
        System.out.println("Imgproc.COLORMAP_HOT=" + Imgproc.COLORMAP_HOT);
        System.out.println("Imgproc.COLORMAP_HSV=" + Imgproc.COLORMAP_HSV);
        System.out.println("Imgproc.COLORMAP_JET=" + Imgproc.COLORMAP_JET);
        System.out.println("Imgproc.COLORMAP_OCEAN=" + Imgproc.COLORMAP_OCEAN);
        System.out.println("Imgproc.COLORMAP_PARULA=" + Imgproc.COLORMAP_PARULA);
        System.out.println("Imgproc.COLORMAP_PINK=" + Imgproc.COLORMAP_PINK);
        System.out.println("Imgproc.COLORMAP_RAINBOW=" + Imgproc.COLORMAP_RAINBOW);
        System.out.println("Imgproc.COLORMAP_SPRING=" + Imgproc.COLORMAP_SPRING);
        System.out.println("Imgproc.COLORMAP_SUMMER=" + Imgproc.COLORMAP_SUMMER);
        System.out.println("Imgproc.COLORMAP_WINTER=" + Imgproc.COLORMAP_WINTER);
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg",
                                            Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);

        //BufferedImage image=matToBufferedImage(applyColorMap(source,alpha));

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("讀取影像至Java Swing視窗");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showAlphaValue = new JLabel("2");
        showAlphaValue.setBounds(311, 20, 46, 15);
        frmjavaSwing.getContentPane().add(showAlphaValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        //lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_alpha = new JSlider();
        slider_alpha.setMaximum(12);
        slider_alpha.setValue(2);
        slider_alpha.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showAlphaValue.setText(slider_alpha.getValue() + "");
                BufferedImage newImage = matToBufferedImage(applyColorMap(source,
                                         slider_alpha.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_alpha.setBounds(101, 10, 200, 25);
        frmjavaSwing.getContentPane().add(slider_alpha);

        JLabel lblAlpha = new JLabel("ColorMap");

        lblAlpha.setBounds(10, 20, 92, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

    }
    public Mat applyColorMap(Mat source, int colormap)
    {

        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.applyColorMap(source, destination, colormap);

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

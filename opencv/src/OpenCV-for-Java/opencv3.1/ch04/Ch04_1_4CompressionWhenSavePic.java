package ch04;

import java.awt.EventQueue;
import java.awt.image.BufferedImage;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;
import javax.swing.JButton;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfInt;
import org.opencv.imgcodecs.Imgcodecs;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

public class Ch04_1_4CompressionWhenSavePic
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }
    Mat source = Imgcodecs.imread("C://opencv3.1//samples//DSC_0600.jpg");

    private JFrame frmRr;

    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {
        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch04_1_4CompressionWhenSavePic window = new Ch04_1_4CompressionWhenSavePic();
                    window.frmRr.setVisible(true);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }

    /**
     * Create the application.
     */
    public Ch04_1_4CompressionWhenSavePic()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {


        BufferedImage image = matToBufferedImage(source);


        frmRr = new JFrame();
        frmRr.setTitle("影像儲存壓縮品質選擇");
        frmRr.setBounds(100, 100, 489, 529);
        frmRr.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmRr.getContentPane().setLayout(null);

        JLabel lblNewLabel = new JLabel("JPEG QUALITY ");
        lblNewLabel.setBounds(10, 10, 112, 15);
        frmRr.getContentPane().add(lblNewLabel);

        final JSlider slider_jpg = new JSlider();

        slider_jpg.setValue(95);
        slider_jpg.setBounds(99, 10, 200, 25);
        frmRr.getContentPane().add(slider_jpg);

        final JLabel lblNewLabel_jpg = new JLabel("95");
        lblNewLabel_jpg.setBounds(317, 10, 46, 15);
        frmRr.getContentPane().add(lblNewLabel_jpg);

        JLabel lblPngQuality = new JLabel("PNG QUALITY ");
        lblPngQuality.setBounds(10, 47, 112, 15);
        frmRr.getContentPane().add(lblPngQuality);

        final JSlider slider_png = new JSlider();

        slider_png.setMaximum(9);
        slider_png.setValue(3);
        slider_png.setBounds(99, 37, 200, 25);
        frmRr.getContentPane().add(slider_png);

        final JLabel lblNewLabel_png = new JLabel("3");
        lblNewLabel_png.setBounds(317, 47, 46, 15);
        frmRr.getContentPane().add(lblNewLabel_png);

        JButton Button_jpg = new JButton("Save as JPG");
        Button_jpg.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {

                MatOfInt JpgCompressionRate = new MatOfInt(Imgcodecs.IMWRITE_JPEG_QUALITY,
                        slider_jpg.getValue());
                Imgcodecs.imwrite("C://opencv3.1//samples//lena-QUALITY" + slider_jpg.getValue()
                                  + ".jpg", source, JpgCompressionRate);
            }
        });
        Button_jpg.setBounds(356, 6, 117, 23);
        frmRr.getContentPane().add(Button_jpg);

        JButton Button_png = new JButton("Save as PNG");
        Button_png.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                MatOfInt PngCompressionRate = new MatOfInt(Imgcodecs.IMWRITE_PNG_COMPRESSION,
                        slider_png.getValue());
                Imgcodecs.imwrite("C://opencv3.1//samples//DSC_0600-COMPRESSION" +
                                  slider_png.getValue() + ".png", source, PngCompressionRate);
            }
        });
        Button_png.setBounds(356, 43, 117, 23);
        frmRr.getContentPane().add(Button_png);

        JLabel Label_img = new JLabel("");
        Label_img.setBounds(10, 72, 446, 413);
        frmRr.getContentPane().add(Label_img);
        Label_img.setIcon(new ImageIcon(image));

        slider_jpg.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                lblNewLabel_jpg.setText(slider_jpg.getValue() + "");
            }
        });

        slider_png.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {

                lblNewLabel_png.setText(slider_png.getValue() + "");

            }
        });

    }

    public BufferedImage matToBufferedImage(Mat matrix)
    {
        int cols = matrix.cols();
        int rows = matrix.rows();
        int elemSize = (int) matrix.elemSize();
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

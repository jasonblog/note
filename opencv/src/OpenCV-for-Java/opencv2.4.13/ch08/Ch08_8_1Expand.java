package ch08;


import java.awt.Component;
import java.awt.EventQueue;

import javax.swing.JFrame;
import java.awt.Canvas;

import javax.swing.ImageIcon;
import javax.swing.JColorChooser;
import javax.swing.JPanel;
import javax.swing.JLabel;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;

import java.awt.event.MouseMotionAdapter;
import java.awt.event.MouseEvent;
import java.awt.Color;
import java.awt.event.MouseAdapter;
import java.awt.image.BufferedImage;
import javax.swing.JButton;
import javax.swing.border.LineBorder;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch08_8_1Expand
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmOpencv;
    private Mat sourceMat = Highgui.imread("C://opencv3.1//samples//lena.jpg");
    private Color color;

    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {
        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch08_8_1Expand window = new Ch08_8_1Expand();
                    window.frmOpencv.setVisible(true);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }

    /**
     * Create the application.
     */
    public Ch08_8_1Expand()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {



        final BufferedImage image = matToBufferedImage(sourceMat);

        frmOpencv = new JFrame();
        frmOpencv.setTitle("opencv 凸透鏡效果練習");
        frmOpencv.getContentPane().setBackground(Color.LIGHT_GRAY);
        frmOpencv.setBounds(100, 100, 547, 627);
        frmOpencv.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmOpencv.getContentPane().setLayout(null);

        final JLabel lblShowXY = new JLabel("");
        lblShowXY.setBounds(405, 2, 124, 23);
        frmOpencv.getContentPane().add(lblShowXY);


        JPanel panel = new JPanel();

        final JLabel lblPaintLabel = new JLabel("");
        lblPaintLabel.setIcon(new ImageIcon(image));
        panel.add(lblPaintLabel);


        panel.setBounds(10, 64, 519, 519);
        frmOpencv.getContentPane().add(panel);

        JLabel lblNewLabel = new JLabel("強度");
        lblNewLabel.setBounds(10, 20, 58, 15);
        frmOpencv.getContentPane().add(lblNewLabel);

        final JSlider slider = new JSlider();
        slider.setMaximum(300);

        slider.setValue(30);
        slider.setMinimum(1);
        slider.setBounds(78, 10, 200, 25);
        frmOpencv.getContentPane().add(slider);

        final JLabel Label_scale = new JLabel("30");
        Label_scale.setBounds(298, 20, 46, 15);
        frmOpencv.getContentPane().add(Label_scale);

        slider.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                Label_scale.setText(slider.getValue() + "");
            }
        });

        panel.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent arg0) {
                lblShowXY.setText("X:" + arg0.getX() + ",Y:" + arg0.getY());
                BufferedImage newImage = matToBufferedImage(expand(sourceMat, slider.getValue(),
                                         arg0.getX(), arg0.getY()));
                lblPaintLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }

    public Mat expand(Mat source, int Scale, int PntX, int PntY)
    {
        Point pt = new Point(PntX, PntY);
        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        source.copyTo(destination);

        for (int y = 0; y < source.height(); y++) {

            for (int x = 0; x < source.width(); x++) {

                int dist = (int)getDistance(new Point(x, y), pt);

                if (dist < Scale) {
                    int newX = (int)(((x - pt.x) * dist / Scale) + pt.x);
                    int newY = (int)(((y - pt.y) * dist / Scale) + pt.y);
                    double[] temp = source.get(newY, newX * 2);
                    double[] temp1 = source.get(newY, newX * 2 + 1);
                    double[] temp2 = source.get(newY, newX * 2 + 2);

                    if (temp != null) {
                        destination.put(y, 2 * x, temp);
                    }

                    if (temp1 != null) {
                        destination.put(y, 2 * x + 1, temp1);
                    }

                    if (temp2 != null) {
                        destination.put(y, 2 * x + 2, temp2);
                    }
                }
            }
        }

        return destination;
    }
    public static double getDistance(Point a, Point b)
    {
        return Math.sqrt(Math.pow(a.x - b.x, 2) + Math.pow(a.y - b.y, 2));
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

    public Color getColor()
    {
        return color;
    }

    public void setColor(Color color)
    {
        this.color = color;
    }
}

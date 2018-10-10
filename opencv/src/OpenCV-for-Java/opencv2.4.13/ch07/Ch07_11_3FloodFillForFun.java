package ch07;

import java.awt.Component;
import java.awt.EventQueue;
import javax.swing.JFrame;
import javax.swing.ImageIcon;
import javax.swing.JColorChooser;
import javax.swing.JPanel;
import javax.swing.JLabel;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
import java.awt.event.MouseEvent;
import java.awt.Color;
import java.awt.event.MouseAdapter;
import java.awt.image.BufferedImage;
import javax.swing.JButton;
import javax.swing.border.LineBorder;

public class Ch07_11_3FloodFillForFun
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmOpencv;
    private Mat paintMat =
        Highgui.imread("C://opencv3.1//samples//floodFill-template.png");
    private Color color;

    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {
        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch07_11_3FloodFillForFun window = new Ch07_11_3FloodFillForFun();
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
    public Ch07_11_3FloodFillForFun()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {



        final BufferedImage image = matToBufferedImage(paintMat);

        frmOpencv = new JFrame();
        frmOpencv.setTitle("opencv 填充顏色練習");
        frmOpencv.getContentPane().setBackground(Color.LIGHT_GRAY);
        frmOpencv.setBounds(100, 100, 476, 300);
        frmOpencv.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmOpencv.getContentPane().setLayout(null);

        final JLabel lblShowXY = new JLabel("");
        lblShowXY.setBounds(22, 218, 124, 23);
        frmOpencv.getContentPane().add(lblShowXY);


        JPanel panel = new JPanel();

        final JLabel lblPaintLabel = new JLabel("");
        lblPaintLabel.setIcon(new ImageIcon(image));
        panel.add(lblPaintLabel);

        panel.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent arg0) {
                lblShowXY.setText("X:" + arg0.getX() + ",Y:" + arg0.getY());
                BufferedImage newImage = matToBufferedImage(floodFill(paintMat, getColor(),
                                         arg0.getX(), arg0.getY()));
                lblPaintLabel.setIcon(new ImageIcon(newImage));
            }
        });
        panel.setBounds(22, 23, 324, 181);
        frmOpencv.getContentPane().add(panel);

        JButton btnNewButton = new JButton("清除");
        btnNewButton.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent arg0) {
                paintMat =
                    Highgui.imread("C://opencv3.1//samples//done-floodFill-template.png");
                BufferedImage newImage = matToBufferedImage(paintMat);
                lblPaintLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnNewButton.setBounds(356, 151, 87, 23);
        frmOpencv.getContentPane().add(btnNewButton);

        JButton btnNewButton_1 = new JButton("Save");
        btnNewButton_1.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent arg0) {
                save(paintMat);
            }
        });
        btnNewButton_1.setBounds(356, 181, 87, 23);
        frmOpencv.getContentPane().add(btnNewButton_1);

        final JPanel panel_Color = new JPanel();
        panel_Color.setBorder(new LineBorder(new Color(0, 0, 0), 2));
        panel_Color.setBounds(363, 47, 80, 94);
        frmOpencv.getContentPane().add(panel_Color);

        JLabel lblNewLabel = new JLabel("目前顏色");
        lblNewLabel.setBounds(364, 22, 79, 15);
        frmOpencv.getContentPane().add(lblNewLabel);

        panel_Color.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {

                lblShowXY.setText("X:" + e.getX() + ",Y:" + e.getY());


                Color c;
                c = JColorChooser.showDialog(((Component) e.getSource())
                                             .getParent(), "Choose Color", Color.blue);
                panel_Color.setBackground(c);
                setColor(c);
            }
        });

    }

    public Mat floodFill(Mat src, Color c, int PntX, int PntY)
    {
        Point seedPoint = new Point(PntX, PntY);
        Mat mask = new Mat();
        Rect rect = new Rect();
        Imgproc.floodFill(src, mask, seedPoint, new Scalar(c.getBlue(), c.getGreen(),
                          c.getRed()), rect, new Scalar(10, 10, 10), new Scalar(20, 20, 20), 4);

        return src;
    }

    public void save(Mat src)
    {
        Highgui.imwrite("C://opencv249//floodFill.jpg", src);
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
package ch07;

import java.awt.EventQueue;
import java.awt.Graphics;
import java.awt.Panel;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.JLayeredPane;
import java.awt.Color;
import java.awt.image.BufferedImage;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionAdapter;
import javax.swing.JButton;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

public class Ch07_14_1RoiForRectangle
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmjavaSwing;

    public int srcX = 0;
    public int srcY = 0;

    public int secondX = 0;
    public int secondY = 0;

    public Mat matWantToSave;

    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {

        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch07_14_1RoiForRectangle window = new Ch07_14_1RoiForRectangle();
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
    public Ch07_14_1RoiForRectangle()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//DSC_0625.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("擷取ROI(區塊)練習");
        frmjavaSwing.setBounds(100, 100, 763, 425);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblLocation = new JLabel("");
        lblLocation.setBounds(10, 10, 112, 25);
        frmjavaSwing.getContentPane().add(lblLocation);

        final JLabel lblLocation2 = new JLabel("");
        lblLocation2.setBounds(158, 10, 112, 25);
        frmjavaSwing.getContentPane().add(lblLocation2);

        JPanel panel = new JPanel();
        panel.setBounds(10, 45, 429, 348);
        frmjavaSwing.getContentPane().add(panel);

        final JLabel lblNewLabel = new JLabel("");
        panel.add(lblNewLabel);
        lblNewLabel.addMouseMotionListener(new MouseMotionAdapter() {
            @Override
            public void mouseDragged(MouseEvent arg0) {
                lblLocation2.setText("X:" + arg0.getX() + ",Y:" + arg0.getY());

                BufferedImage newImage = matToBufferedImage(paintRectangle(arg0.getX(),
                                         arg0.getY()));
                lblNewLabel.setIcon(new ImageIcon(newImage));


            }
        });

        final JLabel lblRoi = new JLabel("");
        lblRoi.setBounds(444, 45, 288, 348);
        frmjavaSwing.getContentPane().add(lblRoi);


        lblNewLabel.setIcon(new ImageIcon(image));

        JButton btnNewButton = new JButton("擷取ROI");
        btnNewButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                Mat RoiMat = getRoi();

                if (RoiMat != null) {
                    BufferedImage newImage = matToBufferedImage(RoiMat);
                    lblRoi.setText("");
                    lblRoi.setIcon(new ImageIcon(newImage));
                } else {
                    lblRoi.setText("未規劃ROI區塊!");
                }

            }
        });
        btnNewButton.setBounds(278, 10, 97, 25);
        frmjavaSwing.getContentPane().add(btnNewButton);

        JButton btnNewButton_1 = new JButton("Save");
        btnNewButton_1.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {

                if (getMatWantToSave() != null) {
                    Highgui.imwrite("C://opencv3.1//samples//DSC_0625-subImage.jpg",
                                    getMatWantToSave());
                } else {
                    System.out.println("未擷取ROI區塊!");
                }


            }
        });
        btnNewButton_1.setBounds(398, 10, 89, 25);
        frmjavaSwing.getContentPane().add(btnNewButton_1);



        lblNewLabel.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent arg0) {
                lblLocation2.setText("");
                lblLocation.setText("初始點:X:" + arg0.getX() + ",Y:" + arg0.getY());
                setSrcX(arg0.getX());
                setSrcY(arg0.getY());
                BufferedImage newImage = matToBufferedImage(paintPoint(arg0.getX(),
                                         arg0.getY()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });



    }

    public Mat paintPoint(int x, int y)
    {
        Mat src = Highgui.imread("C://opencv3.1//samples//DSC_0625.jpg");
        Core.circle(src, new Point(x, y), 2, new Scalar(0, 55, 255), 2);
        return src;

    }

    public Mat paintRectangle(int x, int y)
    {
        Mat src = Highgui.imread("C://opencv3.1//samples//DSC_0625.jpg");
        Point srcPoint = new Point(getSrcX(), getSrcY());
        Core.rectangle(src, srcPoint, new Point(x, y), new Scalar(0, 0, 255), 2);
        setSecondX(x);
        setSecondY(y);
        return src;

    }

    public Mat getRoi()
    {
        Mat RoiMat;

        if (getSecondX() > 0 && getSecondY() > 0) {
            Mat source = Highgui.imread("C://opencv3.1//samples//DSC_0625.jpg");
            Rect roi = new Rect(new Point(getSrcX(), getSrcY()), new Point(getSecondX(),
                                getSecondY()));

            RoiMat = source.submat(roi);
            setMatWantToSave(RoiMat);
        } else {
            RoiMat = null;
        }


        return RoiMat;

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

    public int getSrcX()
    {
        return srcX;
    }

    public void setSrcX(int srcX)
    {
        this.srcX = srcX;
    }

    public int getSrcY()
    {
        return srcY;
    }

    public void setSrcY(int srcY)
    {
        this.srcY = srcY;
    }

    public int getSecondX()
    {
        return secondX;
    }

    public void setSecondX(int secondX)
    {
        this.secondX = secondX;
    }

    public int getSecondY()
    {
        return secondY;
    }

    public void setSecondY(int secondY)
    {
        this.secondY = secondY;
    }

    public Mat getMatWantToSave()
    {
        return matWantToSave;
    }

    public void setMatWantToSave(Mat matWantToSave)
    {
        this.matWantToSave = matWantToSave;
    }
}

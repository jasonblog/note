package ch07;

import java.awt.AlphaComposite;
import java.awt.Color;
import java.awt.EventQueue;
import java.awt.Font;
import java.awt.Graphics2D;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;
import javax.swing.JButton;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

public class Ch07_8_3ChineseWaterMark
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmjavaSwing;
    public BufferedImage globalBufferedImage;



    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {

        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch07_8_3ChineseWaterMark window = new Ch07_8_3ChineseWaterMark();
                    window.frmjavaSwing.setVisible(true);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }

    /**
     * Create the application.
     * @throws IOException
     */
    public Ch07_8_3ChineseWaterMark() throws IOException {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     * @throws IOException
     */
    private void initialize() throws IOException {
        Mat source =  Imgcodecs.imread("C://opencv3.1//samples//DSC_0597.jpg");
        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 中文浮水印練習,英文也適用");
        frmjavaSwing.setBounds(100, 100, 639, 356);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 64, 595, 255);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        JLabel lblNewLabel_1 = new JLabel("透明度:");
        lblNewLabel_1.setBounds(10, 21, 46, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);

        final JLabel lblshowAlpha = new JLabel("1.0");
        lblshowAlpha.setBounds(235, 21, 57, 15);
        frmjavaSwing.getContentPane().add(lblshowAlpha);

        final JSlider slider = new JSlider();
        slider.addChangeListener(new ChangeListener()
        {
            public void stateChanged(ChangeEvent arg0) {

                lblshowAlpha.setText((float)slider.getValue() / 10 + "");
                BufferedImage image = HandleWaterMark((float)slider.getValue() / 10);
                lblNewLabel.setIcon(new ImageIcon(image));
                setGlobalBufferedImage(image);

            }
        });
        slider.setValue(10);
        slider.setMaximum(10);
        slider.setBounds(54, 12, 161, 24);
        frmjavaSwing.getContentPane().add(slider);

        JButton btnNewButton = new JButton("Save");
        btnNewButton.addMouseListener(new MouseAdapter()
        {
            @Override
            public void mouseClicked(MouseEvent arg0) {
                BufferedImage image = getGlobalBufferedImage();

                if (image != null) {
                    try {
                        ImageIO.write(image, "png",
                                      new File("C://opencv3.1//samples//DSC_0597_watremark.png"));
                    } catch (IOException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
                }

            }
        });
        btnNewButton.setBounds(277, 16, 89, 25);
        frmjavaSwing.getContentPane().add(btnNewButton);

    }

    //英文也適用
    public BufferedImage HandleWaterMark(float alpha)
    {
        Mat source =  Imgcodecs.imread("C://opencv3.1//samples//DSC_0597.jpg");
        BufferedImage image = matToBufferedImage(source);
        Graphics2D g2d = image.createGraphics();
        System.out.println(alpha);
        g2d.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_ATOP,
                         alpha));//設定透明度
        g2d.setColor(new Color(0, 0, 0)); //字顏色
        g2d.setFont(new Font("王漢宗中隸書繁", 36, 30));//字型,字型style,大小
        //g2d.setFont(new Font("王漢宗勘亭流繁", 36, 20));//字型,字型style,大小
        g2d.drawString("心的澄清湖", 10, 230); //字位置
        g2d.dispose();
        return image;
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

    public BufferedImage getGlobalBufferedImage()
    {
        return globalBufferedImage;
    }

    public void setGlobalBufferedImage(BufferedImage globalBufferedImage)
    {
        this.globalBufferedImage = globalBufferedImage;
    }
}

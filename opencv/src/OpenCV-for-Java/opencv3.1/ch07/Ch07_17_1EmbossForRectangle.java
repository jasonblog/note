package ch07;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.List;

import org.opencv.core.MatOfPoint;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionAdapter;
import javax.swing.JButton;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

public class Ch07_17_1EmbossForRectangle
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmjavaSwing;

    public int srcX = 0;
    public int srcY = 0;

    public int secondX = 0;
    public int secondY = 0;

    public Point innerP1;
    public Point innerP2;

    public Mat matWantToSave;

    public Mat EmbossArea1;
    public Mat EmbossArea2;
    public Mat EmbossArea3;
    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {

        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch07_17_1EmbossForRectangle window = new Ch07_17_1EmbossForRectangle();
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
    public Ch07_17_1EmbossForRectangle()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//DSC_0836.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("擷取ROI製作立體浮雕製作練習");
        frmjavaSwing.setBounds(100, 100, 990, 395);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblLocation = new JLabel("");
        lblLocation.setBounds(10, 10, 112, 25);
        frmjavaSwing.getContentPane().add(lblLocation);

        final JLabel lblLocation2 = new JLabel("");
        lblLocation2.setBounds(158, 10, 112, 25);
        frmjavaSwing.getContentPane().add(lblLocation2);

        JPanel panel = new JPanel();
        panel.setBounds(10, 45, 473, 301);
        frmjavaSwing.getContentPane().add(panel);

        final JLabel lblNewLabel = new JLabel("");
        panel.add(lblNewLabel);

        //監聽mouse事件,點擊第2個的點
        lblNewLabel.addMouseMotionListener(new MouseMotionAdapter() {
            @Override
            public void mouseDragged(MouseEvent arg0) {
                lblLocation2.setText("X:" + arg0.getX() + ",Y:" + arg0.getY());

                setSecondX(arg0.getX());
                setSecondY(arg0.getY());

                BufferedImage newImage = matToBufferedImage(paintRectangle(arg0.getX(),
                                         arg0.getY()));
                lblNewLabel.setIcon(new ImageIcon(newImage));


            }
        });

        final JLabel lblRoi = new JLabel("");
        lblRoi.setBounds(494, 45, 478, 306);
        frmjavaSwing.getContentPane().add(lblRoi);


        lblNewLabel.setIcon(new ImageIcon(image));

        JButton btnNewButton = new JButton("擷取ROI並且立體化");
        btnNewButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                getRoiAndEmboss();

                if (getEmbossArea1() != null) {

                    Mat source1 = Imgcodecs.imread("C://opencv3.1//samples//DSC_0836.jpg");
                    //再把小圖copy到大圖
                    //Area1
                    Mat embossArea1 = getEmbossArea1(); //get浮雕Area1區塊

                    //取Area1及Area3的ROI區域
                    Rect roi = new Rect(Math.min(getSrcX(), getSecondX()), Math.min(getSrcY(),
                                        getSecondY()), Math.abs(getSecondX() - getSrcX()),
                                        Math.abs(getSecondY() - getSrcY()));//不能比原圖大,及小

                    //貼至source1
                    Mat destinationROI = source1.submat(roi);
                    embossArea1.copyTo(destinationROI, embossArea1);
                    //Area2
                    Mat embossArea2 = getEmbossArea2(); //get浮雕Area2區塊
                    //取Area2及Area3的ROI區域
                    Rect tempInner = new Rect(getInnerP1(), getInnerP2());
                    //貼至source1
                    Mat innerMat = source1.submat(tempInner);
                    embossArea2.copyTo(innerMat, embossArea2);

                    //Area3
                    Mat embossArea3 = getEmbossArea3(); //get浮雕Area3區塊
                    //貼至source1
                    embossArea3.copyTo(destinationROI, embossArea3);

                    BufferedImage image = matToBufferedImage(source1);
                    lblRoi.setText("");
                    lblRoi.setIcon(new ImageIcon(image));

                } else {
                    lblRoi.setText("未規劃ROI區塊!");
                }

            }
        });
        btnNewButton.setBounds(278, 10, 150, 25);
        frmjavaSwing.getContentPane().add(btnNewButton);


        //監聽mouse事件,點擊第1個的點
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
    //繪圖於第1個的點
    public Mat paintPoint(int x, int y)
    {
        Mat src = Imgcodecs.imread("C://opencv3.1//samples//DSC_0836.jpg");
        Imgproc.circle(src, new Point(x, y), 2, new Scalar(0, 55, 255), 2);
        return src;

    }
    //繪ROI區,欲立體化 or浮雕化
    public Mat paintRectangle(int x, int y)
    {
        Mat src = Imgcodecs.imread("C://opencv3.1//samples//DSC_0836.jpg");
        Point srcPoint = new Point(getSrcX(), getSrcY());
        Imgproc.rectangle(src, srcPoint, new Point(x, y), new Scalar(0, 0, 255), 2);
        setSecondX(x);
        setSecondY(y);
        return src;

    }
    //處理原圖ROI區變立體化 or浮雕化
    public void getRoiAndEmboss()
    {
        Mat RoiMat;
        Mat MaskTemplate = null;
        Mat dst = new Mat();

        if (getSecondX() > 0 && getSecondY() > 0) {
            Mat source =
                Imgcodecs.imread("C://opencv3.1//samples//DSC_0836.jpg");
            Rect roi = new Rect(new Point(getSrcX(), getSrcY()), new Point(getSecondX(),
                                getSecondY()));

            RoiMat = source.submat(roi);
            // source.

            //area1-start,找出area1的左上斜邊,右上斜,右下斜邊
            //斜角是45度,算出徑度
            double angleRad = 45 * Math.PI / 180.0; // convert angle to radians
            //先定義與ROI同size,全白單通道模板
            MaskTemplate = new Mat(RoiMat.size(), CvType.CV_8UC1, new Scalar(0));

            int length = 25;//立體化 or浮雕化的厚度
            Point direction = new Point(length * Math.cos(angleRad),
                                        length * Math.sin(angleRad)); // calculate direction

            //多邊形fillPoly所需資料結構
            List<MatOfPoint> allMatOfPoint = new ArrayList<MatOfPoint>();
            MatOfPoint mop1 = new MatOfPoint();
            List<Point> allPoint1 = new ArrayList<Point>();

            //area1的6個點,要循序
            allPoint1.add(new Point(0, 0)); //p1
            allPoint1.add(new Point(0 + direction.x * 0.5, 0 + direction.y * 0.5)); //p2
            allPoint1.add(new Point(RoiMat.width() - direction.x * 0.5,
                                    0 + direction.y * 0.5));//p3
            allPoint1.add(new Point(RoiMat.width() - direction.x * 0.5,
                                    RoiMat.height() - direction.y * 0.5)); //p4
            allPoint1.add(new Point(RoiMat.width(), RoiMat.height())); //p5
            allPoint1.add(new Point(RoiMat.width(), 0)); //p6
            mop1.fromList(allPoint1);
            allMatOfPoint.add(mop1);
            Imgproc.fillPoly(MaskTemplate, allMatOfPoint,  new Scalar(255, 255,
                             255)); //完成填充,area1為白色(255,255,255)

            //取到多邊形area1的mat
            Core.bitwise_and(RoiMat, RoiMat, dst, MaskTemplate);
            //area1明亮化處理
            dst.convertTo(dst, -1, 2, 50);
            //設為global value,在被貼處還需用到
            setEmbossArea1(dst);
            //area1-end

            //area2-start,找出area2的inner矩形部份
            Mat dst2 = new Mat();

            //inner矩形部份變成ROI
            Rect rectDst2 = new Rect(new Point(0 + direction.x * 0.5,
                                               0 + direction.y * 0.5), new Point(RoiMat.width() - direction.x * 0.5,
                                                       RoiMat.height() - direction.y * 0.5));
            //設為global value,在被貼處還需用到
            setInnerP1(getSrcX() + (int)direction.x * 0.5,
                       0 + getSrcY() + (int)direction.y * 0.5);
            setInnerP2(getSrcX() + RoiMat.width() - direction.x * 0.5,
                       getSrcY() + RoiMat.height() - direction.y * 0.5);
            //取到area2的inner矩形部份,且為Mat
            dst2 = RoiMat.submat(rectDst2);
            //設為global value,在被貼處還需用到
            setEmbossArea2(dst2);


            //area3-start,找出area1的左上斜邊,左下斜,右下斜邊
            Mat dst3 = new Mat();
            RoiMat = source.submat(roi);
            //先定義與ROI同size,全白單通道模板
            MaskTemplate = new Mat(RoiMat.size(), CvType.CV_8UC1, new Scalar(0));

            //多邊形fillPoly所需資料結構
            allMatOfPoint = new ArrayList<MatOfPoint>();
            MatOfPoint mop2 = new MatOfPoint();
            List<Point> allPoint2 = new ArrayList<Point>();
            //點集合的所有座標, area3的6個點,要循序
            allPoint2.add(new Point(0, 0)); //p1
            allPoint2.add(new Point(0 + direction.x * 0.5, 0 + direction.y * 0.5)); //p2
            allPoint2.add(new Point(0 + direction.x * 0.5,
                                    RoiMat.height() - direction.y * 0.5));//p3
            allPoint2.add(new Point(RoiMat.width() - direction.x * 0.5,
                                    RoiMat.height() - direction.y * 0.5)); //p4
            allPoint2.add(new Point(RoiMat.width(), RoiMat.height())); //p5
            allPoint2.add(new Point(0, RoiMat.height())); //p6
            mop2.fromList(allPoint2);
            allMatOfPoint.add(mop2);

            //完成填充,area3為白色(255,255,255)
            Imgproc.fillPoly(MaskTemplate, allMatOfPoint,  new Scalar(255, 255, 255));
            //取到多邊形area3的mat
            //Core.add(RoiMat, RoiMat, dst3,MaskTemplate);
            Core.bitwise_and(RoiMat, RoiMat, dst3, MaskTemplate);
            //area3灰暗化處理
            dst3.convertTo(dst3, -1, 0.5, -50);
            //設為global value,在被貼處還需用到
            setEmbossArea3(dst3);
            //area3-end

        } else {
            RoiMat = null;
        }
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

    public Mat getEmbossArea1()
    {
        return EmbossArea1;
    }

    public void setEmbossArea1(Mat embossArea1)
    {
        EmbossArea1 = embossArea1;
    }

    public Mat getEmbossArea2()
    {
        return EmbossArea2;
    }

    public void setEmbossArea2(Mat embossArea2)
    {
        EmbossArea2 = embossArea2;
    }

    public Mat getEmbossArea3()
    {
        return EmbossArea3;
    }

    public void setEmbossArea3(Mat embossArea3)
    {
        EmbossArea3 = embossArea3;
    }

    public Point getInnerP1()
    {
        return innerP1;
    }

    public void setInnerP1(double d, double e)
    {
        Point tmpPoint = new Point(d, e);
        this.innerP1 = tmpPoint;
    }

    public Point getInnerP2()
    {
        return innerP2;
    }

    public void setInnerP2(double d, double e)
    {
        Point tmpPoint = new Point(d, e);
        this.innerP2 = tmpPoint;
    }

}

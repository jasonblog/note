package ch07;

import java.awt.EventQueue;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.Collections;
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
import javax.swing.JButton;

public class Ch07_14_4SettingRoiByPolylineByBoundingRect
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmjavaSwing;

    public List<Point> allPoint = new ArrayList<Point>();
    private List<Integer> pointX = new ArrayList<Integer>();
    private List<Integer> pointY = new ArrayList<Integer>();


    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {

        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch07_14_4SettingRoiByPolylineByBoundingRect window = new
                    Ch07_14_4SettingRoiByPolylineByBoundingRect();
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
    public Ch07_14_4SettingRoiByPolylineByBoundingRect()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//DSC_0625.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("使用多邊形擷取為不規則ROI練習");
        frmjavaSwing.setBounds(100, 100, 922, 425);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblLocation = new JLabel("");
        lblLocation.setBounds(10, 10, 112, 25);
        frmjavaSwing.getContentPane().add(lblLocation);

        final JLabel lblLocation2 = new JLabel("");
        lblLocation2.setBounds(121, 10, 112, 25);
        frmjavaSwing.getContentPane().add(lblLocation2);

        JPanel panel = new JPanel();
        panel.setBounds(10, 45, 429, 348);
        frmjavaSwing.getContentPane().add(panel);

        final JLabel lblNewLabel = new JLabel("");
        panel.add(lblNewLabel);


        lblNewLabel.setIcon(new ImageIcon(image));

        JButton btnNewButton = new JButton("封閉化");
        btnNewButton.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                BufferedImage newImage = matToBufferedImage(paintAndSetPolyClose());
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnNewButton.setBounds(243, 10, 89, 25);
        frmjavaSwing.getContentPane().add(btnNewButton);

        final JLabel Label_R = new JLabel("");
        Label_R.setBounds(449, 45, 332, 343);
        frmjavaSwing.getContentPane().add(Label_R);

        JButton btnNewButton_1 = new JButton("擷取為不規則ROI");
        btnNewButton_1.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {

                Mat roi = getRoi(source);
                BufferedImage newImage = matToBufferedImage(roi);
                Label_R.setIcon(new ImageIcon(newImage));

            }
        });
        btnNewButton_1.setBounds(350, 10, 146, 25);
        frmjavaSwing.getContentPane().add(btnNewButton_1);



        lblNewLabel.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent arg0) {
                lblLocation2.setText("");
                lblLocation.setText("加入點:X:" + arg0.getX() + ",Y:" + arg0.getY());

                BufferedImage newImage = matToBufferedImage(paintPointAndPolylines(arg0.getX(),
                                         arg0.getY()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }

    public Mat paintPointAndPolylines(int x, int y)
    {
        Mat src = Imgcodecs.imread("C://opencv3.1//samples//DSC_0625.jpg");
        List<Integer> pointX = getPointX();
        List<Integer> pointY = getPointY();
        List<Point> allPoint = getAllPoint();
        allPoint.add(new Point(x, y));
        setAllPoint(allPoint);

        pointX.add(x);
        pointY.add(y);
        setPointX(pointX);
        setPointY(pointY);

        //畫滑鼠點擊的點
        Imgproc.circle(src, new Point(x, y), 2, new Scalar(0, 55, 255), 2);

        //畫出Polyline
        if (allPoint.size() >= 2) {
            MatOfPoint mop1 = new MatOfPoint();
            List<MatOfPoint> allMatOfPoint = new ArrayList<MatOfPoint>();
            mop1.fromList(allPoint);
            allMatOfPoint.add(mop1);
            Imgproc.polylines(src, allMatOfPoint, false, new Scalar(255, 255, 255), 2);

        }

        return src;

    }

    public Mat paintAndSetPolyClose()
    {
        Mat src = Imgcodecs.imread("C://opencv3.1//samples//DSC_0625.jpg");
        List<Point> allPoint = getAllPoint();

        //畫出Polyline
        if (allPoint.size() >= 2) {
            MatOfPoint mop1 = new MatOfPoint();
            List<MatOfPoint> allMatOfPoint = new ArrayList<MatOfPoint>();
            mop1.fromList(allPoint);
            allMatOfPoint.add(mop1);
            Imgproc.polylines(src, allMatOfPoint, true, new Scalar(255, 255, 255), 2);

        }

        return src;
    }

    public Mat getRoi(Mat src)
    {
        Mat RoiMat = new Mat();



        List<Point> allPoint = getAllPoint();
        List<Point> newPointSet = new ArrayList<Point>();
        // System.out.println("size="+allPoint.size());
        List<MatOfPoint> allMatOfPoint = new ArrayList<MatOfPoint>();
        MatOfPoint mop1 = new MatOfPoint();

        //準備使用Imgproc.boundingRect所需point資料結構
        mop1.fromList(allPoint);
        allMatOfPoint.add(mop1);
        Rect rect = Imgproc.boundingRect(allMatOfPoint.get(0));
        RoiMat = src.submat(rect);

        allMatOfPoint = new ArrayList<MatOfPoint>();
        //像photoshop的mask,0黑不作用
        Mat  MaskTemplate = new Mat(RoiMat.size(), CvType.CV_8UC1, new Scalar(0));


        //點集合的所有座標要重置,ROI的左上角座標是(rect.x,rect.y),全部都要減去左上以歸零
        Point tmpPoint;

        for (int i = 0; i < allPoint.size(); i++) {
            tmpPoint = allPoint.get(i);
            newPointSet.add(new Point(tmpPoint.x - rect.x, tmpPoint.y - rect.y));
        }

        //準備fillpoly所需資料
        mop1.fromList(newPointSet);
        allMatOfPoint.add(mop1);

        //使用fillPoly填充我們要的區域,255白色,為作用區
        Imgproc.fillPoly(MaskTemplate, allMatOfPoint,  new Scalar(255, 255, 255));

        //不規則的mat
        Mat dst = new Mat();
        Core.bitwise_and(RoiMat, RoiMat, dst, MaskTemplate);
        //用Core.add會較亮
        //Core.add(RoiMat, RoiMat, dst,MaskTemplate );
        //Core.bitwise_xor(RoiMat,  dst,MaskTemplate);
        return dst;
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



    public List<Point> getAllPoint()
    {
        return allPoint;
    }

    public void setAllPoint(List<Point> allPoint)
    {
        this.allPoint = allPoint;
    }

    public List<Integer> getPointX()
    {
        return pointX;
    }

    public void setPointX(List<Integer> pointX)
    {
        this.pointX = pointX;
    }

    public List<Integer> getPointY()
    {
        return pointY;
    }

    public void setPointY(List<Integer> pointY)
    {
        this.pointY = pointY;
    }
}

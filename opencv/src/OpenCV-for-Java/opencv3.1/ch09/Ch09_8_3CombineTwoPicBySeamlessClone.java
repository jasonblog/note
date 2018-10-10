package ch09;

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
import org.opencv.photo.Photo;

import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import javax.swing.JButton;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch09_8_3CombineTwoPicBySeamlessClone
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmjavaSwing;

    public List<Point> allPoint = new ArrayList<Point>();
    private List<Integer> pointX = new ArrayList<Integer>();
    private List<Integer> pointY = new ArrayList<Integer>();
    private int maxX, maxY, minX, minY;
    private Mat globalRoi;
    private Mat MaskRoi;

    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {

        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch09_8_3CombineTwoPicBySeamlessClone window = new
                    Ch09_8_3CombineTwoPicBySeamlessClone();
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
    public Ch09_8_3CombineTwoPicBySeamlessClone()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//DSC_0897.jpg");
        final Mat rightImg = Imgcodecs.imread("C://opencv3.1//samples//DSC_0230.jpg");

        BufferedImage image = matToBufferedImage(source);
        BufferedImage imageRight = matToBufferedImage(rightImg);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("使用多邊形擷取為不規則ROI複製到另一張影像");
        frmjavaSwing.setBounds(100, 100, 922, 482);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblLocation = new JLabel("");
        lblLocation.setBounds(10, 10, 112, 25);
        frmjavaSwing.getContentPane().add(lblLocation);

        final JLabel lblLocation2 = new JLabel("");
        lblLocation2.setBounds(121, 10, 112, 25);
        frmjavaSwing.getContentPane().add(lblLocation2);

        JPanel panel = new JPanel();
        panel.setBounds(10, 59, 200, 379);
        frmjavaSwing.getContentPane().add(panel);

        final JLabel lblNewLabel = new JLabel("");
        panel.add(lblNewLabel);


        lblNewLabel.setIcon(new ImageIcon(image));

        final JLabel label_Right = new JLabel("");
        label_Right.setBounds(430, 92, 449, 346);

        label_Right.setIcon(new ImageIcon(imageRight));
        frmjavaSwing.getContentPane().add(label_Right);

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

        final JLabel Label_mid = new JLabel("");
        Label_mid.setBounds(220, 106, 200, 242);
        frmjavaSwing.getContentPane().add(Label_mid);

        JButton btnNewButton_1 = new JButton("擷取為不規則ROI並貼上");
        btnNewButton_1.setBounds(342, 10, 175, 25);
        frmjavaSwing.getContentPane().add(btnNewButton_1);

        final JSlider sliderX = new JSlider();

        sliderX.setMaximum(465);
        sliderX.setBounds(577, 10, 248, 25);
        frmjavaSwing.getContentPane().add(sliderX);

        JLabel lblNewLabel_1 = new JLabel("X:");
        lblNewLabel_1.setBounds(542, 10, 34, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);

        final JSlider sliderY = new JSlider();

        sliderY.setMaximum(315);
        sliderY.setBounds(577, 45, 248, 25);
        frmjavaSwing.getContentPane().add(sliderY);

        JLabel lblY = new JLabel("Y:");
        lblY.setBounds(542, 45, 34, 15);
        frmjavaSwing.getContentPane().add(lblY);

        final JLabel showX_Val = new JLabel("50");
        showX_Val.setBounds(835, 10, 46, 15);
        frmjavaSwing.getContentPane().add(showX_Val);

        final JLabel showY_Val = new JLabel("50");
        showY_Val.setBounds(835, 45, 46, 15);
        frmjavaSwing.getContentPane().add(showY_Val);

        sliderX.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showX_Val.setText(sliderX.getValue() + "");

                Mat currentRightImg = Imgcodecs.imread("C://opencv3.1//samples//DSC_0230.jpg");
                //若是currentRightImg使用initialize()的rightImg會有疊圖現象
                Mat rightSideImg = pasteToAnother(sliderX.getValue(), sliderY.getValue(),
                                                  getGlobalRoi(), currentRightImg);
                BufferedImage   newImage = matToBufferedImage(rightSideImg);
                label_Right.setIcon(new ImageIcon(newImage));
            }
        });

        sliderY.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showY_Val.setText(sliderY.getValue() + "");

                Mat currentRightImg = Imgcodecs.imread("C://opencv3.1//samples//DSC_0230.jpg");
                Mat rightSideImg = pasteToAnother(sliderX.getValue(), sliderY.getValue(),
                                                  getGlobalRoi(), currentRightImg);
                BufferedImage   newImage = matToBufferedImage(rightSideImg);
                label_Right.setIcon(new ImageIcon(newImage));

            }
        });
        //Listener滑鼠點擊的點
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

        //點擊擷取為不規則ROI按鈕
        btnNewButton_1.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {

                Mat roi = getRoi(source);
                BufferedImage newImage = matToBufferedImage(roi);
                Label_mid.setIcon(new ImageIcon(newImage));

                //set global data
                setGlobalRoi(roi);


                //paste to another one,default position is (50,50)
                Mat rightSideImg = pasteToAnother(100, 100, roi, rightImg);
                //                      Mat src_mask=new Mat(roi.size(),roi.depth(),new Scalar(255));
                //                      Mat mixed_clone=new Mat();
                //                      Photo.seamlessClone(roi,rightImg, src_mask, new Point(150,150), mixed_clone,  Photo.MIXED_CLONE);




                newImage = matToBufferedImage(rightSideImg);
                label_Right.setIcon(new ImageIcon(newImage));

            }
        });
    }
    //繪出點及所有的線
    public Mat paintPointAndPolylines(int x, int y)
    {
        Mat src = Imgcodecs.imread("C://opencv3.1//samples//DSC_0897.jpg");
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

    //封閉不規則線
    public Mat paintAndSetPolyClose()
    {
        Mat src = Imgcodecs.imread("C://opencv3.1//samples//DSC_0897.jpg");
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
    //取到ROI
    public Mat getRoi(Mat src)
    {
        Mat RoiMat = new Mat();
        List<Integer> pointX = getPointX();
        List<Integer> pointY = getPointY();


        //找出所有點集合的min及max,以利建立ROI
        int X0, Y0, X1, Y1, maxX, maxY, minX, minY;
        maxX = Collections.max(pointX);
        maxY = Collections.max(pointY);
        minX = Collections.min(pointX);
        minY = Collections.min(pointY);

        //貼至大圖還需要再用到
        setMaxX(maxX);
        setMinX(minX);
        setMaxY(maxY);
        setMinY(minY);


        //那些點可建立的矩形ROI
        Rect rect = new Rect(new Point(minX, minY), new Point(maxX, maxY));
        RoiMat = src.submat(rect);

        //像photoshop的mask,0黑不作用
        Mat  MaskTemplate = new Mat(RoiMat.size(), CvType.CV_8UC1, new Scalar(0));

        List<Point> allPoint = getAllPoint();
        List<Point> newPointSet = new ArrayList<Point>();
        // System.out.println("size="+allPoint.size());
        List<MatOfPoint> allMatOfPoint = new ArrayList<MatOfPoint>();
        MatOfPoint mop1 = new MatOfPoint();
        Point tmpPoint;

        //點集合的所有座標要重置,ROI的左上角座標是(minX,minY),全部都要減去左上以歸零
        for (int i = 0; i < allPoint.size(); i++) {
            tmpPoint = allPoint.get(i);
            newPointSet.add(new Point(tmpPoint.x - minX, tmpPoint.y - minY));
        }

        mop1.fromList(newPointSet);
        allMatOfPoint.add(mop1);
        //使用fillPoly填充我們要的區域,255白色,為作用區
        Imgproc.fillPoly(MaskTemplate, allMatOfPoint,  new Scalar(255, 255, 255));

        //將mask資料設定為global,之後用到
        setMaskRoi(MaskTemplate);

        //不規則的mat
        Mat dst = new Mat();
        Core.bitwise_and(RoiMat, RoiMat, dst, MaskTemplate);
        //用Core.add會較亮
        //Core.add(RoiMat, RoiMat, dst,MaskTemplate );
        return dst;
    }


    //ROI貼至另一張影像,roi是不規則子影像,anotherImg是預被貼上的另一張影像,x,y是該座標
    public Mat pasteToAnother(int x, int y, Mat roi, Mat anotherImg)
    {


        Mat src_mask = getMaskRoi();
        Mat mixed_clone = new Mat();

        try {
            Photo.seamlessClone(roi, anotherImg, src_mask, new Point(x, y), mixed_clone,
                                Photo.NORMAL_CLONE);

        } catch (Exception e) {
            e.printStackTrace();
        }

        return mixed_clone;

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

    public int getMaxX()
    {
        return maxX;
    }

    public void setMaxX(int maxX)
    {
        this.maxX = maxX;
    }

    public int getMaxY()
    {
        return maxY;
    }

    public void setMaxY(int maxY)
    {
        this.maxY = maxY;
    }

    public int getMinX()
    {
        return minX;
    }

    public void setMinX(int minX)
    {
        this.minX = minX;
    }

    public int getMinY()
    {
        return minY;
    }

    public void setMinY(int minY)
    {
        this.minY = minY;
    }

    public Mat getGlobalRoi()
    {
        return globalRoi;
    }

    public void setGlobalRoi(Mat globalRoi)
    {
        this.globalRoi = globalRoi;
    }

    public Mat getMaskRoi()
    {
        return MaskRoi;
    }

    public void setMaskRoi(Mat maskRoi)
    {
        MaskRoi = maskRoi;
    }
}

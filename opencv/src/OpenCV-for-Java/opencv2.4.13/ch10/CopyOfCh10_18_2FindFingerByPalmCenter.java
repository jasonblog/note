package ch10;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfInt;
import org.opencv.core.MatOfInt4;
import org.opencv.core.MatOfPoint;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.Range;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.highgui.VideoCapture;
import org.opencv.imgproc.Imgproc;
import org.opencv.core.CvType;
public class CopyOfCh10_18_2FindFingerByPalmCenter
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    static ArrayList<Double> HullList = new ArrayList<Double>();

    public static void main(String arg[]) throws Exception{
        JFrame frame1 = new JFrame("Camera");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);
        JFrame frame2 = new JFrame("Threshold");
        frame2.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame2.setSize(640, 480);
        frame2.setBounds(300, 100, frame1.getWidth() + 50, 50 + frame1.getHeight());
        Panel panel4 = new Panel();
        frame2.setContentPane(panel4);
        frame2.setVisible(true);
        //-- 2. Read the video stream
        VideoCapture capture = new VideoCapture(0);
        Mat webcam_image = new Mat();
        Mat hsv_image = new Mat();
        Mat thresholded = new Mat();
        capture.read(webcam_image);
        frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);
        frame2.setSize(webcam_image.width() + 40, webcam_image.height() + 60);
        Mat array255 = new Mat(webcam_image.height(), webcam_image.width(), CvType.CV_8UC1);
        array255.setTo(new Scalar(255));
        Scalar hsv_min = new Scalar(0, 10, 60);
        Scalar hsv_max = new Scalar(20, 150, 255);

        Mat firstArea = webcam_image.colRange(new Range(10, 320)).rowRange(70, 430);

        if (capture.isOpened())
        {
            while (true) {
                Thread.sleep(500);
                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    Imgproc.cvtColor(firstArea, hsv_image, Imgproc.COLOR_BGR2HSV);
                    Core.inRange(hsv_image, hsv_min, hsv_max, thresholded);
                    Imgproc.GaussianBlur(thresholded, thresholded, new Size(15, 15), 0, 0);
                    Mat threshold_output = new Mat(webcam_image.rows(), webcam_image.cols(),
                                                   CvType.CV_8UC1);
                    Imgproc.threshold(thresholded, threshold_output, 50, 255,
                                      Imgproc.THRESH_BINARY);
                    //Mat element = Imgproc.getStructuringElement(Imgproc.MORPH_ELLIPSE,new Size(9, 9));
                    //Imgproc.dilate(threshold_output,threshold_output, element);

                    List<MatOfPoint> contours = new ArrayList<MatOfPoint>();
                    Mat hierarchy = new Mat(webcam_image.rows(), webcam_image.cols(),
                                            CvType.CV_8UC1, new Scalar(0));

                    Imgproc.findContours(threshold_output, contours, hierarchy, Imgproc.RETR_LIST,
                                         Imgproc.CHAIN_APPROX_SIMPLE);
                    //Imgproc.findContours(threshold_output, contours, hierarchy, Imgproc.RETR_TREE, Imgproc.CHAIN_APPROX_SIMPLE);
                    int s = findBiggestContour(contours);
                    //Mat drawing = Mat::zeros( src.size(), CV_8UC1 );
                    Mat drawing = new Mat(threshold_output.size(), CvType.CV_8UC1);
                    Imgproc.drawContours(drawing, contours, s, new Scalar(255, 0, 0, 255), -1);
                    int count = 0;
                    //for計算convexHull




                    Imgproc.findContours(drawing, contours, hierarchy, Imgproc.RETR_TREE,
                                         Imgproc.CHAIN_APPROX_SIMPLE);
                    Mat destination1 = new Mat();
                    Imgproc.distanceTransform(drawing, destination1, Imgproc.CV_DIST_L2, 3);
                    int temp = 0;
                    //半徑
                    int R = 0;

                    //圓心
                    int centerX = 0;
                    int centerY = 0;

                    //距離
                    double d = 0;

                    try {
                        for (int i = 0; i < webcam_image.rows(); i++) {
                            for (int j = 0; j < webcam_image.cols(); j++) {
                                //檢查該點是否在輪廓內,計算點到輪廓邊界距離


                                //d=Imgproc.pointPolygonTest(new MatOfPoint2f(contours.get(0).toArray()), new Point(j,i), true);
                                //System.out.println("d="+d);
                                //if(d>0){
                                temp = (int) destination1.get(i, j)[0];

                                if (temp > R) {
                                    R = temp;
                                    centerX = j;
                                    centerY = i;
                                }

                                //}
                            }
                        }

                        System.out.println("1centerX=" + centerX + ",centerY=" + centerY + ",R=" + R);
                        Core.circle(webcam_image, new Point(centerX, centerY), R, new Scalar(255, 255,
                                    255));
                        Core.line(webcam_image, new Point(centerX, centerY), new Point(centerX,
                                  centerY), new Scalar(255, 255, 255), 5);
                        Core.normalize(destination1, destination1, 0, 255, Core.NORM_MINMAX);

                    } catch (Exception e) {
                        // TODO: handle exception
                    }


                    //              System.out.println("centerX="+centerX+",centerY="+centerY+",R="+R);
                    //              Core.circle(webcam_image, new Point(centerX,centerY), R, new Scalar(255, 255, 255),5);
                    //              Core.line(webcam_image, new Point(centerX,centerY), new Point(centerX,centerY), new Scalar(255, 255, 255),5);
                    //              Core.normalize(destination1, destination1,0,255,Core.NORM_MINMAX);


                    /**
                       for(int i=0;i<contours.size();i++){
                           MatOfInt4 mConvexityDefectsMatOfInt4=new MatOfInt4();
                           MatOfInt hull = new MatOfInt();
                              MatOfPoint tempContour=contours.get(i);
                              Imgproc.convexHull(tempContour, hull,false);
                              //System.out.println("hull1:"+hull.size());

                            int index=(int)hull.get(((int)hull.size().height)-1,0)[0];
                            Point pt, pt0 = new Point(tempContour.get(index, 0)[0], tempContour.get(index, 0)[1]);
                            for(int j = 0; j < hull.size().height -1 ; j++){
                            //for(int j = 0; j < 3 ; j++){
                                index = (int) hull.get(j, 0)[0];
                                pt = new Point(tempContour.get(index, 0)[0], tempContour.get(index, 0)[1]);

                             double distance1=Math.sqrt(Math.pow(pt0.x-pt.x, 2)+Math.pow(pt0.y-pt.y, 2));

                           //剃除較短者
                             if   (distance1>=30){
                                     Core.line(firstArea, pt0, pt, new Scalar(255, 0, 100), 2);//2是厚度

                                 HullList.add(distance1);

                             }
                               // System.out.println("distance-hull-"+j+":" + distance);
                                pt0 = pt;
                            }
                            //for計算convexityDefects
                            if(contours.size()>0 && hull.rows()>3){
                               Imgproc.convexityDefects(tempContour, hull, mConvexityDefectsMatOfInt4);


                                //System.out.println( mConvexityDefectsMatOfInt4.height());
                                int index2=(int)mConvexityDefectsMatOfInt4.toArray().length;
                               //System.out.println( "index2="+index2);
                                int index3;











                             if (tempContour.get(index2, 0)!=null) {
                                Point pt3,pt1,old_pt1=null,old_pt3 = null;
                                for(int j = 0; j < mConvexityDefectsMatOfInt4.size().height -1 ; j++){
                                    index2 = (int) mConvexityDefectsMatOfInt4.get(j, 0)[0];//0是top即指尖,2是down,兩指之底
                                    index3 = (int) mConvexityDefectsMatOfInt4.get(j, 0)[2];
                                    pt1 = new Point(tempContour.get(index2, 0)[0], tempContour.get(index2, 0)[1]);//尖
                                    pt3 = new Point(tempContour.get(index3, 0)[0], tempContour.get(index3, 0)[1]);//底

                                    Core.line(firstArea, pt1, pt1, new Scalar(0, 255, 0), 9);//尖dot,綠
                                    Core.line(firstArea, pt3, pt3, new Scalar(150, 155, 255), 12);//底dot,粉紅

                                    Core.line(firstArea, pt1, pt3, new Scalar(0, 0, 255), 2);//紅,姆指左

                                   //
                                   double distance1=Math.sqrt(Math.pow(pt1.x-pt3.x, 2)+Math.pow(pt1.y-pt3.y, 2));

                                   if(old_pt3!=null&&old_pt1!=null){
                                        Core.line(firstArea, pt1, old_pt3, new Scalar(200, 0, 200), 2);//桃紅,姆指右
                                        Core.putText(firstArea, getAngle(pt1,old_pt3,old_pt1)+"", old_pt3, 0, 0.3, new Scalar(0, 0, 0));//顯示指底角度
                                       double distance2=Math.sqrt(Math.pow(pt1.x-old_pt3.x, 2)+Math.pow(pt1.y-old_pt3.y, 2));
                                       if (distance2>15){
                                           //count++;
                                           Core.line(firstArea, pt1, old_pt3, new Scalar(0, 0, 255), 2);
                                       }
                                        if (distance1>15){
                                           //count++;
                                           Core.line(firstArea, pt1, pt3, new Scalar(220, 0, 255), 2);
                                        }
                                       //1個拇指有長距離但沒有角度,握拳有短距離但也沒有角度.
                                        //pt1:指尖
                                        //pt3:指底
                                        //pt4:前1個的指底
                                        //old_pt1:前1個的指尖
                                        //count算幾個指底:4-->5指,3-->4指,2-->3指,1-->2指,0-->1指,0-->石頭




















                                        //若有角度
                                        if (getAngle(pt1,old_pt3,old_pt1)>20 && getAngle(pt1,old_pt3,old_pt1)<84){
                                            //距離太短所構成的角度不算
                                            if (getDistance(pt1,old_pt3)>25 && getDistance(old_pt3,old_pt1)>25){
                                           // System.out.println("L1:"+getDistance(pt1,pt4)+",L2:"+getDistance(old_pt1,pt4));
                                            count++;
                                           //}else if (getDistance(pt1,pt4)<25 && getDistance(pt4,old_pt1)<25){
                                               //System.out.println("==================qq總共0姆指=====================");
                                               //count=count-1;
                                           }

                                        }

                                   }

                                   if (count>0){
                                       System.out.println("==================總共"+((count+1)>5?5:(count+1))+"姆指=====================");//1~5準確
                                   }else {
                                        if (HullList.size()>=1 ){
                                             Collections.sort(HullList);
                                              if (HullList.get(HullList.size()-1)>80 && HullList.get(HullList.size()-1)<150){
                                                  System.out.println(count+"==================總共0姆指====================="+HullList.get(HullList.size()-1));//1~5準確
                                              }else if (HullList.get(HullList.size()-1)>150 ){
                                                  System.out.println(count+"==================總共1姆指====================="+HullList.get(HullList.size()-1));//1~5準確
                                       }
                                            HullList.clear();
                                   }
                                   }
                                    old_pt3 = pt3;
                                    old_pt1=pt1;
                                }

                             }

                            }

                          }
                       **/
                    /**
                     count=0;
                       **/
                    //繪出手掌判斷幾個手指頭感興趣區域,矩形
                    Core.rectangle(webcam_image, new Point(30, 90), new Point(300, 420),
                                   new Scalar(250, 250, 250), 3);
                    //繪出掌心置放區,以十字表示
                    Core.line(webcam_image, new Point(130, 320), new Point(180, 320),
                              new Scalar(250, 250, 250), 2); //橫
                    Core.line(webcam_image, new Point(155, 295), new Point(155, 345),
                              new Scalar(250, 250, 250), 2); //豎


                    //-- 5. Display the image
                    panel1.setimagewithMat(webcam_image);
                    panel4.setimagewithMat(drawing);
                    frame1.repaint();
                    frame2.repaint();
                } else {
                    System.out.println(" --(!) No captured frame -- Break!");
                    break;
                }
            }
        }

        return;
    }
    public static int findBiggestContour(List<MatOfPoint> contours)
    {
        int indexOfBiggestContour = -1;
        int sizeOfBiggestContour = 0;

        for (int i = 0; i < contours.size(); i++) {
            if (contours.get(i).height() > sizeOfBiggestContour) {
                sizeOfBiggestContour = contours.get(i).height();
                indexOfBiggestContour = i;
            }
        }

        return indexOfBiggestContour;
    }

    public static double getAngle(Point a, Point b, Point c)
    {
        double line1 = Math.sqrt(Math.pow(a.x - b.x, 2) + Math.pow(a.y - b.y, 2));
        double line2 = Math.sqrt(Math.pow(c.x - b.x, 2) + Math.pow(c.y - b.y, 2));
        double dot = (a.x - b.x) * (c.x - b.x) + (a.y - b.y) * (c.y - b.y);
        double angle = Math.acos(dot / (line1 * line2));
        angle = angle * 180 / Math.PI;
        return Math.round(100 * angle) / 100;

    }
    public static double getDistance(Point a, Point b)
    {
        //return Math.round(Math.sqrt(Math.pow(a.x-b.x, 2)+Math.pow(a.y-b.y, 2)*100)/100);
        return Math.sqrt(Math.pow(a.x - b.x, 2) + Math.pow(a.y - b.y, 2));
    }



}

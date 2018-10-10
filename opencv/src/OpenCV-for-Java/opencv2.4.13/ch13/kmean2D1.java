package ch13;

import java.util.Random;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Range;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;
import org.opencv.highgui.Highgui;

public class kmean2D1
{
    public static void main(String[] args) throws Exception{
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);

        //1D的

        int MAX_CLUSTERS = 5;

        //5組
        Scalar colorTab[] =
        {
            new Scalar(0, 0, 255),
            new Scalar(0, 255, 0),
            new Scalar(255, 100, 100),
            new Scalar(255, 0, 255),
            new Scalar(0, 255, 255)
        };
        double[] data1 = new double[1];
        double[] data2 = new double[1];
        double[] data3 = new double[2];
        Mat img = new Mat(500, 500, CvType.CV_8UC3);
        //Random rand = new Random();

        //
        //  System.out.println(clusterCount.dump());
        //System.out.println(clusterCount.dump());






        Mat mat1 = new Mat(4, 4, CvType.CV_8UC1);
        Mat mat2 = new Mat(4, 4, CvType.CV_8UC1);
        Mat mat3 = new Mat(4, 4, CvType.CV_8UC2);

        Core.randn(mat1, 223, 21);
        Core.randn(mat2, 23, 9);

        //Core.randn(mat3, 123, 124);
        // data1=mat1.get(3, 3);
        //System.out.println( data[0]+","+data[1]+","+data[2]);
        // System.out.println("1st="+data1[0]);
        System.out.println("mat1=" + mat1.dump());
        System.out.println("mat2=" + mat2.dump());

        // Mat points=new Mat(4,4, CvType.CV_32FC1);
        //Core.randn(points, 123, 1200);
        //       mat1=mat1.reshape(16,1);
        //       mat2=mat2.reshape(16,1);
        //       data=mat1.get(1,0);
        //       System.out.println( "2nd="+data[0]);

        // System.out.println("mat1="+mat1.dump());
        // System.out.println("mat2="+mat2.dump());


        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++) {
                data1 = mat1.get(i, j);
                data2 = mat2.get(i, j);
                data3[0] = data1[0];
                data3[1] = data2[0];

                mat3.put(i, j, data3);
                //mat3.put(i, j, data2[1]);
                // System.out.println( data1[0]);
                // System.out.println( data2[0]);
            }

        }

        System.out.println("mat3=" + mat3.dump());

        Mat samples = mat3.reshape(2, mat1.cols() * mat1.rows());//轉2行多列的mat
        Mat points = new Mat();//for kmeans使用32f
        //samples.convertTo(points, CvType.CV_32F, 1.0 / 255.0);//資料轉換成kmean使用
        samples.convertTo(points, CvType.CV_32F);//資料轉換成kmean使用
        System.out.println("points0=" + points.dump());

        Mat labels = new Mat();
        TermCriteria criteria = new TermCriteria(TermCriteria.COUNT, 10, 1);
        Mat centers = new Mat(2, 1, points.type());
        //Core.kmeans(points, 2, labels, criteria, 3, Core.KMEANS_PP_CENTERS);
        Core.kmeans(points, 3, labels, criteria, 10, Core.KMEANS_RANDOM_CENTERS);

        System.out.println("points1=" + points.dump());
        System.out.println("points-size=" + points.total());
        System.out.println("points-col=" + points.cols());
        System.out.println("points-row=" + points.rows());
        System.out.println("centers=" + centers.dump());
        System.out.println("labels=" + labels.dump());
        //draw

        Mat draw = new Mat(300, 500, CvType.CV_8UC3, new Scalar(255, 255, 255));

        for (int i = 0; i < points.rows(); i++)
        {
            //for(int j=0;j<points.cols();j++){

            //System.out.println("i="+points.get( i,0)[0]);
            //          data3=mat3.get( i,0);
            data2 = labels.get(i, 0);
            Core.circle(draw, new Point(points.get(i, 0)[0], points.get(i, 0)[1]), 1,
                        colorTab[(int)data2[0]], 2);//畫出所有的點(用顏色分類)
            //Core.circle(draw, new Point((int)(data1[0]*1000),100), 2, colorTab[(int)data2[0]], 2);//畫出所有的點(用顏色分類)
            //Core.circle(draw, new Point((int)(data1[0]*1000),100), 2, new Scalar(255,0,255), 2);
            //System.out.println("i="+(int)(data1[0]*200));

        }

        /**
        Core.line(draw, new Point(0,103), new Point(1000,102),  new Scalar(0,0,0), 1);//畫出橫線(1D用)
        double[] centerData1=centers.get( 0,0);
        System.out.println("centerData1="+(centerData1[0]*1000));
        //Core.circle(draw, new Point((int)(centerData1[0]*1000),103), 4, colorTab[0], 2);//畫出中心點
        **/


        Highgui.imwrite("C://opencv3.1//kmeans-test-2d.jpg", draw);

    }
}

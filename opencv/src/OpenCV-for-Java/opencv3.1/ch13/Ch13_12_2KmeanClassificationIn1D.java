package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

public class Ch13_12_2KmeanClassificationIn1D
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args) throws Exception{
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

        System.out.println("Core.KMEANS_RANDOM_CENTERS=" + Core.KMEANS_RANDOM_CENTERS);
        System.out.println("Core.KMEANS_PP_CENTERS=" + Core.KMEANS_PP_CENTERS);

        Mat mat1 = new Mat(4, 4, CvType.CV_8UC1);
        Mat mat2 = new Mat(4, 4, CvType.CV_8UC1);
        Mat mat3 = new Mat(4, 8, CvType.CV_8UC1);

        Core.randn(mat1, 223, 21);
        Core.randn(mat2, 23, 9);

        System.out.println("mat1=" + mat1.dump());
        System.out.println("mat2=" + mat2.dump());



        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++) {
                data1 = mat1.get(i, j);
                data2 = mat2.get(i, j);
                //data3[0]=data1[0];

                mat3.put(i, j, data1[0]);
                mat3.put(i, j + 4, data2[0]);
                // System.out.println( data1[0]);
                // System.out.println( data2[0]);
            }

        }

        System.out.println("mat3=" + mat3.dump());

        Mat samples = mat3.reshape(1, mat1.cols() * mat1.rows() + mat2.cols() * mat2.rows()); //轉1行多列的mat
        Mat points = new Mat();//for kmeans使用32f
        //samples.convertTo(points, CvType.CV_32F, 1.0 / 255.0);//資料轉換成kmean使用
        samples.convertTo(points, CvType.CV_32F);//資料轉換成kmean使用
        System.out.println("points0=" + points.dump());

        Mat labels = new Mat();
        TermCriteria criteria = new TermCriteria(TermCriteria.COUNT, 10, 1);
        Mat centers = new Mat(2, 1, points.type());

        //可以試著用3類,4類...
        //Core.kmeans(points, 3, labels, criteria, 3, Core.KMEANS_PP_CENTERS);
        Core.kmeans(points, 2, labels, criteria, 10, Core.KMEANS_RANDOM_CENTERS, centers);

        System.out.println("points1=" + points.dump());
        System.out.println("points-size=" + points.total());
        System.out.println("points-col=" + points.cols());
        System.out.println("points-row=" + points.rows());
        System.out.println("centers=" + centers.dump());
        System.out.println("labels=" + labels.dump());
        //draw
        Mat draw = new Mat(200, 1000, CvType.CV_8UC3, new Scalar(255, 255, 255));

        for (int i = 0; i < points.rows(); i++)
        {
            //System.out.println("i="+i);
            data1 = samples.get(i, 0);
            data2 = labels.get(i, 0);
            Imgproc.circle(draw, new Point(data1[0] * 2, 100), 2, colorTab[(int)data2[0]],
                           2);//畫出所有的點(用顏色分類)

        }

        Imgproc.line(draw, new Point(0, 103), new Point(1000, 102),  new Scalar(0, 0, 0), 1); //畫出橫線(1D用)
        double[] centerData1 = centers.get(0, 0);
        System.out.println("centerData1=" + (centerData1[0] * 1000));
        //Core.circle(draw, new Point((int)(centerData1[0]*1000),103), 4, colorTab[0], 2);//畫出中心點

        Imgcodecs.imwrite("C://opencv3.1//samples//kmeans-caseIn1D.jpg", draw);

    }
}

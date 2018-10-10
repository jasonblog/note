package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;

public class PCA
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        /**
         Mat data = new Mat(3, 4, CvType.CV_32F) {
             {
                 put(0, 0, 1, 2, 2, 4);
                 put(1, 0, 2, 4, 4, 8);
                 put(2, 0, 3, 6, 6, 12);
             }
          };

         Mat data = new Mat(4, 4, CvType.CV_32F) {
             {
                 put(0, 0, -1, 1, 0, 5);
                 put(1, 0, 3, -4, 3, 0);
                 put(2, 0, 2, 1, 0, 2);
                 put(3, 0, 1, 2, 3, 2);
             }
          };
          **/
        Mat data = new Mat(4, 4, CvType.CV_32F) {
            {
                put(0, 0, -1, 1, 0, 5);
                put(1, 0, 3, -4, 3, 0);
                put(2, 0, 2, 1, 0, 2);
                put(3, 0, 1, 2, 3, 2);
            }
        };

        Mat mean = new Mat();
        Mat vectors = new Mat();

        Core.PCACompute(data, mean, vectors);

        System.out.println("mean" + mean.dump());
        System.out.println("vectors" + vectors.dump());

        Mat mean_prj = new Mat();
        Mat eigenvectors = new Mat();
        Mat result = new Mat();

        //Core.PCABackProject(data, mean_prj, eigenvectors, result);
        //System.out.println("mean_prj"+mean_prj.dump());
        // System.out.println("eigenvectors"+eigenvectors.dump());
        //System.out.println("result"+result.dump());

        //Core.PCAProject(data, mean_prj, eigenvectors, result);
        //System.out.println("mean_prj"+mean_prj.dump());
        // System.out.println("eigenvectors"+eigenvectors.dump());
        //System.out.println("result"+result.dump());


        /**

           Mat mean_truth = new Mat(1, 4, CvType.CV_32F) {
               {
                   put(0, 0, 2, 4, 4, 8);
               }
           };
           Mat vectors_truth = new Mat(3, 4, CvType.CV_32F, new Scalar(0)) {
               {
                   put(0, 0, 0.2, 0.4, 0.4, 0.8);
               }
           };
         **/
        // System.out.println( mean.dump());
        //      assertMatEqual(mean_truth, mean, EPS);
        //      assertMatEqual(vectors_truth, vectors, EPS);
    }

    public void test()
    {

    }

}

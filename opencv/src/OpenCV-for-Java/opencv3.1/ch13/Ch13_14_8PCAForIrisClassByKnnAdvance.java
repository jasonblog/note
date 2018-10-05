package ch13;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.ml.KNearest;
import org.opencv.ml.Ml;

public class Ch13_14_8PCAForIrisClassByKnnAdvance
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        IrisDatabase iris = new IrisDatabase();
        KNearest knn = KNearest.create();

        //1,3-->100%,2,5,6,7,8-->97.77,4-->95.5%,6
        knn.setDefaultK(8);
        knn.setIsClassifier(true);
        //knn.setAlgorithmType(knn.KDTREE);
        knn.setAlgorithmType(knn.BRUTE_FORCE);
        knn.setEmax(1);


        Mat mean  = new Mat();
        Mat vectors = new Mat();
        Mat IrisDataT = iris.getAllDataMat().t();

        Core.PCACompute(IrisDataT, mean, vectors, 2);
        Mat pcaAllIris = vectors.t();

        //取0~105列做為train
        Mat subMatTrainPcaIris = pcaAllIris.submat(0, 105, 0, 2);

        //取105~150列做為test
        Mat subMatTestPcaIris = pcaAllIris.submat(105, 150, 0, 2);


        boolean r = knn.train(subMatTrainPcaIris, Ml.ROW_SAMPLE,
                              iris.getTrainingLabelsMat());
        System.out.println("是否有訓練成功=" + r);


        //測試精準值

        //預測正確累加1
        int right = 0;
        float result;
        float[] answer = iris.getTestingLabels();

        //System.out.println("組數"=iris.getTestingDataMat().rows());
        for (int i = 0; i < subMatTestPcaIris.rows(); i++) {

            //System.out.println(iris.getTestingDataMat().row(i).dump());
            result = knn.predict(subMatTestPcaIris.row(i));

            if (result == answer[i]) {
                right++;
            } else {
                System.out.println("預測錯誤!KNN預測是" + result + "正確是=" + answer[i]);
            }

        }

        System.out.println("KNN測試精準值=" + ((float)right / (float)
                                               iris.getTestingDataMat().rows()) * 100 + "%");
    }

}

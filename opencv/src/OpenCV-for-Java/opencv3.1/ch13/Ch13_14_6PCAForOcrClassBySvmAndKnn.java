package ch13;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;
import org.opencv.ml.KNearest;
import org.opencv.ml.Ml;
import org.opencv.ml.SVM;

public class Ch13_14_6PCAForOcrClassBySvmAndKnn
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        OcrDatabase ocr = new OcrDatabase();


        Mat meanTrain  = new Mat();
        Mat vectorsTrain = new Mat();
        Mat trainOcrDataT = ocr.getTrainingDataMat().t();

        Core.PCACompute(trainOcrDataT, meanTrain, vectorsTrain, 10);
        Mat pcaTrainOcr = vectorsTrain.t();


        SVM svm = SVM.create();
        TermCriteria criteria = new TermCriteria(TermCriteria.MAX_ITER, 100, 1e-6);
        svm.setKernel(svm.LINEAR);
        svm.setType(svm.C_SVC);
        svm.setGamma(0.5);
        svm.setNu(0.5);
        //1-->97.77778%,2-->100%
        svm.setC(2);
        svm.setTermCriteria(criteria);

        boolean r = svm.train(pcaTrainOcr, Ml.ROW_SAMPLE, ocr.getTrainingLabelsMat());
        System.out.println("SVM是否有訓練成功=" + r);

        /////////////////////////////////////////////////////////////////////////////////


        KNearest knn = KNearest.create();

        knn.setDefaultK(8);
        knn.setIsClassifier(true);
        //knn.setAlgorithmType(knn.KDTREE);
        knn.setAlgorithmType(knn.BRUTE_FORCE);
        knn.setEmax(1);

        boolean Knnr = knn.train(pcaTrainOcr, Ml.ROW_SAMPLE,
                                 ocr.getTrainingLabelsMat());
        System.out.println("KNN是否有訓練成功=" + r);



        Mat meanTest  = new Mat();
        Mat vectorsTest = new Mat();
        Mat testIrisDataT = ocr.getTestingDataMat().t();
        Core.PCACompute(testIrisDataT, meanTest, vectorsTest, 10);

        Mat pcaTestOcr = vectorsTest.t();
        //System.out.println(pcaTestOcr.dump());

        //測試精準值

        //預測正確累加1
        int rightSVM = 0;
        float resultSVM;

        int rightKnn = 0;
        float resultKnn;

        float[] answer = ocr.getTestingLabels();

        //System.out.println("組數"=iris.getTestingDataMat().rows());
        for (int i = 0; i < pcaTestOcr.rows(); i++) {

            //System.out.println(iris.getTestingDataMat().row(i).dump());
            resultSVM = svm.predict(pcaTestOcr.row(i));

            if (resultSVM == answer[i]) {
                rightSVM++;
            } else {
                System.out.println("預測錯誤!支援向量機預測是" + resultSVM + ",正確是=" +
                                   answer[i]);
            }

            //////////////////////////////////////
            resultKnn = knn.predict(pcaTestOcr.row(i));

            if (resultKnn == answer[i]) {
                rightKnn++;
            } else {
                System.out.println("預測錯誤!KNN預測是" + resultKnn + ",正確是=" + answer[i]);
            }

        }

        System.out.println("///////////////////////////////////////////////////////////");

        System.out.println("支援向量機測試精準值=" + ((float)rightSVM /
                           (float)pcaTestOcr.rows()) * 100 + "%");
        System.out.println("KNN測試精準值=" + ((float)rightKnn /
                                               (float)pcaTestOcr.rows()) * 100 + "%");
    }

}

package ch13;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;
import org.opencv.ml.KNearest;
import org.opencv.ml.Ml;
import org.opencv.ml.SVM;

public class Ch13_14_7PCAForOcrClassBySvmAndKnnAdvance
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        OcrDatabase ocr = new OcrDatabase();

        Mat allDataT = ocr.getAllDataMat().t();

        Mat meanAll  = new Mat();
        Mat vectorsAll = new Mat();


        Core.PCACompute(allDataT, meanAll, vectorsAll, 6);
        Mat allPcaOcr = vectorsAll.t();

        //取0~99列做為train
        Mat subMatTrainPcaOcr = allPcaOcr.submat(0, 100, 0, 6);

        //取100~110列做為test
        Mat subMatTestPcaOcr = allPcaOcr.submat(100, 110, 0, 6);
        //System.out.println(subMatTrainPcaOcr.size());


        SVM svm = SVM.create();
        TermCriteria criteria = new TermCriteria(TermCriteria.MAX_ITER, 100, 1e-6);
        svm.setKernel(svm.LINEAR);
        svm.setType(svm.C_SVC);
        svm.setGamma(0.5);
        svm.setNu(0.5);
        //1-->97.77778%,2-->100%
        svm.setC(2);
        svm.setTermCriteria(criteria);

        boolean r = svm.train(subMatTrainPcaOcr, Ml.ROW_SAMPLE,
                              ocr.getTrainingLabelsMat());
        System.out.println("SVM是否有訓練成功=" + r);
        //////////////////////////////////////////////////////

        KNearest knn = KNearest.create();

        knn.setDefaultK(8);
        knn.setIsClassifier(true);
        //knn.setAlgorithmType(knn.KDTREE);
        knn.setAlgorithmType(knn.BRUTE_FORCE);
        knn.setEmax(1);

        boolean Knnr = knn.train(subMatTrainPcaOcr, Ml.ROW_SAMPLE,
                                 ocr.getTrainingLabelsMat());
        System.out.println("KNN是否有訓練成功=" + r);




        //測試精準值

        //預測正確累加1
        int rightSVM = 0;
        float resultSVM;

        int rightKnn = 0;
        float resultKnn;

        float[] answer = ocr.getTestingLabels();

        //System.out.println("組數"=iris.getTestingDataMat().rows());
        for (int i = 0; i < subMatTestPcaOcr.rows(); i++) {

            //System.out.println(iris.getTestingDataMat().row(i).dump());
            resultSVM = svm.predict(subMatTestPcaOcr.row(i));

            if (resultSVM == answer[i]) {
                rightSVM++;
            } else {
                System.out.println("預測錯誤!支援向量機預測是" + resultSVM + ",正確是=" +
                                   answer[i]);
            }

            //////////////////////////////////////////
            resultKnn = knn.predict(subMatTestPcaOcr.row(i));

            if (resultKnn == answer[i]) {
                rightKnn++;
            } else {
                System.out.println("預測錯誤!KNN預測是" + resultKnn + ",正確是=" + answer[i]);
            }


        }

        System.out.println("///////////////////////////////////////////////////////////");
        System.out.println("支援向量機測試精準值=" + ((float)rightSVM /
                           (float)subMatTestPcaOcr.rows()) * 100 + "%");
        System.out.println("KNN測試精準值=" + ((float)rightKnn /
                                               (float)subMatTestPcaOcr.rows()) * 100 + "%");
    }

}

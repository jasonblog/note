package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;
import org.opencv.core.Core.MinMaxLocResult;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.opencv.ml.ANN_MLP;
import org.opencv.ml.Ml;

public class Ch13_10_1AnnSample
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        ANN_MLP ann = ANN_MLP.create();

        float[] labels = {0, 1, 2, 3, 4}; //[3][5]
        Mat labelsMat = new Mat(5, 4, CvType.CV_32FC1);

        labelsMat.put(0, 0, new float[] {1, 0, 0, 0});
        labelsMat.put(1, 0, new float[] {0, 1, 0, 0});
        labelsMat.put(2, 0, new float[] {0, 0, 1, 0});
        labelsMat.put(3, 0, new float[] {0, 0, 0, 1});
        labelsMat.put(4, 0, new float[] {1, 0, 0, 0});

        Mat trainingDataMat = new Mat(5, 2, CvType.CV_32FC1);
        trainingDataMat.put(0, 0, new float[] {0, 0});
        trainingDataMat.put(1, 0, new float[] {20, 30});
        trainingDataMat.put(2, 0, new float[] {100, 50});
        trainingDataMat.put(3, 0, new float[] {300, 400});
        trainingDataMat.put(4, 0, new float[] {3, 6});


        Mat layerSize = new Mat(4, 1, CvType.CV_32SC1);
        int[] layerSizeAry = {2, 5, 5, 4};
        //layerSize.put(0, 0, layerSizeAry);
        layerSize.put(0, 0, layerSizeAry[0]);
        layerSize.put(1, 0, layerSizeAry[1]);
        layerSize.put(2, 0, layerSizeAry[2]);
        layerSize.put(3, 0, layerSizeAry[3]);
        //      layerSize.put(0,4,layerSizeAry[4]);
        //System.out.println(layerSize.dump());
        Mat responses = new Mat();


        ann.setLayerSizes(layerSize);
        ann.setTrainMethod(ann.RPROP);
        //ann.setTrainMethod(ann.BACKPROP);
        //      TermCriteria criteria=new TermCriteria(TermCriteria.MAX_ITER|TermCriteria.EPS, 1000, 0.01);
        //      ann.setTermCriteria(criteria);
        //      ann.setBackpropMomentumScale(0.1);
        //      ann.setBackpropWeightScale(0.1);
        //      ann.setRpropDW0(0.1);
        //      ann.setRpropDWPlus(1.2);
        //      ann.setRpropDWMinus(0.5);
        //      ann.setRpropDWMin(1);
        //      ann.setRpropDWMax(50);

        ann.setActivationFunction(ann.SIGMOID_SYM);

        boolean resultTrain = ann.train(trainingDataMat, Ml.ROW_SAMPLE, labelsMat);
        System.out.println("是否有訓練成功=" + resultTrain);
        Mat results = new Mat();
        int flags = 0;
        Mat sample = new Mat(1, 2, CvType.CV_32FC1);

        //float[] sampleData={20,30};//1
        //float[] sampleData={0,0};//0
        //float[] sampleData={100,50};//2
        //float[] sampleData={300,400};//3
        //float[] sampleData={3,6};//0
        float[] sampleData = {2000, 3000}; //3

        sample.put(0, 0, sampleData);
        System.out.println(" sample=" + sample.dump());
        float result = ann.predict(sample, results, flags);
        System.out.println("result=" + result);
        System.out.println("results=" + results.dump());
        System.out.println("flags=" + flags);
        MinMaxLocResult MinMaxLocres = Core.minMaxLoc(results);
        System.out.println("MinMaxLocres.maxLoc.x=" + MinMaxLocres.maxLoc.x);

    }

}

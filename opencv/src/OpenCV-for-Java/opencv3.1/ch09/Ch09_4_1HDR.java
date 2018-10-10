package ch09;
import java.util.ArrayList;
import java.util.List;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.photo.CalibrateRobertson;
import org.opencv.photo.MergeRobertson;
import org.opencv.photo.CalibrateDebevec;
import org.opencv.photo.MergeDebevec;
import org.opencv.photo.MergeMertens;
import org.opencv.photo.Photo;
import org.opencv.photo.Tonemap;
import org.opencv.photo.TonemapDrago;
import org.opencv.photo.TonemapDurand;
import org.opencv.photo.TonemapMantiuk;
import org.opencv.photo.TonemapReinhard;

public class Ch09_4_1HDR
{
    public static void main(String[] args)
    {
        try {
            System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
            Mat source1 = Imgcodecs.imread("C://opencv3.1//samples//HDRmantis1.jpg",
                                           Imgcodecs.CV_LOAD_IMAGE_COLOR);
            Mat source2 = Imgcodecs.imread("C://opencv3.1//samples//HDRmantis2.jpg",
                                           Imgcodecs.CV_LOAD_IMAGE_COLOR);
            Mat source3 = Imgcodecs.imread("C://opencv3.1//samples//HDRmantis3.jpg",
                                           Imgcodecs.CV_LOAD_IMAGE_COLOR);
            Mat source4 = Imgcodecs.imread("C://opencv3.1//samples//HDRmantis4.jpg",
                                           Imgcodecs.CV_LOAD_IMAGE_COLOR);
            Mat source5 = Imgcodecs.imread("C://opencv3.1//samples//HDRmantis5.jpg",
                                           Imgcodecs.CV_LOAD_IMAGE_COLOR);
            Mat source6 = Imgcodecs.imread("C://opencv3.1//samples//HDRmantis6.jpg",
                                           Imgcodecs.CV_LOAD_IMAGE_COLOR);
            Mat source7 = Imgcodecs.imread("C://opencv3.1//samples//HDRmantis7.jpg",
                                           Imgcodecs.CV_LOAD_IMAGE_COLOR);

            Mat response = new Mat();

            Photo photo = new Photo();
            //CalibrateDebevec calibrate=photo.createCalibrateDebevec();
            CalibrateDebevec calibrateDebevec = photo.createCalibrateDebevec(70, 10.0f,
                                                false);

            Mat calibrateRobertsonMat = new Mat();
            CalibrateRobertson calibrateRobertson = photo.createCalibrateRobertson(30,
                                                    0.01f);

            //Load images and exposure times
            List<Mat> listMat = new ArrayList<Mat>();
            listMat.add(source1);
            listMat.add(source2);
            listMat.add(source3);
            listMat.add(source4);
            listMat.add(source5);
            listMat.add(source6);
            listMat.add(source7);


            Mat times = new Mat(7, 1, CvType.CV_32FC1);
            times.put(0, 0, 32);
            times.put(1, 0, 16);
            times.put(2, 0, 8);
            times.put(3, 0, 4);
            times.put(4, 0, 1);
            times.put(5, 0, 0.5);
            times.put(6, 0, 0.25);

            /*
            times.put(0, 0, 100);
            times.put(1, 0, 225);
            times.put(2, 0, 325);
            times.put(3, 0, 525);
            times.put(4, 0, 625);
            times.put(5, 0, 785);
            times.put(6, 0, 1024);
             */

            //Estimate camera response
            calibrateDebevec.process(listMat, response, times);
            calibrateRobertson.process(listMat, calibrateRobertsonMat, times);
            System.out.println("response=" + response.dump());
            System.out.println("calibrateRobertsonMat=" + calibrateRobertsonMat.dump());
            System.out.println("times=" + times.dump());


            //Make HDR image
            Mat hdr = new Mat();
            Mat hdrRobertson = new Mat();
            MergeDebevec mergeDebevec = photo.createMergeDebevec();
            mergeDebevec.process(listMat, hdr, times, response);
            mergeDebevec.process(listMat, hdrRobertson, times, calibrateRobertsonMat);
            // System.out.println("hdr="+hdr.dump());




            Mat ldr = new Mat();
            Mat ldrRobertson = new Mat();
            //TonemapDurand tonemapDurand=photo.createTonemapDurand(2.2f,11.5f,31.9f,31.1f,31.1f);
            TonemapDurand tonemapDurand = photo.createTonemapDurand(1.0f, 4.0f, 1.0f, 2.0f,
                                          2.0f);
            tonemapDurand.process(hdr, ldr);
            tonemapDurand.process(hdrRobertson, ldrRobertson);

            ldr = EachPixelMultiplyNum(ldr, 255);
            ldrRobertson = EachPixelMultiplyNum(ldrRobertson, 255);


            //Perform exposure fusion
            Mat fusion = new Mat();
            MergeMertens mergeMertens = photo.createMergeMertens();
            mergeMertens.process(listMat, fusion);
            fusion = EachPixelMultiplyNum(fusion, 255);


            Mat mergeRobertsonMat = new Mat();
            MergeRobertson mergeRobertson =  photo.createMergeRobertson();
            mergeRobertson.process(listMat, mergeRobertsonMat, times);
            mergeRobertsonMat = EachPixelMultiplyNum(mergeRobertsonMat, 255);


            //          List<Mat> alignMTBMatList=new ArrayList<Mat>();
            //          AlignMTB  alignMTB=photo.createAlignMTB(6, 4, true);
            //          alignMTB.process(listMat, alignMTBMatList, times, response);


            //Tonemap HDR image
            Mat tonemapMat = new Mat();
            Tonemap tonemap = photo.createTonemap(1.0f);
            tonemap.process(hdr, tonemapMat);
            tonemapMat = EachPixelMultiplyNum(tonemapMat, 255);

            Mat tonemapDragoMat = new Mat();
            TonemapDrago   tonemapDrago = photo.createTonemapDrago(1.0f, 1.0f, 0.85f);
            tonemapDrago.process(hdr, tonemapDragoMat);
            tonemapDragoMat = EachPixelMultiplyNum(tonemapDragoMat, 255);


            Mat tonemapMantiukMat = new Mat();
            TonemapMantiuk tonemapMantiuk =  photo.createTonemapMantiuk(1.0f, 0.7f, 1.0f);
            tonemapMantiuk.process(hdr, tonemapMantiukMat);
            tonemapMantiukMat = EachPixelMultiplyNum(tonemapMantiukMat, 255);


            Mat tonemapReinhardMat = new Mat();
            TonemapReinhard tonemapReinhard =   photo.createTonemapReinhard(1.0f, 0.0f,
                                                1.0f, 0.0f);
            tonemapReinhard.process(hdr, tonemapReinhardMat);
            tonemapReinhardMat = EachPixelMultiplyNum(tonemapReinhardMat, 255);


            //Write results
            Imgcodecs.imwrite("C://opencv3.1//samples//CalibrateDebevec-mantis-ldr.jpg",
                              ldr);
            Imgcodecs.imwrite("C://opencv3.1//samples//CalibrateDebevec-mantis-ldrRobertson.jpg",
                              ldrRobertson);

            Imgcodecs.imwrite("C://opencv3.1//samples//CalibrateDebevec-mantis-fusion.jpg",
                              fusion);
            Imgcodecs.imwrite("C://opencv3.1//samples//CalibrateDebevec-mantis-mergeRobertsonMat.jpg",
                              mergeRobertsonMat);

            //Imgcodecs.imwrite("C://opencv3.1//samples//CalibrateDebevec-hdr.hdr", hdr);
            Imgcodecs.imwrite("C://opencv3.1//samples//CalibrateDebevec-mantis-tonemapMat.jpg",
                              tonemapMat);
            Imgcodecs.imwrite("C://opencv3.1//samples//CalibrateDebevec-mantis-tonemapDragoMat.jpg",
                              tonemapDragoMat);
            Imgcodecs.imwrite("C://opencv3.1//samples//CalibrateDebevec-mantis-tonemapMantiukMat.jpg",
                              tonemapMantiukMat);
            Imgcodecs.imwrite("C://opencv3.1//samples//CalibrateDebevec-mantis-tonemapReinhardMat.jpg",
                              tonemapReinhardMat);

            //Imgcodecs.imwrite("C://opencv3.1//samples//CalibrateDebevec-AlignMTB.jpg", alignMTBMatList.get(0));
            // Imgcodecs.imwrite("C://opencv3.1//samples//CalibrateDebevec-Robertson.jpg",  calibrateRobertsonMat);
            //System.out.println("hdr="+hdr.dump());

        } catch (Exception e) {
            System.out.println("error: " + e.getMessage());
        }
    }

    public static Mat EachPixelMultiplyNum(Mat inputMat, int Val)
    {
        for (int i = 0; i < inputMat.rows(); i++) {
            for (int j = 0; j < inputMat.cols(); j++) {
                double[] data = new double[3];
                data = inputMat.get(i, j);
                data[0] = data[0] * Val;
                data[1] = data[1] * Val;
                data[2] = data[2] * Val;

                inputMat.put(i, j, data);
            }

        }

        return inputMat;
    }

}
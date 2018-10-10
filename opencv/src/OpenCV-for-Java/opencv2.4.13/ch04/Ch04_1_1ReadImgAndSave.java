package ch04;

import java.util.ArrayList;
import java.util.List;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;

public class Ch04_1_1ReadImgAndSave
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }

    public static void main(String[] args)
    {
        try {
            Mat source = Highgui.imread("C://opencv3.1//samples//7.png");
            List<Mat> bgrList = new ArrayList<Mat>(3);

            System.out.println("channels=" + source.channels()); //灰階=1,RGB=3
            System.out.println("total=" + source.total()); //col*row
            System.out.println("col=" + source.cols()); //col*row
            System.out.println("row=" + source.rows()); //col*row
            System.out.println("size=" + source.size()); //cols*rows,ex:9X10
            System.out.println("depth=" + source.depth()); //0
            System.out.println("type=" + source.type()); //16
            System.out.println("m=" +
                               source.dump()); //(BGR,BGR......9組,共9x3=27(col),row=10)

            Core.split(source, bgrList);//split 3 channels,R(2),G(1),B(0)
            System.out.println("blue=" + bgrList.get(0).dump()); //get only blue channel mat
            System.out.println("green=" + bgrList.get(
                                   1).dump()); //get only blue channel mat
            System.out.println("red=" + bgrList.get(2).dump()); //get only blue channel mat

            Highgui.imwrite("C://opencv3.1//samples//7.jpg", source);

        } catch (Exception e) {
            System.out.println("error: " + e.getMessage());
        }
    }

}

package ch13;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class Ch13_15_1CarPlateOcrByTesseract
{

    public static void main(String[] args) throws IOException,
    InterruptedException {
        String resultLine;
        Process p = Runtime.getRuntime().exec("tesseract C://opencv3.1//samples//vehiclePlate2.JPG -l eng c://result -psm 18 nobatch");
        BufferedReader bre = new BufferedReader(new InputStreamReader(p.getInputStream()));

        Thread.sleep(1000);

        FileReader fr = new FileReader("c://result.txt");
        BufferedReader br = new BufferedReader(fr);

        while (br.ready())
        {
            System.out.println(br.readLine());
        }

        br.close();
        fr.close();
    }

}

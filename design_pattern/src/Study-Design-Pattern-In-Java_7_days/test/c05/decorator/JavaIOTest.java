package c05.decorator;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import org.junit.Test;

import c05.decorator.javaIO.ReverseReader;
/**
 * 裝飾模式實例javaIO-測試
 */
public class JavaIOTest {
	@SuppressWarnings("resource")
	@Test
	public void test() throws IOException{
			System.out.println("=========FileReader讀取檔案==========");
			FileReader reader = new FileReader("test.txt");
			int c = reader.read();
			while (c >= 0) {
	            System.out.print((char)c);
	            c = reader.read();
	        }	
			
			System.out.println("=========BufferedReader讀取檔案==========");
			BufferedReader bufferedReader = new BufferedReader(new FileReader("test.txt"));
			String line = bufferedReader.readLine();;
			while (line!=null) {
                System.out.println(line);
                line = bufferedReader.readLine();
            }	
			
			System.out.println("=========Reverse Reader反轉讀入的內容==========");
			// 測試將讀入的句子倒轉
			ReverseReader reverseReader = new ReverseReader(new FileReader("test.txt"));
			String rLine = reverseReader.reverseLine();
			while (rLine!=null) {
                System.out.println(rLine);
                rLine = reverseReader.reverseLine();
            }	
	}
}

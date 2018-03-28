package c05.decorator.javaIO;

import java.io.FilterInputStream;
import java.io.IOException;
import java.io.InputStream;
/**
 * 裝飾類別-將讀入的字母轉成大寫
 */
public class UpperCaseInputStream extends FilterInputStream{

	public UpperCaseInputStream(InputStream in) {
		super(in);
	}

	@Override
	public int read() throws IOException {
		int c = super.read();
		return (c == -1) ? c : Character.toUpperCase((char)c) ;
	}

}

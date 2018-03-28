package c17.interpreter;

import org.junit.Test;

import c17.interpreter.numberInterpreter.Context;
import c17.interpreter.numberInterpreter.DownExpression;
import c17.interpreter.numberInterpreter.Expression;
import c17.interpreter.numberInterpreter.UpExpression;

/**
 * 解譯器模式-測試
 */
public class InterpreterTest {
	@Test
	public void test(){
		Expression ex ;
		Context context = new Context();
		context.setText("A4461 B1341 A676 B1787");

		System.out.println("============解譯器模式測試============");
		System.out.println("待解譯內容：A4461 B1341 A676 B1787");
		
		System.out.println("---解譯結果---");
		// A則後面的數字*2，B則後面的數字/2
		for(String str : context.getText().split("\\s")){
			// 不同的解譯器代表可以藉由實作更多Expression的字類別來擴充解譯器能力
			if(str.charAt(0) == 'A'){
				ex = new UpExpression();
			} else {
				ex = new DownExpression();
			}
			
			ex.interpret(str);
		}
	}
}

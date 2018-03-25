package c17.interpreter.numberInterpreter;
/**
 * 如果第一個字為A，數字*2(ConcreteExpression)
 */
public class UpExpression extends Expression {
	@Override
	public void excute(Integer number) {
		System.out.print(number*2 + " ");
	}

}

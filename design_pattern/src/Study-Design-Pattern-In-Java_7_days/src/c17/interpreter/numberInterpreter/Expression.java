package c17.interpreter.numberInterpreter;
/**
 * 共用解譯器類別(Expression)
 */
public abstract class Expression {
	public void interpret(String str){
		if(str.length() > 0){
			String text = str.substring(1, str.length());
			Integer number = Integer.valueOf(text);
			excute(number);
		}
	}
	
	/**
	 * 
	 * @param text 要解譯的內容
	 */
	protected abstract void excute(Integer number);
}

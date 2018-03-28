package c17.interpreter.numberInterpreter;
/**
 * 待解譯的資料(Context)
 */
public class Context {
	// 存放待解譯資料
    private String text;

	public String getText() {
		return text;
	}

	/**
	 * 以空白分段，每段開頭為字母A或B，之後接一數字 (ex. A122 B11 A178) 
	 * @param text
	 */
	public void setText(String text) {
		this.text = text;
	}

}

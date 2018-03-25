package c21.bridge.descript.b;
public abstract class 寄信 {
	protected 信件 mail;
	
	@SuppressWarnings("unused")
	private 寄信(){};
	
	public 寄信(信件 mail){
		this.mail = mail;
	}
	
	// 寄信
	abstract public void send();
}

package c21.bridge.descript.b;

public class 普通信件 extends 寄信{
	public 普通信件(信件 mail) {
		super(mail);
	}
	@Override
	public void send() {
		System.out.print(">>信件寄出後一天內抵達  ");
		super.mail.resgiterState();
	}
}

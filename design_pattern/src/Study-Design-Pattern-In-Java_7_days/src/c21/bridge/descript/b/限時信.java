package c21.bridge.descript.b;

public abstract class 限時信 extends 寄信{
	public 限時信(信件 mail) {
		super(mail);
	}

	@Override
	public void send() {
		System.out.print(">>信件寄出後24小時內抵達  ");
		super.mail.resgiterState();
	}
}

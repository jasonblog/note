package c21.bridge.mail;
//掛號信
public class RegisterMail extends Mail{
	@Override
	void resgiterState() {
		System.out.println("這是一封掛號信，收件人必需簽名  ");
	}
}

package c21.bridge.mail;
//非掛號信
public class NoRegisterMail extends Mail{
	@Override
	void resgiterState() {
		System.out.println("這是封信不是註冊信，收件人不用簽名  ");		
	}
}

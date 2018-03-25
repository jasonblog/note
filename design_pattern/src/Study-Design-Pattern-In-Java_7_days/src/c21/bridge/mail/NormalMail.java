package c21.bridge.mail;
//一般信件
public class NormalMail extends MailSender{
	public NormalMail(Mail mail) {
		super(mail);
	}
	@Override
	public void send() {
		System.out.print(">>信件寄出後3~5天內抵達  ");
		super.mail.resgiterState();
	}
}

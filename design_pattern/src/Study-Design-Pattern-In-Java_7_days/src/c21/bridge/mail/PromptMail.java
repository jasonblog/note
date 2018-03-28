package c21.bridge.mail;
//限時信
public class PromptMail extends MailSender{
	public PromptMail(Mail mail) {
		super(mail);
	}

	@Override
	public void send() {
		System.out.print(">>信件寄出後24小時內抵達  ");
		super.mail.resgiterState();
	}
}

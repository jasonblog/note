package c21.bridge.mail;
public abstract class MailSender {
	protected Mail mail;
	
	@SuppressWarnings("unused")
	private MailSender(){};
	
	public MailSender(Mail mail){
		this.mail = mail;
	}
	
	// 寄信
	abstract public void send();
}

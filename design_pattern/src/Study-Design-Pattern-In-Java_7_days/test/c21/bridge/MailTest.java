package c21.bridge;

import org.junit.Test;

import c21.bridge.mail.MailSender;
import c21.bridge.mail.NoRegisterMail;
import c21.bridge.mail.NormalMail;
import c21.bridge.mail.PromptMail;
import c21.bridge.mail.RegisterMail;

/**
 * 橋接模式-測試
 */
public class MailTest {

	@Test
	public void test(){
		System.out.println("============橋接模式測試============");
		System.out.println("----ㄧ般信件測試----");
		MailSender mailSender = new NormalMail(new NoRegisterMail());
		mailSender.send();
		mailSender= new NormalMail(new RegisterMail());
		mailSender.send();
		System.out.println("----限時信件測試----");
		mailSender = new PromptMail(new NoRegisterMail());
		mailSender.send();
		mailSender= new PromptMail(new RegisterMail());
		mailSender.send();
	}
}

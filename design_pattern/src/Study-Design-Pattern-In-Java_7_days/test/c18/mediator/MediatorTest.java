package c18.mediator;

import org.junit.Test;

import c18.mediator.Chatroom.CommonUser;
import c18.mediator.Chatroom.MessageMediator;
import c18.mediator.Chatroom.Messager;
import c18.mediator.Chatroom.VIPUser;
/**
 * 中介者模式-測試
 */
public class MediatorTest {
	@Test
	public void Test(){
		System.out.println("============中介者模式測試============");

		Messager jacky = new VIPUser("jacky");
		Messager huant = new CommonUser("huant");
		Messager neil = new CommonUser("neil");
		
		MessageMediator.joinChat(jacky);
		MessageMediator.joinChat(huant);
		MessageMediator.joinChat(neil);
		System.out.println("---VIP會員直接送訊息給每個人---");
		jacky.sendToAll("hi, 你好");
		
		System.out.println("---私底下送訊息---");
		jacky.send("單挑阿!PK阿!", huant);

		neil.send("收假了!!掰掰", jacky);
		System.out.println("---當非VIP會員想送訊息給每個人---");
		neil.sendToAll("阿阿阿!!!");
	}
}

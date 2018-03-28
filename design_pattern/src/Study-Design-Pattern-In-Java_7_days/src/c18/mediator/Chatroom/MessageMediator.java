package c18.mediator.Chatroom;

import java.util.ArrayList;
import java.util.List;

/**
 * 中介者類別(ConcreteMediator)
 */
public class MessageMediator implements Mediator {
	private static List<VIPUser> vipList = new ArrayList<>();
	private static List<CommonUser> userList = new ArrayList<>();

	public static void joinChat(Messager messager){
		if(messager.getClass().getSimpleName().equals("VIPUser")){
			vipList.add((VIPUser) messager);
		} else {
			userList.add((CommonUser) messager);
		}
	}
	
	// 發訊息給某人
	public void send(String message,String from , Messager to){
		for(Messager msg : vipList){
			if(msg.getName().equals(from)){
				System.out.println(from + "->" + to.getName() + ":" + message);
			}
		}
		for(Messager msg : userList){
			if(msg.getName().equals(from)){
				System.out.println(from + "->" + to.getName() + ":" + message);
			}
		}
	};
	
	// 發訊息給每個人
	public void sendToAll(String from, String message){
		for(Messager msg : vipList){
			if(!msg.getName().equals(from)){
				System.out.println(from + "->" + msg.getName() + ":" + message);
			}
		}
		
		for(Messager msg : userList){
			if(!msg.getName().equals(from)){
				System.out.println(from + "->" + msg.getName() + ":" + message);
			}
		}
		
	};
}
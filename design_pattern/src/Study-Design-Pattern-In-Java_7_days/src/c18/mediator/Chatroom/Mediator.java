package c18.mediator.Chatroom;
/**
 * 中介者介面(Mediator)
 */
public interface Mediator {
	// 發訊息給某人
	void send(String message,String from , Messager to);
	
	// 發訊息給每個人
	void sendToAll(String from, String message);
}

package c18.mediator.Chatroom;
/**
 * 可以發送訊息的類別(ConcreteColleague)
 */
public class CommonUser extends Messager{

	public CommonUser(String name) {
		super(name);
	}	

	@Override
	public void sendToAll(String message){
		System.out.println("非VIP用戶不能使用廣播");
	}
}

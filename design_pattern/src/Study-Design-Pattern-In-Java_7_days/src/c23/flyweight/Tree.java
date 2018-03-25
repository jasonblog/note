package c23.flyweight;
/**
 * 樹木(Flyweight)
 */
public class Tree {
	private String type;	// 樹種(內部性質，可以共享的資訊)
	private String owner;	// 樹的擁有者(外部性質，不能共享的資訊)

	public Tree(String type){
		this.type = type;
		System.out.println("取得一顆新的" + type);
	}
	
	
	public void setOwner (String owner){
		this.owner = owner;
	}
	
	/**
	 * 
	 */
	public void display(){
		System.out.println(type  + " , 擁有者: " + owner);
	}
}

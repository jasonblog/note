package c24.visitor.description;

/**
 * 廚師介面-被操作的元素
 */
public abstract class Chef {
	private String name;
	public Chef(String name){
		this.name = name;
	}
	public String getName() {
		return name;
	}
	
	abstract void cookTofu(); 	//廚師要會做豆腐
	abstract void cookSaoMai(); //廚師要會做燒賣
}

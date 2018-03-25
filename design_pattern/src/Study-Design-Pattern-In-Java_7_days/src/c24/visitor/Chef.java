package c24.visitor;

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
	
	// visitor代表裁判指定的料理
	public abstract void accept(Visitor visitor);

}

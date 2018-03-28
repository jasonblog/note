package c05.decorator.title;

/**
 * 長槍兵(ConcreteComponent)
 */
public class Lancer implements Adventurer{
	// 冒險者的姓名
	private String name ;

	// 冒險者被創立的時候要有姓名
	public Lancer(String name){
		this.name = name;
	}

	// 攻擊
	public void attack(){
		System.out.println("長槍攻擊 by " + name);
	}

}

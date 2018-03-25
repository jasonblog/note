package c05.decorator.title;
/**
 * 稱號-敏捷 (ConcreteDecorator)
 */
public class TitleAgile extends Title{	

	public TitleAgile(Adventurer adventuerer) {
		super(adventuerer);
	}

	// 稱號讓攻擊變快
	@Override
	public void attack(){
		System.out.print("快速 ");
		super.adventuerer.attack();
	}

	// 取得稱號後獲得新的技能
	public void useFlash(){
		System.out.println("使用瞬間移動");
	}

}

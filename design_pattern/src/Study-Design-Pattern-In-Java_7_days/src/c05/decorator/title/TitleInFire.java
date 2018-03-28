package c05.decorator.title;
/**
 * 稱號-燃燒(ConcreteDecorator)
 */
public class TitleInFire extends Title{
	public TitleInFire(Adventurer adventurer) {
		super(adventurer);
	}
	
	// 稱號讓攻擊增加燃燒
	@Override
	public void attack(){
		System.out.print("燃燒 ");
		super.attack();
	}
	
	public void fireball(){
		System.out.println("丟火球");
	}
}

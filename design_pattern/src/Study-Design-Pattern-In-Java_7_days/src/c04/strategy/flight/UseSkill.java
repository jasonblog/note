package c04.strategy.flight;
/**
 * 使用技能(ConcreteStrategy)
 */
public class UseSkill implements FlightStrategy {
	@Override
	public void execute() {
		System.out.println("使用超級痛的技能攻擊");		
	}
}

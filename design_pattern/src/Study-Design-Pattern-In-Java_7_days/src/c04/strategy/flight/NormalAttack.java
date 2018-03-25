package c04.strategy.flight;
/**
 * 一般攻擊(ConcreteStrategy)
 */
public class NormalAttack implements FlightStrategy {

	@Override
	public void execute() {
		System.out.println("使用一般攻擊");		
	}

}

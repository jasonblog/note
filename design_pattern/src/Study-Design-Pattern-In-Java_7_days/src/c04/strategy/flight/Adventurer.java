package c04.strategy.flight;
/**
 * 冒險者(Context)
 */
public class Adventurer {
	FlightStrategy flightStrategy;  //不同戰鬥方式效果不同(strategy)
	/**
	 * 攻擊
	 */
	public void attack(){
		// 預設為普通攻擊
		if(flightStrategy == null){
			flightStrategy = new NormalAttack();
		}
		flightStrategy.execute();
	}
	
	/**
	 * 選擇不同的武器(策略)
	 */
	public void choiceStrategy(FlightStrategy strategy){
		this.flightStrategy = strategy;
	}
}

package c15.builder.robotBuilder;

/**
 * 鋼彈建造者類別(ConcreteBuilder)
 */
public class GundamBuilder extends RobotBuilder{
	
	/**
	 * 建造機器人外型
	 */
	public Form buildForm(){
		// 這邊可以想像成用工廠類別可以創造很多種不同的外型
		return new Form("鋼彈");
	};
	/**
	 * 建造機器人動力系統
	 */
	public Power buildPower(){
		// 這邊可以想像成用工廠類別可以創造不同的動力系統
		return new Power("亞哈反應爐","Beta發電機","氫電池");
	};
	/**
	 * 建造機器人武器系統
	 */
	public Weapon buildWeapon(){
		// 這邊可以想像成用工廠類別可以創造不同的武器
		return new Weapon(new String[]{"60mm火神砲","突擊長矛","薩克機槍","光束劍"});

	};

}


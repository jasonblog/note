package c15.builder.robotBuilder;

/**
 * 機器人建造器抽像類別(AbstractBuilder)
 *
 */
public abstract class RobotBuilder {
	
	/**
	 * 建造機器人外型
	 */
	public abstract Form buildForm();
	/**
	 * 建造機器人動力系統
	 */
	public abstract Power buildPower();
	/**
	 * 建造機器人武器系統
	 */
	public abstract Weapon buildWeapon();

}


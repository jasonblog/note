package c15.builder.robotBuilder;

/**
 * 指揮如何組裝機器人(Director)
 */
public class Director {
	private RobotBuilder builder;
	public Director(RobotBuilder builder){
		this.builder = builder;
	}
	
	/**
	 * Builder Pattern的特色就是會在Director內規範建造的順序
	 * @return
	 */
	public IRobot builderRobot(){
		IRobot robot = new Gundam();
		// 依照順序建造機器人
		robot.setForm(builder.buildForm());
		robot.setPower(builder.buildPower());
		robot.setWeapon(builder.buildWeapon());		
		return robot;
	}
}

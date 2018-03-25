
package c15.builder.robotBuilder;
/**
 * 機器人介面(Product)
 */
public abstract class IRobot {
	Form  form; // 外型
	Power power; // 動力
	Weapon weapon; //武器
	
	public void setForm (Form  form){
		this.form = form;
	};
	
	public void setPower(Power power){
		this.power = power;
	};
	
	public void setWeapon(Weapon weapon){
		this.weapon = weapon;
	};
	
	public void display(){
		System.out.println("機器人外型：" + form);
		System.out.println("機器人動力：" + power);
		System.out.println("機器人武器：" + weapon);

	}; // 展示機器人
}

package c12.state;

import org.junit.Test;

import c12.state.warrior.Warrior;

/**
 * 狀態模式-測試
 */
public class WarriorTest {
	Warrior warrior = new Warrior();
	
	@Test 
	public void test(){
		System.out.println("============狀態模式測試============");
		warrior.move();
		
		warrior.getDamage(30); // 受到傷害
		warrior.move();
		warrior.getDamage(50); // 受到傷害
		warrior.move();
		
		warrior.heal(120); 		// 接受治療
		warrior.move();

		warrior.getDamage(110);
		warrior.move();
		warrior.heal(20); 		// 接受治療， hp = 0的時候治療無效
	}
}

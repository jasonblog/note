package c03.abstractfactory;

import org.junit.Assert;
import org.junit.Test;

import c03.abstractfactory.factory.ArcherEquipFactory;
import c03.abstractfactory.factory.Clothes;
import c03.abstractfactory.factory.EquipFactory;
import c03.abstractfactory.factory.WarriorEquipFactory;
import c03.abstractfactory.factory.Weapon;
import c03.abstractfactory.village.Adventurer;
import c03.abstractfactory.village.ArcherTrainingCamp;
import c03.abstractfactory.village.TrainingCamp;
import c03.abstractfactory.village.WarriorTrainingCamp;
/**
 * 抽像工廠模式-測試
 */
public class EquipFactoryTest {
	private EquipFactory equipFactory;
	@Test
	/**
	 * 測試工廠是否能正確生產裝備
	 */
	public void equipFactoryTest(){
        System.out.println("==========抽像工廠模式測試==========");
		
		// 幫弓箭手生產裝備
		equipFactory = new ArcherEquipFactory();
		Clothes archerLeather = equipFactory.productArmor();
		Weapon archerBow = equipFactory.productWeapon();

		// 皮甲的防禦應該是5，弓的攻擊為10，範圍為10
		Assert.assertEquals(5, archerLeather.getDef());
		Assert.assertEquals(10, archerBow.getAtk());
		Assert.assertEquals(10, archerBow.getRange());

		
		// 幫鬥士生產裝備
		equipFactory = new WarriorEquipFactory();
		Clothes armor = equipFactory.productArmor();
		Weapon longSword = equipFactory.productWeapon();
		
		// 盔甲的防禦應該是10，弓的攻擊為10，範圍為1
		Assert.assertEquals(10, armor.getDef());
		Assert.assertEquals(10, longSword.getAtk());
		Assert.assertEquals(1, longSword.getRange());
		
		
		// 弓箭手訓練營
		TrainingCamp camp = new ArcherTrainingCamp();
		// 訓練弓箭手
		Adventurer archer = camp.trainAdventurer();
		
		// 鬥士訓練營
		camp = new WarriorTrainingCamp();
		// 訓練鬥士
		Adventurer warrior = camp.trainAdventurer();
		
		archer.display();
		warrior.display();
	}
}

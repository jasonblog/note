package c03.abstractfactory.village;

import c03.abstractfactory.factory.EquipFactory;
import c03.abstractfactory.factory.WarriorEquipFactory;

/**
 * 鬥士訓練營(Con)
 */
public class WarriorTrainingCamp implements TrainingCamp {
	private static EquipFactory factory = new WarriorEquipFactory();
	
	@Override
	public Adventurer trainAdventurer() {
		Warrior warrior = new Warrior();
		// ...進行基本訓練
		// ...鬥士訓練課程
		
		// 訓練完成配發裝備
		warrior.setWeapon(factory.productWeapon());
		warrior.setClothes(factory.productArmor());
		return warrior; 
	}
}

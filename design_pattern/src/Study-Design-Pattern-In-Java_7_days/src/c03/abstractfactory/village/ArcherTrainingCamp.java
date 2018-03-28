package c03.abstractfactory.village;

import c03.abstractfactory.factory.ArcherEquipFactory;
import c03.abstractfactory.factory.EquipFactory;
import c03.abstractfactory.village.Adventurer;
/**
 * 實體工廠-弓箭手訓練營
 */
public class ArcherTrainingCamp implements TrainingCamp {
	private static EquipFactory factory = new ArcherEquipFactory();

	@Override
	public Adventurer trainAdventurer() {
		Archer archer = new Archer();
		// ...進行基本訓練
		// ...弓箭手訓練課程
		// 訓練完成配發裝備
		archer.setWeapon(factory.productWeapon());
		archer.setClothes(factory.productArmor());
		return archer; 
	}

}

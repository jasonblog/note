package c02.factory.village;

/**
 * 實體工廠-鬥士訓練營
 */
public class WarriorTrainingCamp implements TrainingCamp {
	@Override
	public Adventurer trainAdventurer() {
		System.out.println("訓練一個鬥士");
		return new Warrior(); 
	}
}

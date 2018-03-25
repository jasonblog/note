package c01.simpleFactory.village;

/**
 * 冒險者訓練營(SimpleFactor)
 */
public class TrainingCamp {
	public static Adventurer trainAdventurer(String type){
		switch(type){
			case "archer" : {
				System.out.println("訓練一個弓箭手");
				return new Archer(); 			
			}
			case "warrior": {
				System.out.println("訓練一個鬥士");
				return new Warrior();				
			}
			// 假如冒險者種類新增，增加case就可以
			default : return null;
		}
	}
}

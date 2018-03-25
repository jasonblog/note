package c02.factory;

import org.junit.Test;

import c02.factory.village.Adventurer;
import c02.factory.village.ArcherTrainingCamp;
import c02.factory.village.TrainingCamp;
import c02.factory.village.WarriorTrainingCamp;

import org.junit.Assert;
/**
 * 工廠模式-測試
 */
public class TrainingCampTest {
	@Test
	public void test(){
		System.out.println("==========工廠模式測試==========");
		
		//訓練營訓練冒險者
		//先用弓箭手訓練營訓練弓箭手
		TrainingCamp trainingCamp = new ArcherTrainingCamp();
		Adventurer memberA = trainingCamp.trainAdventurer();
		
		//用鬥士訓練營訓練鬥士
		trainingCamp = new WarriorTrainingCamp();
		Adventurer memberB = trainingCamp.trainAdventurer();
		
		//看看是不是真的訓練出我們想要的冒險者
		Assert.assertEquals(memberA.getType(), "Archer");
		Assert.assertEquals(memberB.getType(), "Warrior");
		// memberB應該是Warrior不是Knight，因此下面這行會報錯
		// Assert.assertEquals(memberB.getType(), "Knight");
	}
}

package c05.decorator;

import org.junit.Test;

import c05.decorator.title.Adventurer;
import c05.decorator.title.Lancer;
import c05.decorator.title.TitleAgile;
import c05.decorator.title.TitleInFire;
import c05.decorator.title.TitleStrong;
/**
 * 裝飾者模式-測試
 */
public class TitleTest {
	@Test
	public void test(){
		System.out.println("============裝飾者模式測試============");
		
		// 一開始沒有任何稱號的冒險者
		Adventurer lancer = new Lancer("Jacky");
		System.out.println("---長槍兵Jacky---");
		lancer.attack();
		
		System.out.println();	
		System.out.println("---取得強壯稱號的jacky---");
		TitleStrong sJacky = new TitleStrong(lancer);
		sJacky.attack();

		
		System.out.println();
		System.out.println("---取得敏捷稱號的jacky---");
		TitleAgile aJacky = new TitleAgile(sJacky);
		aJacky.attack();
		aJacky.useFlash();
		
		System.out.println();
		System.out.println("---取得燃燒稱號的jacky---");
		TitleInFire fJacky = new TitleInFire(sJacky);
		fJacky.attack();
		fJacky.fireball();	
		
		System.out.println("---jacky決定成為一個非常強壯的槍兵---");
		TitleStrong ssJacky = new TitleStrong(fJacky);
		ssJacky.attack();
	}
}

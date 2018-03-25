package c06.observer;

import org.junit.Test;

import c06.observer.association.Adventurer;
import c06.observer.association.Association;
import c06.observer.association.Bard;
import c06.observer.association.Gunman;
import c06.observer.association.Lancer;
import c06.observer.association.Subject;

/**
 * 觀察者模式-測試
 */
public class AssociationTest {
	@Test
	public void test () {
		System.out.println("============觀察者模式測試============");

		// 冒險者們
		Adventurer lancer = new Lancer("jacky");
		Adventurer lancer2 = new Lancer("seven");
		Adventurer bard = new Bard("lee");
		Adventurer gunman = new Gunman("longWu");
	
		// 冒險者協會
		Subject association = new Association();
		association.add(lancer);
		association.add(lancer2);
		association.add(bard);
		association.add(gunman);
		
		System.out.println("---派送簡單任務---");
		association.sendQuestions("run");
		
		System.out.println();
		System.out.println("---派送複雜任務---");
		association.sendQuestions("run run run, run for your life");
		
		// seven表示他不想接到任務通知了
		association.remove(lancer2);
		System.out.println();
		System.out.println("---派送複雜任務(seven已經不在名單中)---");
		association.sendQuestions("run run run, run for your life");	
	}
}

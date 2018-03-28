package c06.observer.association;
/**
 * 槍兵(Concrete Observer)-繼承冒險者
 */
public class Lancer extends Adventurer {
	public Lancer(String name) {
		super(name);
	}

	@Override
	public void getQuestions(String questions) {
		System.out.println(name + ":單來就改，任務來就接，沒在怕的");		
	}
}

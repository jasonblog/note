package c06.observer.association;
/**
 * 槍手(Concrete Observer)-繼承冒險者
 */
public class Gunman extends Adventurer {

	public Gunman(String name) {
		super(name);
	}

	@Override
	public void getQuestions(String questions) {
		if(questions.length() < 10){
			System.out.println(name + ":任務太簡單了，我不想理他");
		} else {
			System.out.println(name + ":只要我的手上有槍，誰都殺不死我，出發執行任務賺獎金拉!!!");
		}
	}
}

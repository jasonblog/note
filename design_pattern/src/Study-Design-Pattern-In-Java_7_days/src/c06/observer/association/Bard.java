package c06.observer.association;
/**
 * 吟遊詩人(Concrete Observer)-繼承冒險者
 */
public class Bard extends Adventurer {

	public Bard(String name) {
		super(name);
	}

	@Override
	public void getQuestions(String questions) {
		if(questions.length() > 10){
			System.out.println(name + ":任務太難了，我只會唱歌跳舞，不接不接");
		} else {
			System.out.println(name + ":當街頭藝人太難賺了，偶爾也是要解任務賺點錢的");
		}

	}
}

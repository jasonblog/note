package c06.observer.association;
/**
 * 冒險者協會(ConcreteSubject)
 */
public class Association extends Subject {

	@Override
	public void sendQuestions(String questions) {
		for(Adventurer adventurer : list){
			adventurer.getQuestions(questions);
		}
		
	}
}

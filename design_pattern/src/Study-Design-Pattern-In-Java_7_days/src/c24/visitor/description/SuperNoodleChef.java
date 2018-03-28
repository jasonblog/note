package c24.visitor.description;
//特級麵點師傅
public class SuperNoodleChef extends Chef {
	public SuperNoodleChef(String name) {
		super(name);
	}

	@Override
	void cookTofu() {
		System.out.println(this.getName() + " : 鐵桿臭豆腐");								
	}

	@Override
	void cookSaoMai() {
		System.out.println(this.getName() + " : 鐵桿50人份燒賣");					
	}
}

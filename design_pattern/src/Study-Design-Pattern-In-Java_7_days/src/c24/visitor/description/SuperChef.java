package c24.visitor.description;

// 特級廚師
public class SuperChef extends Chef {

	public SuperChef(String name) {
		super(name);
	}

	@Override
	void cookTofu() {
		System.out.println(this.getName() + " : 宇宙大燒賣");		
	}

	@Override
	void cookSaoMai() {
		System.out.println(this.getName() + " : 熊貓豆腐");		
	}
}

package c24.visitor.description;

 //黑暗料理界廚師
public class DarkChef extends Chef {
	public DarkChef(String name) {
		super(name);
	}

	@Override
	void cookTofu() {
		System.out.println(this.getName() + " : 魔幻鴉片燒賣");
	}

	@Override
	void cookSaoMai() {
		System.out.println(this.getName() + " : 豆腐三重奏");		
	}
}

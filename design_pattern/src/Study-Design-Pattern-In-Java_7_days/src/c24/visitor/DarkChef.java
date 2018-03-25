package c24.visitor;
/**
 * 黑暗料理界廚師-被操作的物件
 */
public class DarkChef extends Chef {
	
	public DarkChef(String name) {
		super(name);
	}

	// 如何實現做料理的工作已經移交給visitor
	@Override
	public void accept(Visitor visitor) {
		visitor.cook(this);
	}

}

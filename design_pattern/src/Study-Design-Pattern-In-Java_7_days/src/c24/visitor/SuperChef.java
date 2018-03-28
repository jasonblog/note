package c24.visitor;
/**
 * 特級廚師-被操作的元素
 */
public class SuperChef extends Chef {

	public SuperChef(String name) {
		super(name);
	}

	// 如何實現做料理的工作已經移交給visitor
	@Override
	public void accept(Visitor visitor) {
		visitor.cook(this);
	}

}

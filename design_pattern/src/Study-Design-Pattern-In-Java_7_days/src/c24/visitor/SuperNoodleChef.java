package c24.visitor;
/**
 * 特級麵點師傅-被操作的物件
 */
public class SuperNoodleChef extends Chef {

	public SuperNoodleChef(String name) {
		super(name);
	}

	// 如何實現做料理的工作已經移交給visitor
	@Override
	public void accept(Visitor visitor) {
		visitor.cook(this);
	}

}

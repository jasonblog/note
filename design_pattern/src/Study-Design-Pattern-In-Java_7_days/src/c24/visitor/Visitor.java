package c24.visitor;
/**
 * 指定的菜餚-拜訪者
 */
public interface Visitor {
	// 利用overload來實現每種不同廚師煮出不同的指定菜餚
	void cook(DarkChef superChef);
	void cook(SuperChef superChef);
	void cook(SuperNoodleChef superNoodleChef);
}

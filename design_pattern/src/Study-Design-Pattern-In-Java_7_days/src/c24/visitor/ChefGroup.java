package c24.visitor;

import java.util.ArrayList;
import java.util.List;

/**
 * 參加比賽的廚師(被操作元素集合)
 */
public class ChefGroup {
	private List<Chef> list = new ArrayList<>();
	
	public void join(Chef chef){
		list.add(chef);
	}
	
	public void leave(Chef chef){
		list.remove(chef);
	}
	
	/**
	 * 指定比賽題目
	 */
	public void topic(Visitor visitor){
		for(Chef chef : list){
			chef.accept(visitor);
		}
	}
}

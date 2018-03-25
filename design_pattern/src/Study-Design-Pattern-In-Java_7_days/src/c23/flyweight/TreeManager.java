package c23.flyweight;

import java.util.HashMap;
import java.util.Map;

/**
 * 樹種管理員(Flyweight factory)
 */
public class TreeManager {
	private static Map<String, Tree> treePool = new HashMap<>();
	
	public static Tree getTree(String type){
		// 如果目前還沒有這種種類的樹，就新增一棵
		if(!treePool.containsKey(type)){
			treePool.put(type, new Tree(type));
		}
		// 已經有這樣的樹，拿pool裡面的出來
		return treePool.get(type);
	}
}

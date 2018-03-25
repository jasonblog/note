package c24.visitor.description;

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
	public void topic(Topic topic){
		String topicName = topic.getClass().getSimpleName();
		
		if(topicName.equals("Topic_saoMai")){
			//比賽題目為燒賣
			for(Chef chef : list){
				chef.cookSaoMai();
			}				
		} else if(topicName.equals("tofu")){
			// 比賽題目為豆腐
			for(Chef chef : list){
				chef.cookTofu();
			}
		}
	}
}

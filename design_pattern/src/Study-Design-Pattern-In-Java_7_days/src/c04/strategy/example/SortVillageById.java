package c04.strategy.example;

import java.util.Comparator;
/**
 * 使用ID排序(ConcretStrategy)
 */
public class SortVillageById implements Comparator<Village>{
	@Override
	public int compare(Village o1, Village o2) {
		if(o1.id > o2.id){
			return 1;
		} 
		
		if(o1.id < o2.id){
			return -1;
		}
		return 0;
	}
}
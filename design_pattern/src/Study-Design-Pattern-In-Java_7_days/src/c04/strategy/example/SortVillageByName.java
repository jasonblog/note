package c04.strategy.example;

import java.util.Comparator;

/**
 * 村莊名稱做排序(ConcretStrategy)
 */
public class SortVillageByName implements Comparator<Village>{
	@Override
	public int compare(Village o1, Village o2) {
		if(o1.name.charAt(0) > o2.name.charAt(0)){
			return 1;
		} 
		
		if(o1.name.charAt(0) < o2.name.charAt(0)){
			return -1;
		}
		return 0;
	}
}
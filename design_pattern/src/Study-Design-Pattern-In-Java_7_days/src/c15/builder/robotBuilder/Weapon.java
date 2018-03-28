package c15.builder.robotBuilder;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * 機器人組件-武器(Product Part)
 */
public class Weapon {
	List<String> list = new ArrayList<>();
	public Weapon(String[] weaponList){
		list.addAll(Arrays.asList(weaponList));
	}
	@Override
	public String toString(){
		return list.toString();		
	}
}

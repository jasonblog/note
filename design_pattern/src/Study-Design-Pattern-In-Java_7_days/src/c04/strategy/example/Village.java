package c04.strategy.example;
/**
 * 村莊類別，等等拿來做排序用
 */
public class Village {
	public int id;
	public String name;
	public int population;
	public double area;
	
	public Village (int id, String name, int population, double area){
		this.id = id; 
		this.name = name;
		this.population = population;
		this.area = area;
	}
	
	@Override
	public String toString(){
		return id + "." + name + "(人口: " + population + " 面積: "+ area + ")";
	}
}

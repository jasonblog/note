package c01.simpleFactory.village;

public class SimpleFactory {
	public static Product produce(String type){
		switch(type){
			case "productA" : return new ProductA(); 
			case "productB": return new ProductB();
			default : return null;
		}
	}
}

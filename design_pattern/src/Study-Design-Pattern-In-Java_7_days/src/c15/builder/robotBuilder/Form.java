package c15.builder.robotBuilder;
/**
 * 機器人組件-外型(Product Part)
 */
public class Form {
	private String formName; 
	
	public Form(String formName){
		this.formName = formName;
	}
	
	public String toString(){
		return this.formName;
	}
	
}

package c20.prototype;

/**
 * 冒險者的履歷
 */
public class Resume implements Cloneable{
	private String name;
	private int level;
	private String profession;
	private AdventureExperience experience;

	public Resume(String name, String profession,  int level){
		this.name = name;
		this.level = level;
		this.profession = profession;
		experience = new AdventureExperience();
	}
	
	private Resume(AdventureExperience experience) throws CloneNotSupportedException{
		this.experience = (AdventureExperience) experience.clone();
	}

	@Override
	public Object clone() throws CloneNotSupportedException {
		// 直接使用 super.clone()，不會得到新的 AdventureExperinece實體
		Resume clone = new Resume(experience);
		clone.setName(this.name);
		clone.setLevel(this.level);
		clone.setProfession(this.profession);
		return clone;
	}
	
	
	public void display(){
		System.out.printf("冒險者：%s-%s 等級:%d \n", name, profession, level);
		System.out.printf("冒險經歷: %s %s \n", experience.getDate(), experience.getLocation());
		System.out.println();
	}

	public void setExperience(String date, String location) {
		experience.setDate(date);
		experience.setLocation(location);
	}
	
	public void setName(String name) {
		this.name = name;
	}

	public void setLevel(int level) {
		this.level = level;
	}

	public void setProfession(String profession) {
		this.profession = profession;
	}
}	




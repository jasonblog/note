package c20.prototype;

/**
 * 冒險者的冒險經歷
 */
public class AdventureExperience implements Cloneable {
	private String date;		// 日期
	private String location;	// 地點
	
	public String getLocation() {
		return location;
	}
	public void setLocation(String location) {
		this.location = location;
	}
	public String getDate() {
		return date;
	}
	public void setDate(String date) {
		this.date = date;
	}
	
	@Override
	protected Object clone() throws CloneNotSupportedException {
		return super.clone();
	} 
}

package c13.proxy.protect;
/**
 * 一般使用的個人資料Bean
 */
public class PersonBean implements Person{
	private String name ;
	private int likeCount;
	
	@Override
	public void setLikeCount(int like) {
		this.likeCount = like;
	}

	public int getLikeCount() {
		return this.likeCount;
	}
	
	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}
}

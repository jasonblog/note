package c13.proxy.protect;
/**
 * 個人資料介面
 */
public interface Person {
	void setLikeCount(int like);
	int getLikeCount();
	String getName();
	void setName(String name);
}

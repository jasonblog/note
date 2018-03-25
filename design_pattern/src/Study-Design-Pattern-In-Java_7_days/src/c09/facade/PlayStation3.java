package c09.facade;
/**
 * PS3 
 */
public class PlayStation3 extends Electronics {
	private String cd ; // 目前播放的CD

	// 放入CD片
	public void putCd(String cd) {
		this.cd = cd;
	}

	public String getCd() {
		return cd;
	}

	// 播放CD
	public void play(){
		System.out.println(this.getClass().getSimpleName()  + "開始播放 " + cd);
	}
	
	@Override
	public void showStatus(){
		super.showStatus();
		if(isPowerOn()){
			System.out.println(this.getClass().getSimpleName()  + "目前放入cd: " + cd);
		}
	}
}

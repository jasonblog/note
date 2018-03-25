package c09.facade;
/**
 * 液晶電視
 */
public class Television  extends Electronics {
	private int sound = 50 ;         // 音量 (0為靜音，100為最大)
	private String source = "tvBox"; // 訊號源
	private int channel = 9;         // 電視頻道
	
	// 選擇訊號源
	public void switchSource(String source) {
		this.source = source;
	}
	
	// 調整音量
	public void setSound(int sound) {
		this.sound = sound;
	}
	
	// 選電視頻道
	public void switchChannel(int channel) {
		this.channel = channel;
	}
	
	// 看目前觀看電視頻道
	public void showTV() {
		System.out.println("目前觀看的是頻道: " + channel);
	}

	@Override
	public void showStatus(){
		super.showStatus();
		if(isPowerOn()){
			System.out.print(this.getClass().getSimpleName()  + "音量為: " + sound);
			if(source.equals("tvBox")){
				System.out.println(", 頻道: " +  channel);
			}
			
			if(source.equals("ktv")){
				System.out.println(", ktv播放中");
			}
			
			if(source.equals("ps")){
				System.out.println(", ps畫面顯示中");
			}
		}
	}
	
	public int getSound() {
		return sound;
	}

	public String getSource() {
		return source;
	}

	public int getChannel() {
		return channel;
	}
}

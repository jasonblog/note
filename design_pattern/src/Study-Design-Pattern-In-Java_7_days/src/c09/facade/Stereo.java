package c09.facade;
/**
 * 環繞音響
 */
public class Stereo extends Electronics {
	private int sound = 50 ;         // 音量 (0為靜音，100為最大)
	
	// 調整音量
	public void setSound(int sound) {
		this.sound = sound;
	}

	public int getSound() {
		return sound;
	}
	
	@Override
	public void showStatus(){
		super.showStatus();
		if(isPowerOn()){
			System.out.println(this.getClass().getSimpleName()  + "音量為: " + sound);
		}
	}
}

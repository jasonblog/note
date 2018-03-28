package c09.facade;
/**
 * 管理影音設備的外觀類別(Facade)
 */
public class VedioRoomFacade {
	// 房間內總共有這些影音設備
	Television tv = new Television();
	Stereo stereo = new Stereo();
	PlayStation3 ps = new PlayStation3();
	KTVsystem ktv = new KTVsystem();
	
	/**
	 * 準備用ps3看電影
	 */
	public void readyPlayMovie(String cd){
		stereo.powerOn(); // 音響要先開
		tv.powerOn();     // 接著開電視
		setSound(50);     // 設定音量
		tv.switchSource("ps"); //電視切到ps訊號源
		ps.powerOn();     // 開ps3
		ps.putCd(cd);     // 放入cd
	}
	
	/**
	 * 用ps3放電影
	 */
	public void playMovie(){
		if(ps.isPowerOn()){
			ps.play();
		}
	}
	// 看目前觀看電視頻道
	public void showTv(){
		tv.showTV();
	}
	
	/**
	 * 關閉全部設備
	 */
	public void turnOffAll(){
		stereo.powerOff(); // 音響要先關
		ktv.powerOff();    // KTV有開的話第二個關
		ps.powerOff();     // 電視如果先關你就看不到ps的畫面了
		tv.powerOff();     // 電視最後關
	}
	
	/**
	 * 看電視
	 */
	public void watchTv(){
		tv.powerOn();     // 開電視
		tv.switchSource("tvBox"); //電視切到電視訊號源
	}
	
	// 選電視頻道
	public void switchChannel(int channel) {
		tv.switchChannel(channel);
	}
	
	/**
	 * 準備唱KTV
	 */
	public void readyKTV(){
		stereo.powerOn(); // 音響要先開
		ktv.powerOn();    // 開啟ktv點唱機
		tv.powerOn();     // 開電視
		setSound(50);     // 設定音量
		tv.switchSource("ktv"); //電視切到ps訊號源
	}
	/**
	 * ktv點歌
	 * @param song
	 */
	public void selectSong(String song){
		if(ktv.isPowerOn()){
			ktv.selectSong(song);
		}
	}
	/**
	 * ktv播放歌曲
	 */
	public void playSong(){
		if(ktv.isPowerOn()){
			ktv.playSong();
		}
	}
	/**
	 * 設定音量
	 * @param soundLevel
	 */
	public void setSound(int soundLevel){
		if(tv.isPowerOn()){
			tv.setSound(soundLevel);
		}
		if(stereo.isPowerOn()){
			stereo.setSound(soundLevel);
		}
	}
	
	/**
	 * 顯示所有機器的狀態
	 */
	public void showAllStatus(){
		tv.showStatus();
		stereo.showStatus();
		ps.showStatus();
		ktv.showStatus();
	}
}

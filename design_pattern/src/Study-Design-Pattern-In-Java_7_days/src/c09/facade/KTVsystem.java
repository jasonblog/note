package c09.facade;
	/**
	 * KTV點歌機
	 */
	public class KTVsystem extends Electronics {
		private String song; // 歌曲
		
		//選歌
		public void selectSong(String song){
			this.song = song;
		}
		//播放
		public void playSong(){
			System.out.println(this.getClass().getSimpleName() + "播放 " + song );
		}
	}

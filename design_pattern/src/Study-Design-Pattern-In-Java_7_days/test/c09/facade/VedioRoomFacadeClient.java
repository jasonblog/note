package c09.facade;

import org.junit.Test;

import c09.facade.VedioRoomFacade;

/**
 * 外觀模式-測試(Client)
 */
public class VedioRoomFacadeClient {
	VedioRoomFacade superRemote = new VedioRoomFacade();
	
	@Test 
	public void test(){
		System.out.println("============外觀模式測試============");
		System.out.println("以下測試碼可以看出使用外觀模式後，操作步驟會比一個一個類別進去操作方便取多");

		System.out.println("---看電影---");
		// 看電影
		superRemote.readyPlayMovie("Life of Pi");
		superRemote.playMovie();
		superRemote.showAllStatus();
		System.out.println();
		System.out.println("---關機器---");
		// 關閉機器
		superRemote.turnOffAll();
		superRemote.showAllStatus();
		
		System.out.println("---看電視---");
		// 看電視
		superRemote.watchTv();
		superRemote.showTv();
		superRemote.switchChannel(20); //換頻道
		superRemote.showTv();
		superRemote.turnOffAll();
		System.out.println();
		
		System.out.println("---唱ktv---");
		// 唱ktv
		superRemote.readyKTV();
		superRemote.selectSong("Moon");
		superRemote.playSong();
		superRemote.showAllStatus();

	} 
}

package c16.chainOfResponsibility;

import c16.chainOfResponsibility.applyInCompany.ApplyRequest;
import c16.chainOfResponsibility.applyInCompany.CommonManager;
import c16.chainOfResponsibility.applyInCompany.GeneralManager;
import c16.chainOfResponsibility.applyInCompany.Majordomo;
import c16.chainOfResponsibility.applyInCompany.Manager;

/**
 * 責任鏈模式-測試
 */
public class ManagerClient {

	
	public static void main(String[] args) {
		System.out.println("============責任鏈模式測試============");

		Manager pm = new CommonManager("PM經理");
		Manager gl = new Majordomo("總監");
		Manager gm = new GeneralManager("總經理");
		
		// 設定上級，可隨實際需求更改
		pm.setSuperior(gl);
		gl.setSuperior(gm);
		
		ApplyRequest request = new ApplyRequest();
		request.setRequestType("請假");
		request.setRequestContent("小菜請假");
		request.setRequestCount(2);
		pm.apply(request);
		
		request.setRequestCount(4);
		pm.apply(request);

		request.setRequestType("加薪");
		request.setRequestContent("小菜加薪");
		request.setRequestCount(2000);
		pm.apply(request);

		request.setRequestCount(900);
		pm.apply(request);


	}

}

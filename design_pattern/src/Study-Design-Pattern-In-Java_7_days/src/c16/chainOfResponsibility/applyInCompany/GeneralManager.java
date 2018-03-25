package c16.chainOfResponsibility.applyInCompany;

/**
 * 總經理(Concretehandler)
 *
 */
public class GeneralManager extends Manager {

	public GeneralManager(String name) {
		super(name);
	}

	@Override
	public void apply(ApplyRequest request) {
		if(request.getRequestType().equals("請假")){
			System.out.print(request.getRequestType() + ":" + request.getRequestContent());
			System.out.println(" " + request.getRequestCount()  + "天 被" + name + "批准");
		} else {
			if(request.getRequestCount() <= 1000){
				System.out.print(request.getRequestType() + ":" + request.getRequestContent());
				System.out.println(" " + request.getRequestCount() + "元 被"  + name + "批准");
			} else {
				System.out.print(request.getRequestType() + ":" + request.getRequestContent());
				System.out.println(" " + request.getRequestCount() + "元 被"  + name + "駁回");
			}
		}
	}

}

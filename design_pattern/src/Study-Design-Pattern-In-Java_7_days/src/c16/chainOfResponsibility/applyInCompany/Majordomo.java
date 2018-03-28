package c16.chainOfResponsibility.applyInCompany;

/**
 * 總監(Concretehandler)
 *
 */
public class Majordomo extends Manager {

	public Majordomo(String name) {
		super(name);
	}

	@Override
	public void apply(ApplyRequest request) {
		//5天以下的假就批准，其他丟給上級
		if(request.getRequestType().equals("請假") && request.getRequestCount() <= 5){
			System.out.print(request.getRequestType() + ":" + request.getRequestContent());
			System.out.println(" " + request.getRequestCount() + "天 被" + name + "批准");
		} else {
			if(superior != null){
				superior.apply(request);
			}
		}
	}

}

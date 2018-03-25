package c16.chainOfResponsibility.applyInCompany;
/**
 * 提出申請(Request)
 */
public class ApplyRequest {
	/**
	 * 申請類別
	 */
	private String requestType;
	/**
	 * 申請內容
	 */
	private String requestContent;
	/**
	 * 申請數
	 */
	private int requestCount;
	
	
	
	public String getRequestType() {
		return requestType;
	}
	public void setRequestType(String requestType) {
		this.requestType = requestType;
	}
	public String getRequestContent() {
		return requestContent;
	}
	public void setRequestContent(String requestContent) {
		this.requestContent = requestContent;
	}
	public int getRequestCount() {
		return requestCount;
	}
	public void setRequestCount(int requestCount) {
		this.requestCount = requestCount;
	}
}

package c11.composite.organizationTree;

import java.util.ArrayList;
import java.util.List;

/**
 * 有分支的協會(Composite)
 *
 */
public class Association extends AbstractAssociation{


	private List<AbstractAssociation> branchs = new ArrayList<>();

	
	public Association(String name) {
		super(name);
	}
	
	/**
	 * 增加轄下分會或部門
	 * @param a
	 */
	public void add(AbstractAssociation a){
		branchs.add(a);
	};
	
	/**
	 * 移除增加轄下分會或部門
	 */
	public void remove(AbstractAssociation a){
		branchs.remove(a);
	};
	/**
	 * 印出組織結構圖
	 */
	public void display(int depth){
		for(int i = 0 ; i < depth ; i++){
			System.out.print('-');
		}
		System.out.println(name);
		for(AbstractAssociation a : branchs){
			a.display(depth+2);
		}
	};
	
	/**
	 * 印出組織職責
	 */
	public void lineOfDuty(){
		for(AbstractAssociation a : branchs){
			a.lineOfDuty();
		}
	};


}
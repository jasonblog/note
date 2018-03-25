package c22.memento;
/**
 * 要備份的物件
 */
public class GameRole {
	private int hp = 100;
	private int atk = 100;
	private int def = 100;
	private String name = "第六天魔王";

	public RoleStateMemo save(){
		return new RoleStateMemo(hp,atk,def);
	}
	

	/**
	 * 
	 */
	public void fight(){
		hp = 30;
		System.out.println(name + "剩下30%血量，出大招把隊伍打的半死");
	}
	
	public void stateDisplay(){
		System.out.println(name+"的狀態：");
		System.out.print("hp=" + hp);
		System.out.print(", atk=" + atk);
		System.out.println(", def=" + def);
		System.out.println();
	}
	
	
	
	public void load(RoleStateMemo memo){
		this.hp = memo.getHp();
		this.atk = memo.getAtk();
		this.def = memo.getDef();
	}
	public int getHp() {
		return hp;
	}
	public void setHp(int hp) {
		this.hp = hp;
	}
	public int getAtk() {
		return atk;
	}
	public void setAtk(int atk) {
		this.atk = atk;
	}
	public int getDef() {
		return def;
	}
	public void setDef(int def) {
		this.def = def;
	}
}

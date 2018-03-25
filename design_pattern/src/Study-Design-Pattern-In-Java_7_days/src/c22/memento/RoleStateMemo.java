package c22.memento;
/**
 * 備忘錄物件
 */
public class RoleStateMemo {
	private int hp;
	private int atk;
	private int def;

	public RoleStateMemo(int hp, int atk, int def) {
		this.hp = hp;
		this.atk = atk;
		this.def = def;
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

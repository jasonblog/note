package c10.template.maze;
/**
 * 簡單的迷宮(ConcreteTemplate)
 */
public class EazyMaze extends MazeTemplate{

	public EazyMaze() {
		super.difficulty = 1; // 沒限制等級
	}

	@Override
	void createMaze() {
		System.out.println("準備100*100的迷宮");
		System.out.println("安排10隻小怪物");
		System.out.println("安排等級10的BOSS");
		System.out.println("拔草整理場地");
		System.out.println("簡易迷宮準備完成!!!");
	}

	@Override
	void start() {
		System.out.println("冒險者開始進行簡單迷宮的冒險");
	}

}

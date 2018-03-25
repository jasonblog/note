package c10.template.maze;
/**
 * 困難的迷宮(ConcreteTemplate)
 */
public class DifficultMaze extends MazeTemplate{

	public DifficultMaze() {
		super.isDoubleMaze = true; // 困難模式有隱藏關卡
		super.difficulty = 50; // 50級以上才能進入困難迷宮
	}

	@Override
	void createMaze() {
		System.out.println("準備1000*1000的迷宮(包刮隱藏迷宮)");
		System.out.println("安排打不完的小怪物");
		System.out.println("安排等級50的中BOSS，100隻");
		System.out.println("安排等級120的超級BOSS，放隱藏迷宮的保物");
		System.out.println("拔草整理場地，重新油漆牆壁，擺放各種陷阱，擺放假屍體");
		System.out.println("困難迷宮準備完成!!!");
	}

	@Override
	void start() {
		System.out.println("冒險者開始進行困難迷宮的冒險");
	}

}

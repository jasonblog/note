package c10.template;

import org.junit.Test;

import c10.template.adventurer.Adventurer;
import c10.template.adventurer.GundamJustice;
import c10.template.adventurer.Sabar;
import c10.template.maze.DifficultMaze;
import c10.template.maze.EazyMaze;
import c10.template.maze.MazeTemplate;

/**
 * 樣版模式-測試
 */
public class MazeTest {
	Adventurer sabar = new Sabar(); // 等級10的劍士
	Adventurer justice = new GundamJustice(); // 等級100的正義鋼彈
	
	MazeTemplate easyMaze = new EazyMaze();		 // 簡單迷宮
	MazeTemplate hardMaze = new DifficultMaze(); // 困難迷宮
	
	@Test
	public void test(){
		System.out.println("============樣版模式測試============");

		System.out.println(" =====困難迷宮======");
		sabar = hardMaze.adventure(sabar);	
		System.out.println(" =====簡單迷宮練功======");
		sabar = easyMaze.adventure(sabar);
		
		// 練功後劍士可以進行困難迷宮
		System.out.println(" =====困難迷宮測試======");
		sabar = hardMaze.adventure(sabar);
		justice = hardMaze.adventure(justice); 

	}

}

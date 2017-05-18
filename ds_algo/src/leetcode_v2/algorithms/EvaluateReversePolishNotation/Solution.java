import java.util.*;
public class Solution {
	public int evalRPN(List<String> tokens) {
		LinkedList<String> repo = new LinkedList<String>();
		for (String token : tokens) {
			if (isOperator(token)) {
				String num1 = repo.pop();
				String num2 = repo.pop();
				int result = expr(num2, num1, token);
				repo.push(String.valueOf(result));
			} else {
				repo.push(token);
			}
		}
		return Integer.parseInt(repo.pop());
	}
	private boolean isOperator(String s) {
		return s.equals("+")
			|| s.equals("-")
			|| s.equals("*")
			|| s.equals("/");
	}
	private int expr(String num1, String num2, String op) {
		int a = Integer.parseInt(num1);
		int b = Integer.parseInt(num2);
		switch (op) {
			case "+":
				return a + b;
			case "-":
				return a - b;
			case "*":
				return a * b;
			case "/":
				return a / b;
			default:
				throw new IllegalArgumentException();
		}
	}
	public static void main(String[] args) {
		List<String> s1 = Arrays.asList("2", "1", "+", "3", "*");
		List<String> s2 = Arrays.asList("4", "13", "5", "/", "+");
		Solution solution = new Solution();
		System.out.println(solution.evalRPN(s1));
		System.out.println(solution.evalRPN(s2));
	}
}

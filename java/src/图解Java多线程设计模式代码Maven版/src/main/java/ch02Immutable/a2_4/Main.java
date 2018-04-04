package ch02Immutable.a2_4;

public class Main {
    public static void main(String[] args) {
        // 创建实例
        UserInfo userinfo = new UserInfo("Alice", "Alaska");

        // 显示
        System.out.println("userinfo = " + userinfo);

        // 修改状态
        StringBuffer info = userinfo.getInfo();
        info.replace(12, 17, "Bobby");  // 12到17是字符串"Alice"的位置

        // 再次显示
        System.out.println("userinfo = " + userinfo);
    }
}
/*
userinfo = [ UserInfo: <info name="Alice" address="Alaska" /> ]
userinfo = [ UserInfo: <info name="Bobby" address="Alaska" /> ]
 */
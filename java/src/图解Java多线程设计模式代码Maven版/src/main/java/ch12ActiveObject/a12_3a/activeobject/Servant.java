package ch12ActiveObject.a12_3a.activeobject;

public class Servant implements ActiveObject {
    public Result<String> search(String word) {
        System.out.print("search(" + word + ")");
        for (int i = 0; i < 50; i++) {
            System.out.print(".");
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
            }
        }
        System.out.println("found.");
        String url = "http://somewhere/" + word + ".html"; // dummy URL
        return new RealResult<String>(url);
    }
}

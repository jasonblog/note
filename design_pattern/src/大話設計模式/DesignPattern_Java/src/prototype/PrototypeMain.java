package prototype;

public class PrototypeMain
{

    public static void main(String[] args)
    {
        Resume linResume = new Resume("Lincoln");
        linResume.setAge("23");
        linResume.setSex("M");

        System.out.println(linResume.toString());

        Resume linResume2 = linResume.clone();
        linResume2.setAge("22");
        System.out.println(linResume2.toString());
    }
}

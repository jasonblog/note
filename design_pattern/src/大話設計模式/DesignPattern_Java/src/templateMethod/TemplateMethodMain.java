package templateMethod;

public class TemplateMethodMain
{

    /**
     * @lincoln liu
     */
    public static void main(String[] args)
    {
        TestPaper pa = new TestPaperA();
        pa.question1();
        pa.question2();

        TestPaper pb = new TestPaperB();
        pb.question1();
        pb.question2();
    }

}

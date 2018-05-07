package interpreter;

public class Note extends Expression
{

    @Override
    public void excute(String key, double value)
    {
        String note = null;

        if (key.equals("C")) {
            note = "1";
        }

        if (key.equals("D")) {
            note = "2";
        }

        if (key.equals("E")) {
            note = "3";
        }

        if (key.equals("F")) {
            note = "4";
        }

        if (key.equals("G")) {
            note = "5";
        }

        if (key.equals("A")) {
            note = "6";
        }

        if (key.equals("B")) {
            note = "7";
        }

        System.out.print(" " + note + " ");
    }

}

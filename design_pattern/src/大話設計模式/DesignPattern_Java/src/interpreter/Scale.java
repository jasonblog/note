package interpreter;

public class Scale extends Expression
{

    @Override
    public void excute(String key, double value)
    {
        String scale = null;
        int intKey = (int)value;

        if (intKey == 1) {
            scale = "low scale";
        }

        if (intKey == 2) {
            scale = "mid scale";
        }

        if (intKey == 3) {
            scale = "high scale";
        }

        System.out.print(" " + scale + " ");
    }

}

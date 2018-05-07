package interpreter;

public abstract class Expression
{

    public void interpret(PlayContext context)
    {
        if (context.getContext().length() == 0) {
            return;
        } else {
            String playKey =  context.getContext().substring(0, 1);
            context.setContext(context.getContext().substring(2));
            double  playValue = Double.parseDouble(context.getContext().substring(0,
                                                   context.getContext().indexOf(" ")));
            context.setContext(context.getContext().substring(
                                   context.getContext().indexOf(" ") + 1));

            excute(playKey, playValue);
        }
    }

    public abstract void excute(String key, double value);

}

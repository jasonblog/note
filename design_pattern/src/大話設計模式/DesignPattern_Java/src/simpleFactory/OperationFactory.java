package simpleFactory;

public class OperationFactory
{

    public static Operation createOperation(char operate)
    {
        Operation oper = null;

        switch (operate) {
        case '+':
            oper = new OperationAdd();
            break;

        case '-':
            oper = new OperationSub();
            break;
        }

        return oper;
    }

}

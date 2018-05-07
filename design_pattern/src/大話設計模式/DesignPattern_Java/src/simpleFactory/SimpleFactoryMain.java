package simpleFactory;

public class SimpleFactoryMain
{

    /**
     * @Lincoln Liu
     */
    public static void main(String[] args)
    {
        Operation operAdd = OperationFactory.createOperation('+');
        Operation operSub = OperationFactory.createOperation('-');

        operAdd.setA(11);
        operAdd.setB(22);
        System.out.println(operAdd.getResult());

        operSub.setA(11);
        operSub.setB(22);
        System.out.println(operSub.getResult());
    }
}

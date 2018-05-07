package composite;

public class CompositeMain
{

    /**
     * @param args
     */
    public static void main(String[] args)
    {
        ConcreteCompany root = new
        ConcreteCompany("The Head Office Of Our Company In Beijing");
        root.add(new HRDepartment("Head Office HR"));
        root.add(new FinanceDepartment("Head Office Finance"));

        ConcreteCompany comp = new ConcreteCompany("The leaf company in shanghai");
        comp.add(new HRDepartment("shanghai office HR"));
        comp.add(new FinanceDepartment("shanghai office finance"));
        root.add(comp);

        ConcreteCompany comp1 = new
        ConcreteCompany("the office of shanghai company in nanjing");
        comp1.add(new HRDepartment("nanjing office HR"));
        comp1.add(new FinanceDepartment("nanjing office finance"));
        comp.add(comp1);

        ConcreteCompany comp2 = new
        ConcreteCompany("the office of shanghai company in hangzhou");
        comp2.add(new HRDepartment("hangzhou office HR"));
        comp2.add(new FinanceDepartment("hangzhou office finance"));
        comp.add(comp2);

        System.out.println("the structure of our company:");
        root.display(1);

        System.out.println("the duty of every department:");
        root.lineOfDuty();
    }

}

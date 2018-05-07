package chainOfResponsibility;

public class Manager extends RequestHandler
{

    @Override
    public void handleRequest(Request que)
    {
        if (que.getType().equals("increase salary")) {
            System.out.println("Manager  agree the salary increase request");
        } else {
            System.out.println("Director has no right to handle it, Pls ask help from Manager!");
            this.getNextHandler().handleRequest(que);
        }
    }

}

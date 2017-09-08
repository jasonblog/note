public class test
{
    public static void main(String[] args)
    {
        try {
            double[][] positions = new double[][] {{1.0, 1.0}, {3.0, 1.0}};
            double[] distances = new double[] {1.0, 1.0};
            double[] expectedPosition = new double[] {2.0, 1.0};
            double acceptedDelta = 0.5;
            new TrilaterationTest(positions, distances, expectedPosition, acceptedDelta);
        } catch (Exception ex) {
            System.out.println("something wrong");
        }

    }
}

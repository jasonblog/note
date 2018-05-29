# 四元數 diff


```java
public class quat_test
{
    static public double[] quatMultiply(double a[], double b[])
    {
        assert(a.length == 4);
        assert(b.length == 4);

        double c[] = new double[4];

        c[3] = (b[3] * a[3]) - (b[0] * a[0]) - (b[1] * a[1]) - (b[2] * a[2]);
        c[0] = (b[3] * a[0]) + (b[0] * a[3]) - (b[1] * a[2]) + (b[2] * a[1]);
        c[1] = (b[3] * a[1]) + (b[1] * a[3]) - (b[2] * a[0]) + (b[0] * a[2]);
        c[2] = (b[3] * a[2]) + (b[2] * a[3]) - (b[0] * a[1]) + (b[1] * a[0]);

        double m = Math.sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2] + c[3] * c[3]);
        c[0] /= m;  // x
        c[1] /= m;  // y
        c[2] /= m;  // z
        c[3] /= m;  // w

        return c;
    }

    public static void main(String[] args)
    {
        double q_init[] = new double[4];
        q_init[3] = 1;
        q_init[0] = 0;
        q_init[1] = 0;
        q_init[2] = 0;

        double diff_quat[] = new double[4];
        double new_quat[] = new double[4];
        double pre_quat[] = new double[4];

        pre_quat[3] = 0.999;
        pre_quat[0] = -0;
        pre_quat[1] = -0.044;
        pre_quat[2] = -0;

        new_quat[3] = 0.956;
        new_quat[0] = 0;
        new_quat[1] = 0.292;
        new_quat[2] = 0;

        diff_quat = quatMultiply(new_quat, pre_quat);
        // diff_quat = quatMultiply(pre_quat, new_quat);
        System.out.println("diff_quat w " + diff_quat[3] + " x " + diff_quat[0] +
                         " y " + diff_quat[1] + " z " + diff_quat[2]);

        double result[] = new double[4];
        result = quatMultiply(q_init, diff_quat);

        System.out.println("result w " + result[3] + " x " + result[0] +
                         " y " + result[1] + " z " + result[2]);

    }
}
```
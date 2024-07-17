public class inMode {
    public static void main(String[] args) {
        double z = 10.5;
        double result = manipulate(5, z); // Actual Parameters: 5, 10.5
        System.out.println(String.format("z: %.2f, result: %.2f", z, result));
    }

    public static double manipulate(int x, double y) {
        y = y * 2;
        return x + y;
    }
}

using System;

namespace ParameterPassing
{
    class Program
    {
        static void Main(string[] args)
        {
            int x = 7;
            int y = 10;
            Console.WriteLine("Before calling DiffModes: x = {0}, y = {1}", x, y);
            DiffModes(x, out y);
            Console.WriteLine("After calling DiffModes: x = {0}, y = {1}", x, y);
        }

        static void DiffModes(int x, out int y)
        {
            x = x * 2;
            y = x + 1;
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace CompareFileDates
{
    class Program
    {
        public static readonly int SAME_DATE = 0;
        public static readonly int FILE_1_OLDER = 1;
        public static readonly int FILE_2_OLDER = 2;
        public static readonly int EXIT_ERROR = 3;

        static int Main(string[] args)
        {
            if (args.Length == 1)
            {
                String one = (String)args.GetValue(0);
                if (one.Contains("help"))
                {
                    System.Console.WriteLine("Usage: CompareFileDates.exe <file1> <file2>");
                    System.Console.WriteLine("    Compares the last write times of two files");
                    System.Console.WriteLine("Return values:");
                    System.Console.WriteLine("    0: Files last written to at same time");
                    System.Console.WriteLine("    1: file1 is older");
                    System.Console.WriteLine("    2: file2 is older");
                    System.Console.WriteLine("    3: An error occurred (or this help message was printed)");
                    return Program.EXIT_ERROR;
                }
            }
            if (args.Length != 2)
            {
                System.Console.WriteLine("Incorrect number of arguments, need two arguments");
                return Program.EXIT_ERROR;
            }

            String File1 = (String)args.GetValue(0);
            String File2 = (String)args.GetValue(1);

            if (!File.Exists(File1))
            {
                System.Console.WriteLine("File does not exist: " + File1);
                return Program.EXIT_ERROR;
            }
            if (!File.Exists(File2))
            {
                System.Console.WriteLine("File does not exist: " + File2);
                return Program.EXIT_ERROR;

            }

            DateTime Date1 = File.GetLastWriteTimeUtc(File1);
            DateTime Date2 = File.GetLastWriteTimeUtc(File2);

            int result = Date1.CompareTo(Date2);
            if (result < 0)
            {
                return Program.FILE_1_OLDER;
            }
            if (result > 0)
            {
                return Program.FILE_2_OLDER;
            }
            return Program.SAME_DATE;
        }
    }
}

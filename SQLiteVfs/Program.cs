using System;
using System.Data.SQLite;

namespace SQLiteVfs
{
    class Program
    {
        static void Main(string[] args)
        {
            using (var db = new InMemoryDatabase())
            {
                var input = new byte[10];
                for (int i = 0; i < input.Length; ++i) input[i] = (byte)i;
                db.SetData("kiskakas", input);

                byte[] output;
                db.GetData("kiskakas", out output);
            }
        }
    }
}

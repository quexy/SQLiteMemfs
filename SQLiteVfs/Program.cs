using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.SQLite;
using System.Runtime.InteropServices;

namespace SQLiteVfs
{
    class Program
    {
        static void Main(string[] args)
        {
            using (var db = new InMemoryDatabase())
            {
                byte[] data;
                db.GetData("kiskakas", out data);
            }
        }
    }
}

using System;
using System.Linq;
using System.Data.SQLite;
using System.Text;

namespace SQLiteVfs
{
    class Program
    {
        static void Main(string[] args)
        {
            using (var db = new InMemoryDatabase())
            {
                byte[] data;

                CreateDatabase("X:\\db1");

                db.GetData("X:\\db1", out data);
                db.SetData("X:\\db2", data);

                string str1 = new String(data.Select(b => (char)b).ToArray());
                InsertData("X:\\db2");

                db.GetData("X:\\db2", out data);
                db.SetData("X:\\db3", data);

                var str2 = new String(data.Select(b => (char)b).ToArray());
                ReadData("X:\\db3");
            }
        }

        private static void CreateDatabase(string dbFile)
        {
            var connStr = string.Format("Data Source={0};Version=3;FailIfMissing=False;", dbFile);
            using (var connection = new SQLiteConnection(connStr))
            {
                connection.Open();

                var tableCmd = "create table stuff(id none not null primary key, title nvarchar(255) not null, data blob null)";
                using (var command = new SQLiteCommand(tableCmd, connection))
                {
                    command.ExecuteNonQuery();
                }
            }
        }

        private static void InsertData(string dbFile)
        {
            var connStr = string.Format("Data Source={0};Version=3;FailIfMissing=False;", dbFile);
            using (var connection = new SQLiteConnection(connStr))
            {
                connection.Open();

                var insertCmd = "insert into stuff(id, title, data) values (@id, @title, @data)";
                for (int i = 1; i < 11; ++i)
                {
                    using (var command = new SQLiteCommand(insertCmd, connection))
                    {
                        var guid = Guid.NewGuid();
                        command.Parameters.Add(new SQLiteParameter("@id", guid));
                        command.Parameters.Add(new SQLiteParameter("@title", guid.ToString("B")));
                        var data = "".PadRight(i, (i - 1).ToString()[0]).PadLeft(10, '.');
                        command.Parameters.Add(new SQLiteParameter("@data", Encoding.ASCII.GetBytes(data)));
                        command.ExecuteNonQuery();
                    }
                }
            }
        }

        private static void ReadData(string dbFile)
        {
            var connStr = string.Format("Data Source={0};Version=3;FailIfMissing=True;ReadOnly=True;", dbFile);
            using (var connection = new SQLiteConnection(connStr))
            {
                connection.Open();

                var queryCmd = "select id, title, data from stuff";
                using (var command = new SQLiteCommand(queryCmd, connection))
                {
                    using (var reader = command.ExecuteReader())
                    {
                        while (reader.Read())
                        {
                            Console.WriteLine("| {0} | {1} | {2} |",
                                reader.GetGuid(0),
                                reader.GetString(1),
                                Encoding.ASCII.GetString((byte[])reader.GetValue(2))
                            );
                        }
                    }
                }
            }
        }
    }
}

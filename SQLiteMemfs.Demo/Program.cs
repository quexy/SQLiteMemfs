using System;
using System.Data.SQLite;
using System.Text;
using Memfs = System.Data.SQLite.SQLiteMemfs;

namespace SQLiteMemfs.Demo
{
    class Program
    {
        static void Main(string[] args)
        {
            using (var memfs = new Memfs())
            {
                CreateDatabase("X:\\db1");

                using (var iStream = memfs.GetStream("X:\\db1"))
                using (var oStream = memfs.GetStream("X:\\db2"))
                    iStream.CopyTo(oStream);

                InsertData("X:\\db2");

                using (var iStream = memfs.GetStream("X:\\db2"))
                using (var oStream = memfs.GetStream("X:\\db3"))
                    iStream.CopyTo(oStream);

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
            var connStr = string.Format("Data Source={0};Version=3;FailIfMissing=True;", dbFile);
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
                        command.Parameters.Add(new SQLiteParameter("@data", Encoding.Unicode.GetBytes(data)));
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
                                Encoding.Unicode.GetString((byte[])reader.GetValue(2))
                            );
                        }
                    }
                }
            }
        }
    }
}

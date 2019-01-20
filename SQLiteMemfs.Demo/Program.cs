using System;
using System.Data.SQLite;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Memfs = System.Data.SQLite.SQLiteMemfs;

namespace SQLiteMemfs.Demo
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                var guid = Guid.NewGuid();
                using (var memfs = new Memfs())
                {
                    CreateDatabase("X:\\db1");

                    CloneDatabase(memfs, "X:\\db1", "X:\\db2");

                    InsertData("X:\\db2", guid);

                    CloneDatabase(memfs, "X:\\db2", "X:\\db3");

                    const int msec = 1000;
                    Console.WriteLine("Parallel Update...");
                    using (var cancellation = new CancellationTokenSource(5 * msec))
                    {
                        var task1 = Task.Run(() => AlterData("X:\\db3", guid.Increase(2), cancellation.Token.WaitHandle));
                        var task2 = Task.Run(() => AlterData("X:\\db3", guid.Increase(4), cancellation.Token.WaitHandle));

                        try { Task.WaitAll(task1, task2); } catch { /* handle later */ }

                        if (task1.IsFaulted) Console.WriteLine("AlterData (1) failed: {0}", task1.Exception);
                        if (task2.IsFaulted) Console.WriteLine("AlterData (2) failed: {0}", task2.Exception);
                    }

                    ReadData("X:\\db3");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }

            if (System.Diagnostics.Debugger.IsAttached)
            {
                Console.Write("Press any key to continue . . . ");
                Console.ReadKey(true); Console.WriteLine();
            }
        }

        private static void CreateDatabase(string dbFile)
        {
            Console.WriteLine("Create database; file '{0}'...", dbFile);
            var connStr = string.Format("Data Source={0};Version=3;FailIfMissing=False;", dbFile);
            using (var connection = new SQLiteConnection(connStr))
            {
                connection.Open();

                var tableCmd = "CREATE TABLE stuff("
                    + "  id NONE NOT NULL PRIMARY KEY"
                    + ", title NVARCHAR(255) NOT NULL"
                    + ", data BLOB NULL"
                    + ")";
                using (var command = new SQLiteCommand(tableCmd, connection))
                {
                    command.ExecuteNonQuery();
                }
            }
        }

        private static void CloneDatabase(Memfs memfs, string source, string target)
        {
            Console.WriteLine("Clone database; from '{0}' into '{1}'...", source, target);
            using (var iStream = memfs.GetStream(source))
            using (var oStream = memfs.GetStream(target))
                iStream.CopyTo(oStream);
        }

        private static void InsertData(string dbFile, Guid guid)
        {
            Console.WriteLine("Insert data; file '{0}'...", dbFile);
            var connStr = string.Format("Data Source={0};Version=3;FailIfMissing=True;", dbFile);
            using (var connection = new SQLiteConnection(connStr))
            {
                connection.Open();

                var insertCmd = "INSERT INTO stuff(id, title, data) VALUES (@id, @title, @data)";
                for (int i = 1; i < 11; ++i)
                {
                    using (var command = new SQLiteCommand(insertCmd, connection))
                    {
                        command.Parameters.Add(new SQLiteParameter("@id", guid));
                        command.Parameters.Add(new SQLiteParameter("@title", guid.ToString("B")));
                        var data = "".PadRight(i, (i - 1).ToString()[0]).PadLeft(10, '.');
                        command.Parameters.Add(new SQLiteParameter("@data", Encoding.Unicode.GetBytes(data)));
                        command.ExecuteNonQuery();
                    }

                    guid = guid.NextGuid();
                }
            }
        }

        private static void ReadData(string dbFile)
        {
            Console.WriteLine("Read data; file '{0}'...", dbFile);
            var connStr = string.Format("Data Source={0};Version=3;FailIfMissing=True;ReadOnly=True;", dbFile);
            using (var connection = new SQLiteConnection(connStr))
            {
                connection.Open();

                var queryCmd = "SELECT id, title, data FROM stuff";
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

        private static void AlterData(string dbFile, Guid guid, WaitHandle handle)
        {
            Console.WriteLine("Alter data; id: {0}", guid);
            var connStr = string.Format("Data Source={0};Version=3;FailIfMissing=True;ReadOnly=True;", dbFile);
            using (var connection = new SQLiteConnection(connStr))
            {
                connection.Open();
                using (var transaction = connection.BeginTransaction())
                using (var command = connection.CreateCommand())
                {
                    command.Transaction = transaction;

                    command.CommandText = "UPDATE stuff SET data=@data WHERE id=@id";
                    command.Parameters.Add(new SQLiteParameter("@id", guid));
                    var data = Encoding.Unicode.GetBytes("Changed".PadRight(10, ' '));
                    command.Parameters.Add(new SQLiteParameter("@data", data));

                    command.ExecuteNonQuery();

                    handle.WaitOne();

                    transaction.Commit();
                }
                Console.WriteLine("    Data ({0}) changed", guid);
            }
        }
    }

    static class GuidExtensions
    {
        public static Guid NextGuid(this Guid guid)
        {
            return Increase(guid, 1);
        }

        public static Guid Increase(this Guid guid, int amount)
        {
            int carry = amount;
            var bytes = guid.ToByteArray();
            for (var i = 0; i < bytes.Length; ++i)
            {
                int value = bytes[i] + (carry & 0xff);
                bytes[i] = (byte)(value & 0xff);
                carry = (carry >> 8) + (value >> 8);
            }
            return new Guid(bytes);
        }
    }
}

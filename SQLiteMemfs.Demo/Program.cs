using System;
using System.Data.SQLite;
using System.Diagnostics;
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
                using (var memfs = new Memfs())
                {
                    CheckStreamReadWrite(memfs, "C:\\temp\\test.bin", "X:\\test.bin", 6000);
                }

                var guid = Guid.NewGuid();
                using (var memfs = new Memfs())
                {
                    CreateDatabase("X:\\db1");

                    CloneDatabase(memfs, "X:\\db1", "X:\\db2");

                    InsertData("X:\\db2", guid);

                    CloneDatabase(memfs, "X:\\db2", "X:\\db3");

                    ParallelUpdate(guid, "X:\\db1", "X:\\db2");
                    ParallelUpdate(guid, "X:\\db3", "X:\\db3");

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

        private static void CheckStreamReadWrite(Memfs memfs, string fileName, string memfsName, int bufferSize)
        {
            using (var fileStream = System.IO.File.OpenRead(fileName))
            using (var memfsStream = memfs.GetStream(memfsName))
            {
                //memfsStream.SetLength(fileStream.Length);

                int amount = 0;
                var buffer = new byte[bufferSize];
                while ((amount = fileStream.Read(buffer, 0, buffer.Length)) > 0)
                    memfsStream.Write(buffer, 0, amount);

                if (fileStream.Position != memfsStream.Position)
                    Console.WriteLine("finished at different positions");
                if (fileStream.Length != memfsStream.Length)
                    Console.WriteLine("different file lengths");
            }

            using (var fileStream = System.IO.File.OpenRead(fileName))
            using (var memfsStream = memfs.GetStream(memfsName))
            {
                int total = 0, amount = 0;
                var buffer = new byte[bufferSize];
                var data = new byte[buffer.Length];
                while ((amount = fileStream.Read(buffer, 0, buffer.Length)) > 0)
                {
                    var len = memfsStream.Read(data, 0, amount);
                    if (len != amount) Console.WriteLine("did not read correct amount");
                    if (fileStream.Position != memfsStream.Position) Console.WriteLine("streams at different positions");
                    //Console.WriteLine("file: {0}; memfs: {1}", fileStream.Position, memfsStream.Position);
                    for (int i = 0; i < Math.Min(len, amount); ++i)
                        if (buffer[i] != data[i]) Console.WriteLine("mismatch at position {0}", total + i);

                    total += len;
                    if (amount != len) fileStream.Seek(total, System.IO.SeekOrigin.Begin);
                }
                Console.WriteLine("compare finished");
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
                            Console.WriteLine("    | {0} | {1} | {2} |",
                                reader.GetGuid(0),
                                reader.GetString(1),
                                Encoding.Unicode.GetString((byte[])reader.GetValue(2))
                            );
                        }
                    }
                }
            }
        }

        const int msec = 1000;
        private static void AlterData(string dbFile, Guid guid, WaitHandle handle)
        {
            var sw = Stopwatch.StartNew();
            try
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

                        Thread.Sleep(3 * msec);
                        handle.WaitOne();

                        transaction.Commit();
                    }
                    Console.WriteLine("    Data ({0}) changed", guid);
                }
            }
            finally
            {
                Console.WriteLine("    Finished in {0}", sw.Elapsed);
            }
        }

        private static void ParallelUpdate(Guid guid, string file1, string file2)
        {
            Console.WriteLine("Parallel Update...");
            using (var cancellation = new CancellationTokenSource(5 * msec))
            {
                var task1 = Task.Run(() => AlterData(file1, guid.Increase(2), cancellation.Token.WaitHandle));
                var task2 = Task.Run(() => AlterData(file2, guid.Increase(4), cancellation.Token.WaitHandle));

                try { Task.WaitAll(task1, task2); } catch { /* handle later */ }

                if (task1.IsFaulted) Console.WriteLine("AlterData (1) failed: {0}", task1.Exception);
                if (task2.IsFaulted) Console.WriteLine("AlterData (2) failed: {0}", task2.Exception);
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

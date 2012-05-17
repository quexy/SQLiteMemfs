using System;
using System.IO;
using System.Runtime.InteropServices;

namespace System.Data.SQLite
{
    public sealed class InMemoryDatabase : IDisposable
    {
        [DllImport("memdb.dll", EntryPoint = "memdb_init", CallingConvention = CallingConvention.Cdecl)]
        private static extern int memdb_init();

        [DllImport("memdb.dll", EntryPoint = "memdb_destroy", CallingConvention = CallingConvention.Cdecl)]
        private static extern int memdb_destroy();

        [DllImport("memdb.dll", EntryPoint = "memdb_getsize", CallingConvention = CallingConvention.Cdecl)]
        private static extern long memdb_getsize
        (
            [In, MarshalAs(UnmanagedType.LPStr)] string file
        );

        [DllImport("memdb.dll", EntryPoint = "memdb_setsize", CallingConvention = CallingConvention.Cdecl)]
        private static extern void memdb_setsize
        (
            [In, MarshalAs(UnmanagedType.LPStr)] string file,
            [In] long size
        );

        [DllImport("memdb.dll", EntryPoint = "memdb_readdata", CallingConvention = CallingConvention.Cdecl)]
        private static extern int memdb_readdata
        (
            [In, MarshalAs(UnmanagedType.LPStr)] string file,
            [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] byte[] data,
            [In] int size,
            [In] long offset
        );

        [DllImport("memdb.dll", EntryPoint = "memdb_writedata", CallingConvention = CallingConvention.Cdecl)]
        private static extern int memdb_writedata
        (
            [In, MarshalAs(UnmanagedType.LPStr)] string file,
            [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] byte[] data,
            [In] int size,
            [In] long offset
        );

        public InMemoryDatabase()
        {
            memdb_init();
        }

        public long GetSize(string file)
        {
            return memdb_getsize(file);
        }

        public void SetSize(string file, long size)
        {
            memdb_setsize(file, size);
        }

        public void GetData(string file, out byte[] data)
        {
            long offset = 0;
            long size = memdb_getsize(file);

            data = new byte[size];
            while (size > int.MaxValue)
            {
                memdb_readdata(file, data, int.MaxValue, offset);
                offset += int.MaxValue;
                size -= offset;
            }
            memdb_readdata(file, data, (int)size, offset);
        }

        public int ReadData(string file, byte[] buffer, int offset, int count)
        {
            return memdb_readdata(file, buffer, count, offset);
        }

        public void SetData(string file, byte[] data)
        {
            memdb_writedata(file, data, data.Length, 0);
        }

        public int WriteData(string file, byte[] buffer, int offset, int count)
        {
            return memdb_writedata(file, buffer, count, offset);
        }

        public Stream GetStream(string file)
        {
            return new MemdbStream(this, file);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        ~InMemoryDatabase()
        {
            Dispose(false);
        }

        bool disposed = false;
        private void Dispose(bool disposing)
        {
            if (!disposed)
            {
                disposed = true;
                memdb_destroy();
            }
        }
    }
}

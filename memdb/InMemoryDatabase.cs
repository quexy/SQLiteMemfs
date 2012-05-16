using System;
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
        private static extern int memdb_getsize
        (
            [In, MarshalAs(UnmanagedType.LPStr)] string file
        );

        [DllImport("memdb.dll", EntryPoint = "memdb_getdata", CallingConvention = CallingConvention.Cdecl)]
        private static extern int memdb_getdata
        (
            [In, MarshalAs(UnmanagedType.LPStr)] string file,
            [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] byte[] data,
            [In] int size,
            [In] long offset
        );

        [DllImport("memdb.dll", EntryPoint = "memdb_setdata", CallingConvention = CallingConvention.Cdecl)]
        private static extern int memdb_setdata
        (
            [In, MarshalAs(UnmanagedType.LPStr)] string file,
            [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] byte[] data,
            [In] int size,
            [In] long offset
        );

        private static readonly object sync = new object();

        public InMemoryDatabase()
        {
            lock (sync) memdb_init();
        }

        public void GetData(string file, out byte[] data)
        {
            lock (sync)
            {
                int size = memdb_getsize(file);
                data = new byte[size];
                memdb_getdata(file, data, size, 0);
            }
        }

        public void SetData(string file, byte[] data)
        {
            lock (sync) memdb_setdata(file, data, data.Length, 0);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        ~InMemoryDatabase()
        {
            // When we're being reclaimed 'sync' is not guaranteed
            // to exist any more, so we're "resurrecting" it
            // for the duration of the finalizer. 
            GC.ReRegisterForFinalize(sync);
            Dispose(false);
            GC.KeepAlive(sync);
        }

        bool disposed = false;
        private void Dispose(bool disposing)
        {
            if (!disposed)
            {
                disposed = true;
                lock (sync) memdb_destroy();
            }
        }
    }
}

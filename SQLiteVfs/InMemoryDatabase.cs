using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace System.Data.SQLite
{
    public class InMemoryDatabase : IDisposable
    {
        [DllImport("memdb.dll", EntryPoint = "memdb_init", CallingConvention = CallingConvention.Cdecl)]
        private static extern int memdb_init();

        [DllImport("memdb.dll", EntryPoint = "memdb_getdatasize", CallingConvention = CallingConvention.Cdecl)]
        private static extern int memdb_getdatasize
        (
            [In, MarshalAs(UnmanagedType.LPStr)] string file,
            [Out] out int size
        );

        [DllImport("memdb.dll", EntryPoint = "memdb_getdata", CallingConvention = CallingConvention.Cdecl)]
        private static extern int memdb_getdata
        (
            [In, MarshalAs(UnmanagedType.LPStr)] string file,
            [In, Out, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] ref byte[] data,
            [In, Out] ref int size
        );

        [DllImport("memdb.dll", EntryPoint = "memdb_destroy", CallingConvention = CallingConvention.Cdecl)]
        private static extern int memdb_destroy();

        private static readonly object sync = new object();

        public InMemoryDatabase()
        {
            lock (sync) memdb_init();
        }

        public void GetData(string file, out byte[] data)
        {
            lock (sync)
            {
                int size = 0;
                memdb_getdatasize(file, out size);
                data = new byte[size];
                memdb_getdata(file, ref data, ref size);
            }
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
                if (disposing)
                {
                    // nop
                }

                memdb_destroy();
            }
        }
    }
}

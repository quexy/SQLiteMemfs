#include "SQLiteMemfs.h"
#include "MemfsStream.h"

#include "memfs.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;

namespace System
{
    namespace Data
    {
        namespace SQLite
        {
            SQLiteMemfs::SQLiteMemfs(void)
            {
                Monitor::Enter(memfs_sync);
                disposed = false;
                memfs_init();
                Monitor::Exit(memfs_sync);
            }

            SQLiteMemfs::~SQLiteMemfs(void)
            {
                Destroy(true);
                GC::SuppressFinalize(this);
            }

            SQLiteMemfs::!SQLiteMemfs(void)
            {
                GC::ReRegisterForFinalize(memfs_sync);
                Destroy(false);
                GC::KeepAlive(memfs_sync);
            }

            void SQLiteMemfs::Destroy(Boolean disposing)
            {
                if (!disposed)
                {
                    Monitor::Enter(memfs_sync);
                    disposed = true;
                    memfs_destroy();
                    Monitor::Exit(memfs_sync);
                }
            }

            System::IO::Stream^ SQLiteMemfs::GetStream(System::String^ file)
            {
                return gcnew System::Data::SQLite::MemfsStream(file);
            }

            void SQLiteMemfs::Delete(String^ file)
            {
                IntPtr ptr = Marshal::StringToHGlobalAnsi(file);
                memfs_delfile(static_cast<const char*>(ptr.ToPointer()));
                Marshal::FreeHGlobal(ptr);
            }
        }
    }
}

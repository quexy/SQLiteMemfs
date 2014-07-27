#include "SQLiteMemfs.h"
#include "MemfsStream.h"

#pragma unmanaged
#include "memfs.h"
#pragma managed

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;

namespace System
{
    namespace Data
    {
        namespace SQLite
        {
            [System::Security::Permissions::SecurityPermission(System::Security::Permissions::SecurityAction::Assert, UnmanagedCode = true)]
            SQLiteMemfs::SQLiteMemfs(void)
            {
                disposed = false;
                memfs_init();
            }

            SQLiteMemfs::~SQLiteMemfs(void)
            {
                Destroy(true);
                GC::SuppressFinalize(this);
            }

            SQLiteMemfs::!SQLiteMemfs(void)
            {
                Destroy(false);
            }

            [System::Security::Permissions::SecurityPermission(System::Security::Permissions::SecurityAction::Assert, UnmanagedCode = true)]
            void SQLiteMemfs::Destroy(Boolean disposing)
            {
                if (!disposed)
                {
                    disposed = true;
                    memfs_destroy();
                }
            }

            System::IO::Stream^ SQLiteMemfs::GetStream(System::String^ file)
            {
                return gcnew System::Data::SQLite::MemfsStream(file);
            }

            [System::Security::Permissions::SecurityPermission(System::Security::Permissions::SecurityAction::Assert, UnmanagedCode = true)]
            void SQLiteMemfs::Delete(String^ file)
            {
                IntPtr ptr = Marshal::StringToHGlobalAnsi(file);
                memfs_delfile(static_cast<const char*>(ptr.ToPointer()));
                Marshal::FreeHGlobal(ptr);
            }
        }
    }
}

#include "SQLiteMemfs.h"
#include "MemfsStream.h"

#include "memfs.h"

using namespace System;

namespace System
{
    namespace Data
    {
        namespace SQLite
        {
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
        }
    }
}

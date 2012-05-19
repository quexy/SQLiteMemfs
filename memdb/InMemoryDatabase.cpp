#include "InMemoryDatabase.h"
#include "MemdbStream.h"

#include "memdb.h"

using namespace System;

namespace System
{
    namespace Data
    {
        namespace SQLite
        {
            InMemoryDatabase::InMemoryDatabase(void)
            {
                disposed = false;
                memdb_init();
            }

            InMemoryDatabase::~InMemoryDatabase(void)
            {
                if (!disposed)
                {
                    disposed = true;
                    memdb_destroy();
                }
            }

            System::IO::Stream^ InMemoryDatabase::GetStream(System::String^ file)
            {
                return gcnew System::Data::SQLite::MemdbStream(file);
            }
        }
    }
}

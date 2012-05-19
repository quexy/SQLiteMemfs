#include "InMemoryFileSystem.h"
#include "MemfsStream.h"

#include "memfs.h"

using namespace System;

namespace System
{
    namespace Data
    {
        namespace SQLite
        {
            InMemoryFileSystem::InMemoryFileSystem(void)
            {
                disposed = false;
                memfs_init();
            }

            InMemoryFileSystem::~InMemoryFileSystem(void)
            {
                Destroy(true);
                GC::SuppressFinalize(this);
            }

            InMemoryFileSystem::!InMemoryFileSystem(void)
            {
                Destroy(false);
            }

            void InMemoryFileSystem::Destroy(Boolean disposing)
            {
                if (!disposed)
                {
                    disposed = true;
                    memfs_destroy();
                }
            }

            System::IO::Stream^ InMemoryFileSystem::GetStream(System::String^ file)
            {
                return gcnew System::Data::SQLite::MemfsStream(file);
            }
        }
    }
}

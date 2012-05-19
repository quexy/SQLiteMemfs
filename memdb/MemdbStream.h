#pragma once

#include "InMemoryDatabase.h"

#include "memdb.h"

using namespace System;
using namespace System::IO;

namespace System
{
    namespace Data
    {
        namespace SQLite
        {
            ref class MemdbStream sealed : public Stream
            {
            private:
                Int64 position;
                IntPtr filename;
                Boolean disposed;
                void Destroy(Boolean disposing);

            public:
                MemdbStream(System::String^ filename);

                ~MemdbStream(void);
                !MemdbStream(void);

                property Boolean CanRead
                {
                    virtual Boolean get() override { return true; }
                }

                property Boolean CanSeek
                {
                    virtual Boolean get() override { return true; }
                }

                property Boolean CanWrite
                {
                    virtual Boolean get() override { return true; }
                }

                virtual void Flush() override;

                property Int64 Length
                {
                    virtual Int64 get() override;
                }

                property Int64 Position
                {
                    virtual Int64 get() override { return position; }
                    virtual void set(Int64 value) override { position = value; }
                }

                virtual Int32 Read(cli::array<Byte, 1>^ buffer, Int32 offset, Int32 count) override;

                virtual Int64 Seek(Int64 offset, SeekOrigin origin) override;

                virtual void SetLength(Int64 value) override;

                virtual void Write(cli::array<Byte, 1>^ buffer, Int32 offset, Int32 count) override;
            };
        }
    }
}

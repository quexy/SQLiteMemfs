#include "MemdbStream.h"

#include "memdb.h"

using namespace System;
using namespace System::IO;
using namespace System::Runtime::InteropServices;

namespace System
{
    namespace Data
    {
        namespace SQLite
        {
            const char* ptr2str(IntPtr ptr)
            {
                return static_cast<const char*>(ptr.ToPointer());
            }

            MemdbStream::MemdbStream(String^ filename)
            {
                position = 0;
                disposed = false;
                this->filename = Marshal::StringToHGlobalAnsi(filename);
            }

            MemdbStream::~MemdbStream(void)
            {
                Destroy(true);
                GC::SuppressFinalize(this);
            }

            MemdbStream::!MemdbStream(void)
            {
                GC::ReRegisterForFinalize(filename);
                Destroy(false);
                GC::KeepAlive(filename);
            }

            void MemdbStream::Destroy(bool disposing)
            {
                if(!disposed)
                {
                    disposed = true;
                    Marshal::FreeHGlobal(filename);
                }
            }

            void MemdbStream::Flush() { /* NOP */ }

            Int64 MemdbStream::Length::get() { return memdb_getsize(ptr2str(filename)); }

            Int32 MemdbStream::Read(array<Byte>^ buffer, Int32 offset, Int32 count)
            {
                pin_ptr<unsigned char> ptr = &buffer[offset];
                Int32 size = memdb_readdata(ptr2str(filename), ptr, count, position);
                position += size;
                return size;
            }

            Int64 MemdbStream::Seek(Int64 offset, SeekOrigin origin)
            {
                switch (origin)
                {
                case SeekOrigin::Begin:
                    Position = offset;
                    break;
                case SeekOrigin::Current:
                    Position = Position + offset;
                    break;
                case SeekOrigin::End:
                    Position = Length - offset;
                    break;
                default:
                    throw gcnew InvalidOperationException("Unrecognized SeekOrigin: " + origin.ToString());
                }
                return Position;
            }

            void MemdbStream::SetLength(Int64 value)
            {
                memdb_setsize(ptr2str(filename), value);
            }

            void MemdbStream::Write(array<Byte>^ buffer, Int32 offset, Int32 count)
            {
                pin_ptr<unsigned char> ptr = &buffer[offset];
                Int32 size = memdb_writedata(ptr2str(filename), ptr, count, position);
                position += size;
            }
        }
    }
}

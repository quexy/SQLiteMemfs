#include "MemfsStream.h"

#pragma unmanaged
#include "memfs.h"
#pragma managed

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

            MemfsStream::MemfsStream(String^ filename)
            {
                position = 0;
                disposed = false;
                this->filename = Marshal::StringToHGlobalAnsi(filename);
            }

            MemfsStream::~MemfsStream(void)
            {
                Destroy(true);
                GC::SuppressFinalize(this);
            }

            MemfsStream::!MemfsStream(void)
            {
                Destroy(false);
                GC::KeepAlive(filename);
            }

            void MemfsStream::Destroy(Boolean disposing)
            {
                if (disposed) return;
                disposed = true;

                if (filename == IntPtr::Zero) return;
                Marshal::FreeHGlobal(filename);
                filename = IntPtr::Zero;
            }

            void MemfsStream::Flush() { /* NOP */ }

            [System::Security::Permissions::SecurityPermission(System::Security::Permissions::SecurityAction::Assert, UnmanagedCode = true)]
            Int64 MemfsStream::Length::get() { return memfs_getsize(ptr2str(filename)); }

            [System::Security::Permissions::SecurityPermission(System::Security::Permissions::SecurityAction::Assert, UnmanagedCode = true)]
            Int32 MemfsStream::Read(array<Byte>^ buffer, Int32 offset, Int32 count)
            {
                if (buffer->Length < offset + count)
                    throw gcnew IndexOutOfRangeException("Max position would exceed array size");
                pin_ptr<unsigned char> ptr = &buffer[offset];
                Int32 size = memfs_readdata(ptr2str(filename), ptr, count, position);
                position += size;
                return size;
            }

            Int64 MemfsStream::Seek(Int64 offset, SeekOrigin origin)
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

            [System::Security::Permissions::SecurityPermission(System::Security::Permissions::SecurityAction::Assert, UnmanagedCode = true)]
            void MemfsStream::SetLength(Int64 value)
            {
                bool success = memfs_setsize(ptr2str(filename), value);
                if (!success) throw gcnew IOException("Resize failed");
            }

            [System::Security::Permissions::SecurityPermission(System::Security::Permissions::SecurityAction::Assert, UnmanagedCode = true)]
            void MemfsStream::Write(array<Byte>^ buffer, Int32 offset, Int32 count)
            {
                pin_ptr<unsigned char> ptr = &buffer[offset];
                Int32 size = memfs_writedata(ptr2str(filename), ptr, count, position);
                position += size;
            }
        }
    }
}

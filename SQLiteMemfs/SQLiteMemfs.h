#pragma once

using namespace System;
using namespace System::IO;

namespace System
{
    namespace Data
    {
        namespace SQLite
        {
            /// <summary>
            /// An SQLite 3 VFS to keep database files in memory.
            /// The files can be accessed both by SQLite and any third party.
            /// </summary>
            public ref class SQLiteMemfs sealed : public System::IDisposable
            {
            private:
                Boolean disposed;
                void Destroy(Boolean disposing);

            public:
                /// <summary>
                /// Registers the VFS in the SQLite VFS stack, or increases the reference count
                /// </summary>
                SQLiteMemfs(void);

                /// <summary>
                /// Decreases the VFS reference count, and removes the VFS from the SQLite VFS stack if count reached zero.
                /// </summary>
                ~SQLiteMemfs(void);
                !SQLiteMemfs(void);

                /// <summary>
                /// Returns a <see cref="Stream"/> of the specified file.
                /// </summary>
                /// <param name="file">the name of the file</param>
                /// <returns>Returns a <see cref="Stream"/> of the file.</returns>
                Stream^ GetStream(String^ file);

                /// <summary>
                /// Deletes the specified file, freeing up the memory it occupied.
                /// </summary>
                /// <param name="file">the name of the file to delete</param>
                void Delete(String^ file);
            };
        }
    }
}

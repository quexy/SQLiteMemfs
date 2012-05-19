#pragma once

using namespace System;
using namespace System::IO;

namespace System
{ 
    namespace Data
    {
        namespace SQLite
        {
            public ref class InMemoryFileSystem sealed : public System::IDisposable
            {
            private:
                Boolean disposed;
                void Destroy(Boolean disposing);

            public:
                InMemoryFileSystem(void);

                ~InMemoryFileSystem(void);
                !InMemoryFileSystem(void);

                Stream^ GetStream(String^ file);
            };
        }
    }
}

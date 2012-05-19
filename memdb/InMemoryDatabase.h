#pragma once

using namespace System;
using namespace System::IO;

namespace System
{ 
    namespace Data
    {
        namespace SQLite
        {
            public ref class InMemoryDatabase : public System::IDisposable
            {
            private:
                Boolean disposed;

            public:
                InMemoryDatabase(void);

                ~InMemoryDatabase(void);

                Stream^ GetStream(String^ file);
            };
        }
    }
}

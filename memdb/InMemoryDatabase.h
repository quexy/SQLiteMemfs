#pragma once

using namespace System;
using namespace System::IO;

namespace System
{ 
    namespace Data
    {
        namespace SQLite
        {
            public ref class InMemoryDatabase sealed : public System::IDisposable
            {
            private:
                Boolean disposed;
                void Destroy(Boolean disposing);

            public:
                InMemoryDatabase(void);

                ~InMemoryDatabase(void);
                !InMemoryDatabase(void);

                Stream^ GetStream(String^ file);
            };
        }
    }
}

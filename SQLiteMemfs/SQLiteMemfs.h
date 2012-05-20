#pragma once

using namespace System;
using namespace System::IO;

namespace System
{ 
    namespace Data
    {
        namespace SQLite
        {
            public ref class SQLiteMemfs sealed : public System::IDisposable
            {
            private:
                Boolean disposed;
                void Destroy(Boolean disposing);

            public:
                SQLiteMemfs(void);

                ~SQLiteMemfs(void);
                !SQLiteMemfs(void);

                static Stream^ GetStream(String^ file);
            };
        }
    }
}

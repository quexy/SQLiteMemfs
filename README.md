# SQLiteMemfs
In-memory VFS for SQLite

An extended in-memory database for SQLite. Unlike the built-in `:memory:` syntax, the files stay in memory
until explicitly deleted or the VFS is unloaded. The databases are accessible outside of SQLite through the VFS.

For details see the demo.

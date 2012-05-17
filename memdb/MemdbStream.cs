using System;
using System.IO;

namespace System.Data.SQLite
{
    class MemdbStream : Stream
    {
        private long position = 0;
        private readonly string filename;
        private readonly InMemoryDatabase memdb;

        public MemdbStream(InMemoryDatabase memdb, string filename)
        {
            this.memdb = memdb;
            this.filename = filename;
        }

        public override bool CanRead { get { return true; } }

        public override bool CanSeek { get { return true; } }

        public override bool CanWrite { get { return true; } }

        public override void Flush() { /* NOP */ }

        public override long Length { get { return memdb.GetSize(filename); } }

        public override long Position
        {
            get { return position; }
            set { position = value; }
        }

        public override int Read(byte[] buffer, int offset, int count)
        {
            return memdb.ReadData(filename, buffer, offset, count);
        }

        public override long Seek(long offset, SeekOrigin origin)
        {
            switch (origin)
            {
                case SeekOrigin.Begin:
                    Position = offset;
                    break;
                case SeekOrigin.Current:
                    Position = Position + offset;
                    break;
                case SeekOrigin.End:
                    Position = Length - offset;
                    break;
                default:
                    throw new InvalidOperationException("Unrecognized SeekOrigin: " + origin);
            }
            return Position;
        }

        public override void SetLength(long value)
        {
            memdb.SetSize(filename, value);
        }

        public override void Write(byte[] buffer, int offset, int count)
        {
            memdb.WriteData(filename, buffer, offset, count);
        }
    }
}

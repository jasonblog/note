/**
 *  һ��ǿ���Э����, protocolstream.h
 *  zhangyl 2017.05.27
 */

#ifndef __PROTOCOL_STREAM_H__
#define __PROTOCOL_STREAM_H__

#include <stdlib.h>
#include <sys/types.h>
#include <string>
#include <sstream>
#include <stdint.h>

using namespace std;

//������Э��Ĵ������࣬�ڲ��ķ�����֮��ͨѶ��ͳһ������Щ��
namespace net
{
    enum
    {
        TEXT_PACKLEN_LEN = 4,
        TEXT_PACKAGE_MAXLEN = 0xffff,
        BINARY_PACKLEN_LEN = 2,
        BINARY_PACKAGE_MAXLEN = 0xffff,

        TEXT_PACKLEN_LEN_2 = 6,
        TEXT_PACKAGE_MAXLEN_2 = 0xffffff,

        BINARY_PACKLEN_LEN_2 = 4,               //4�ֽ�ͷ����
        BINARY_PACKAGE_MAXLEN_2 = 0x10000000,   //����󳤶���256M,�㹻��

        CHECKSUM_LEN = 2,
    };

    //����У���
    unsigned short checksum(const unsigned short* buffer, int size);
    bool compress_(unsigned int i, char* buf, size_t& len);
    bool uncompress_(char* buf, size_t len, unsigned int& i);

    //�ӿڶ���
    struct IReadStream
    {
        virtual ~IReadStream() {}
        virtual const char* GetData() const = 0;
        virtual size_t GetSize() const = 0;
    };

    struct IWriteStream
    {
        virtual ~IWriteStream() {}
        virtual const char* GetData() const = 0;
        virtual size_t GetSize() const = 0;
    };

    class BinaryReadStream : public IReadStream
    {
    private:
        const char* const ptr;
        const size_t      len;
        const char*       cur;
        BinaryReadStream(const BinaryReadStream&);
        BinaryReadStream& operator=(const BinaryReadStream&);

    public:
        BinaryReadStream(const char* ptr, size_t len);
        virtual const char* GetData() const;
        virtual size_t GetSize() const;
        bool IsEmpty() const;
        bool ReadString(string* str, size_t maxlen, size_t& outlen);
        bool ReadCString(char* str, size_t strlen, size_t& len);
        bool ReadCCString(const char** str, size_t maxlen, size_t& outlen);
        bool ReadInt32(int32_t& i);
        bool ReadInt64(int64_t& i);
        bool ReadShort(short& i);
        bool ReadChar(char& c);
        size_t ReadAll(char* szBuffer, size_t iLen) const;
        bool IsEnd() const;
        const char* GetCurrent() const{ return cur; }

    public:
        bool ReadLength(size_t & len);
        bool ReadLengthWithoutOffset(size_t &headlen, size_t & outlen);
    };

    class BinaryWriteStream : public IWriteStream
    {
    public:
        BinaryWriteStream(string* data);
        virtual const char* GetData() const;
        virtual size_t GetSize() const;
        bool WriteCString(const char* str, size_t len);
        bool WriteString(const string& str);
        bool WriteDouble(double value, bool isNULL = false);
        bool WriteInt64(int64_t value, bool isNULL = false);
        bool WriteInt32(int32_t i, bool isNULL = false);
        bool WriteShort(short i, bool isNULL = false);
        bool WriteChar(char c, bool isNULL = false);
        size_t GetCurrentPos() const{ return m_data->length(); }
        void Flush();
        void Clear();
    private:
        string* m_data;
    };

}// end namespace

#endif //!__PROTOCOL_STREAM_H__
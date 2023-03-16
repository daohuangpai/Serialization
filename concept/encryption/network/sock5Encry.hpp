#include "concept\error\errorbase.hpp"
#include "concept\encryption\fixBufferEncry.hpp"
#include "concept\encryption\encry.hpp"
#include "concept\encryption\lengthEncry.hpp"
#include "concept/encryption/bufferEncry.hpp"
#include <variant>
#include <asio.hpp>

#define __SOCK_VER 0x05
#define __SOCK5_RSV 0x00
using Sock6Addr = std::variant<asio::ip::address, std::string>;
NAMESPACE_BIN_ENCRY
{
    //template<ConceptEncryption Encry>
        /*
        *         +----+-----+-------+------+----------+----------+
            |VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
            +----+-----+-------+------+----------+----------+
            | 1  |  1  | X'00' |  1   | Variable |    2     |
            +----+-----+-------+------+----------+----------+
            VER 字段表征 Socks 版本, 固定为 0x05
    CMD 字段指示连接的类型, 占 1 个字节, 共有 3 个取值, 分别为 0x01 (CONNECT), 0x02 (BIND), 0x03 (UDP ASSOCIATE), 关于每个值的含义将在下面讨论
    RSV 字段为保留字段, 占 1 个字节, 固定为 0x00
    ATYP 字段指示地址类型 (DST.ADDR 字段的类型), 0x01 为 IPv4 地址, 0x03 为域名, 0x04 为 IPv6 地址
    DST.ADDR 字段指示客户端所要访问的目的地址, 这是一个变长字段, 其长度由前一个字段的值来决定,
    当 ATYP 字段的值为 0x01 时, 表示该字段为 IPv4 地址, 从而长度为 4 个字节, 当 ATYP 字段的值为 0x03 时代表地址类型为域名,
    此时 DST.ADDR 字段的第一个字节的值指示其后所跟随的域名所占的字节数, 当 ATYP 字段的值为 0x04 时, 表示该字段为 IPv6 地址, 其长度为 16 个字节
        */
    struct sock5AddressEncry
    {
        __SIGNED_ENCRYPTION
         template<typename EncodeContainerType>
        static EncodeContainerType to_bin(asio::ip::address& address)
        {
            EncodeContainerType buffer;
            if (address.is_v4())
            {
                auto v4 = address.to_v4().to_bytes();
                buffer.push_back(0x01);
                buffer.insert(std::end(buffer), std::begin(v4), std::end(v4));
            }
             else if (address.is_v6())
            {
                auto v6 = address.to_v6().to_bytes();
                buffer.push_back(0x04);
                buffer.insert(std::end(buffer), std::begin(v6), std::end(v6));
             }
            return std::move(buffer);
        }

        using encodeType = Sock6Addr;
        template<typename EncodeContainerType>
        static EncodeContainerType Encode(encodeType& sock6Addr)
        {
            EncodeContainerType data;
            switch (sock6Addr.index())
            {
            case 0:
                data = to_bin<EncodeContainerType>(std::get<asio::ip::address>(sock6Addr));
                break;
            case 1:
                {
                std::string host = std::get<std::string>(sock6Addr);
                data.push_back(0x03);
                unsigned char size = host.size();
                __error_throw_out_UserErrorCode(size == host.size(), DataProtocolStatue::ERROR_DATA, "host size error!")
                data.push_back(size);
                data.insert(std::end(data), std::begin(host), std::end(host));
                return std::move(data);
                }
                break;
            default:
                __error_throw_out_UserErrorCode(false, DataProtocolStatue::ERROR_DATA, "error sock6Addr!")
                break;
            }
            return std::move(data);
        }
        template<typename iterator>
        static encodeType Decode(iterator begin, iterator end, size_t* readbyte)
        {
            encodeType ret;

            iterator itr = begin;
            auto c = *itr++;
            iterator itrEnd = itr;
            switch (c)
            {
            case 0x01://ip4
                {
                  auto size = end - itr;
                  __error_throw_out_UserErrorCode(size >= 4, DataProtocolStatue::LACK_DATA,"ip4 address need 4 byte!")
                  itrEnd = itr + 4;
                  asio::ip::address_v4::bytes_type byte;
                  std::copy(itr, itrEnd, std::begin(byte));
                  ret = asio::ip::make_address_v4(byte);
                }
                break;
            case 0x03://域名
                {
                //第一个字节是host大小
                auto size = *itr++;
                __error_throw_out_UserErrorCode(end - itr >= size, DataProtocolStatue::LACK_DATA, "host size not fit!")
                itrEnd = itr + size;
                std::string host;
                host.resize(size);
                std::copy(itr, itrEnd, std::begin(host));
                ret = host;
               }
               break;
           case 0x04://ip6
               {
               auto size = end - itr;
               __error_throw_out_UserErrorCode(size >= 16, DataProtocolStatue::LACK_DATA,"ip4 address need 16 byte!")
               itrEnd = itr + 16;
               asio::ip::address_v6::bytes_type byte;
               std::copy(itr, itrEnd, std::begin(byte));
               ret = asio::ip::make_address_v6(byte);
               }
               break;
           default:
               __error_throw_out_UserErrorCode(false, DataProtocolStatue::ERROR_DATA, "error ATYP!")
               break;
           }
           *readbyte = itrEnd - begin;
           return std::move(ret);
       }
   };
}
//第一次握手协议
using HandShockChuckType = encryption::HeadFixEncry<encryption::bufferEncry<std::vector<unsigned char>, unsigned char, encryption::NATIVE>, __SOCK_VER>;
//服务器返回
using HandShockReplyChuckType = encryption::HeadFixEncry<encryption::typeEncry<unsigned char>, __SOCK_VER>;
//握手成功后的请求协议
using Sock5Request = encryption::linkEncryptionEncry<
    encryption::HeadFixEncry<encryption::typeEncry<unsigned char>, __SOCK_VER>,
    encryption::HeadFixEncry<encryption::sock5AddressEncry, __SOCK5_RSV>,
    encryption::typeEncry<unsigned short>
>;
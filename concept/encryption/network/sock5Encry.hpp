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
            VER �ֶα��� Socks �汾, �̶�Ϊ 0x05
    CMD �ֶ�ָʾ���ӵ�����, ռ 1 ���ֽ�, ���� 3 ��ȡֵ, �ֱ�Ϊ 0x01 (CONNECT), 0x02 (BIND), 0x03 (UDP ASSOCIATE), ����ÿ��ֵ�ĺ��彫����������
    RSV �ֶ�Ϊ�����ֶ�, ռ 1 ���ֽ�, �̶�Ϊ 0x00
    ATYP �ֶ�ָʾ��ַ���� (DST.ADDR �ֶε�����), 0x01 Ϊ IPv4 ��ַ, 0x03 Ϊ����, 0x04 Ϊ IPv6 ��ַ
    DST.ADDR �ֶ�ָʾ�ͻ�����Ҫ���ʵ�Ŀ�ĵ�ַ, ����һ���䳤�ֶ�, �䳤����ǰһ���ֶε�ֵ������,
    �� ATYP �ֶε�ֵΪ 0x01 ʱ, ��ʾ���ֶ�Ϊ IPv4 ��ַ, �Ӷ�����Ϊ 4 ���ֽ�, �� ATYP �ֶε�ֵΪ 0x03 ʱ�����ַ����Ϊ����,
    ��ʱ DST.ADDR �ֶεĵ�һ���ֽڵ�ֵָʾ����������������ռ���ֽ���, �� ATYP �ֶε�ֵΪ 0x04 ʱ, ��ʾ���ֶ�Ϊ IPv6 ��ַ, �䳤��Ϊ 16 ���ֽ�
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
            case 0x03://����
                {
                //��һ���ֽ���host��С
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
//��һ������Э��
using HandShockChuckType = encryption::HeadFixEncry<encryption::bufferEncry<std::vector<unsigned char>, unsigned char, encryption::NATIVE>, __SOCK_VER>;
//����������
using HandShockReplyChuckType = encryption::HeadFixEncry<encryption::typeEncry<unsigned char>, __SOCK_VER>;
//���ֳɹ��������Э��
using Sock5Request = encryption::linkEncryptionEncry<
    encryption::HeadFixEncry<encryption::typeEncry<unsigned char>, __SOCK_VER>,
    encryption::HeadFixEncry<encryption::sock5AddressEncry, __SOCK5_RSV>,
    encryption::typeEncry<unsigned short>
>;
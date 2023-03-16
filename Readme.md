进程间共享内存、服务器的RPC调用需要函数序列化或者网络传输往往要用到自定义的格式，有时候是很麻烦的，而且协议的内容也很难被直观的反应出来
所以这里使用编解码器描述这种关系

结合functionInfoTaker.hpp的函数结构提取工具可以实现将任意函数序列化反序列化实现RPC接口

使用统一的接口可以对C++内部的各种格式或者类直接进行使用任意stl模板进行编码，同时解码一个stl模板（std::string\std::vector等有标准接口的）的二进制流变回原来的类型，隐藏底层传输在上层使用就可以直接使用类一样

对于所有不同类型和协议的编解码工具，都有三个必要内容
1、编码函数Encode
2、解码函数Decode
3、标识是编解码工具的__SIGNED_ENCRYPTION




现有的几种简单
1、typeEncry.--直接对一个类型T进行二进制编解码
2、ReflectorDataJsonEncry-对一个反射类（通过__reflectorWithSerialize包裹）使用json格式进行编解码的编码器
3、bufferEncry--对一个任意的string\vector等类型数据进行编解码，编码后的数据格式为长度+数据的编码器
4、HeadFixEncry--定义一个固定长度的头部，对数据进行编解码，编码后的数据格式为头部+数据的编码器
5、BackFixEncry--定义一个固定长度的尾部，对数据进行编解码，编码后的数据格式为数据+尾部的编码器
6、linkEncryptionEncry--对任意多的编码器进行连接组合成一个任意的编码器
7、bitLengthEncry--网络协议中往往拆分多个bit位为一个东西，对任意类型的网络传输的整型数据拆分为多个std::bitset
8、sock5代理协议的HandShockChuckType、HandShockReplyChuckType、Sock5Request握手、应答、请求协议的编码器
9、netLengthEncry封装网络的长度和本机的长度转化的编解码
大部分用法在TestReflecttorEncryption.cpp、TestReflecttorEncryption.cpp、TestEncry.cpp都有描述




一、使用__reflectorWithSerialize声明一个结构体，使得其可以被反射
```C++
//定义一个反射类
struct ReflectorClass {
    __reflectorWithSerialize(
        (uint32)idInt,
        (float)valueFloat,
        (double)valueDouble,
        (std::string)valueString
    )

     __reflectorSerializeEquation(ReflectorClass);//定义等号
};

TEST(TestReflecttorEncryption, Reflect)
{
    ReflectorClass rawValue = { 1,2.678,98.3849756,"hello world!" };
    std::cout << "visit reflectorClass:" << std::endl;
    Reflector::visit_each(rawValue, [](auto fieldData)
        {
            std::cout << fieldData.name() << ":" << fieldData.get() << std::endl;
        });
}

```
输出整个结构体的全部信息
```
visit reflectorClass:
idInt:1
valueFloat:2.678
valueDouble:98.385
valueString:hello world!
```
使用encryption::ReflectorDataJsonEncry可以对全部__reflectorWithSerialize定义的反射结构体进行编解码


比如对三个不同的类型比如std::string、typeEncryClass和反射类ReflectorClass进行编解码
使用协议
头部添加0xff，尾部添加0x0f, 0xff, 0x0f
0xff stringLength binary(std::string) binary(typeEncryClass) lengthof(json(ReflectorClass)) json(ReflectorClass) 0x0f, 0xff, 0x0f
```C++
struct typeEncryClass
{
	float fvalue;
	int ivalue;
	bool operator==(const typeEncryClass& other)const
	{
		return ((fvalue == other.fvalue) && (ivalue == other.ivalue));
	}
};

struct ReflectorClass {
	__reflectorWithSerialize(
		(uint32)idInt,
		(float)valueFloat,
		(double)valueDouble,
		(std::string)valueString
	)

	__reflectorSerializeEquation(ReflectorClass);//定义等号
};
	using encodeType = encryption::BackFixEncry<
		encryption::HeadFixEncry<
	    encryption::linkEncryptionEncry<
			encryption::bufferEncry<std::string, std::uint64_t, encryption::LITTLE_ENDIAN>,
			encryption::typeEncry<typeEncryClass>,
			encryption::ReflectorDataJsonEncry<ReflectorClass>
		>,
		0xff>,
		0x0f, 0xff, 0x0f>;

	std::string text = "hello world";
	typeEncryClass typeEncry = {1.98,9};
	ReflectorClass rawValue = { 1,2.678,98.3849756,"hello world!" };
	auto value = std::make_tuple(text, typeEncry, rawValue);
	auto encodeData = encodeType::Encode<std::vector<char>>(value);
	helper::dump(encodeData);
	size_t byteRead;
	//解码结果也是一个和value一样的tuple结构
	auto decodevalue = encodeType::Decode(std::begin(encodeData), std::end(encodeData), &byteRead);
	EXPECT_EQ(byteRead, std::size(encodeData)) << "error";

	EXPECT_EQ(value, decodevalue);
```
输出
```
0000 - ff 0b 00 00 00 00 00 00-00 68 65 6c 6c 6f 20 77   .........hello w
0010 - 6f 72 6c 64 a4 70 fd 3f-09 00 00 00 9f 00 00 00   orld.p.?........
0020 - 00 00 00 00 7b 0a 20 20-20 20 22 76 61 6c 75 65   ....{.    "value
0030 - 30 22 3a 20 7b 0a 20 20-20 20 20 20 20 20 22 69   0": {.        "i
0040 - 64 49 6e 74 22 3a 20 31-2c 0a 20 20 20 20 20 20   dInt": 1,.
0050 - 20 20 22 76 61 6c 75 65-46 6c 6f 61 74 22 3a 20     "valueFloat":
0060 - 32 2e 36 37 37 39 39 39-39 37 33 32 39 37 31 31   2.67799997329711
0070 - 39 2c 0a 20 20 20 20 20-20 20 20 22 76 61 6c 75   9,.        "valu
0080 - 65 44 6f 75 62 6c 65 22-3a 20 39 38 2e 33 38 34   eDouble": 98.384
0090 - 39 37 35 36 2c 0a 20 20-20 20 20 20 20 20 22 76   9756,.        "v
00a0 - 61 6c 75 65 53 74 72 69-6e 67 22 3a 20 22 68 65   alueString": "he
00b0 - 6c 6c 6f 20 77 6f 72 6c-64 21 22 0a 20 20 20 20   llo world!".
00c0 - 7d 0a 7d 0f ff 0f                                 }.}...
```


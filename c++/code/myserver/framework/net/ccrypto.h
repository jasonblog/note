//
//  ccrypto.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef __CCRYPTO_H__
#define __CCRYPTO_H__
#include <openssl/aes.h>

class CAes
{
public:
	//构造函数
	CAes();
	//析构函数
	~CAes();
	//初始化
	int init(const unsigned char *userKey, int len);
	//加密
	char *encrypt(const char *input, int inputlen, int &outputlen);
	//解密
	char *decrypt(const char *input, int inputlen, int &outputlen);
	//重置缓冲区大小
	void resize(int size);
public:
	AES_KEY _enKey;
	AES_KEY _deKey;
	char *_input;
	char *_output;
	int _inputlen;
	int _outputlen;
};
#endif // __CCRYPTO_H__

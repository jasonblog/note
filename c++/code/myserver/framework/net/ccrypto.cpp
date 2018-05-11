#include "ccrypto.h"
#include "base.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


CAes::CAes()
{
	_input = (char *) malloc(20480 * sizeof(char));
	_inputlen = MAX_PACKAGE_LEN;
	_output = (char *) malloc(MAX_PACKAGE_LEN * sizeof(char));
	_outputlen = MAX_PACKAGE_LEN;
}

CAes::~CAes()
{
	if (_input) {
		free(_input);
		_input = NULL;
	}

	if (_output) {
		free(_output);
		_output = NULL;
	}
}

int CAes::init(const unsigned char *userKey, int len)
{
	int ret = AES_set_encrypt_key(userKey, len * 8, &_enKey);

	ret = AES_set_decrypt_key(userKey, len * 8, &_deKey);
	return ret;
}

char *CAes::encrypt(const char *in, int inputlen, int &outputlen)
{
	if (inputlen > _inputlen) {
		resize(inputlen);
	}
	outputlen = (inputlen / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
	memcpy(_input, (char *) in, inputlen);
	unsigned char topad = outputlen - inputlen;
	memset(_input + inputlen, topad, topad);

	int en_len = 0;
	while (en_len < outputlen) {
		AES_encrypt((unsigned char *) _input + en_len, (unsigned char *) _output + en_len, &_enKey);
		en_len += AES_BLOCK_SIZE;
	}

	return _output;
}

char *CAes::decrypt(const char *in, int inputlen, int &outputlen)
{
	if (inputlen > _inputlen) {
		resize(inputlen);
	}
	memcpy(_input, in, inputlen);
	int en_len = 0;
	while (en_len < inputlen) {
		AES_decrypt((unsigned char *) _input + en_len, (unsigned char *) _output + en_len, &_deKey);
		en_len += AES_BLOCK_SIZE;
	}
	int x = _output[inputlen - 1];
	outputlen = inputlen - x;
	return _output;
}

void CAes::resize(int size)
{
	_input = (char *) realloc(_input, size);
	_inputlen = size;
	_output = (char *) realloc(_output, size);
	_outputlen = size;
}


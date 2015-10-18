/*
print.c:
Copyright (C) 2009  david leels <davidontech@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/.
*/


typedef char * va_list;
#define _INTSIZEOF(n)   ((sizeof(n)+sizeof(int)-1)&~(sizeof(int) - 1) )
#define va_start(ap,v) ( ap = (va_list)&v + _INTSIZEOF(v) )
#define va_arg(ap,t) ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap)    ( ap = (va_list)0 )

const char *digits="0123456789abcdef";
char numbers[68];

static char print_buf[1024];

#define FORMAT_TYPE_MASK	0xff00
#define FORMAT_TYPE_SIGN_BIT	0x0100
#define FORMAT_TYPE_NONE	0x000
#define FORMAT_TYPE_CHAR	0x100
#define FORMAT_TYPE_UCHAR	0x200
#define FORMAT_TYPE_SHORT	0x300
#define FORMAT_TYPE_USHORT	0x400
#define FORMAT_TYPE_INT		0x500
#define FORMAT_TYPE_UINT	0x600
#define FORMAT_TYPE_LONG	0x700
#define FORMAT_TYPE_ULONG	0x800
#define FORMAT_TYPE_STR		0xd00
#define FORMAT_TYPE_PTR		0x900
#define FORMAT_TYPE_SIZE_T	0xb00

#define FORMAT_TYPE(x)	((x)&FORMAT_TYPE_MASK)
#define SET_FORMAT_TYPE(x,t)	do{\
(x)&=~FORMAT_TYPE_MASK;(x)|=(t);\
}while(0)
#define FORMAT_SIGNED(x)	((x)&FORMAT_TYPE_SIGN_BIT)

#define FORMAT_FLAG_MASK	0xffff0000
#define FORMAT_FLAG_SPACE	0x10000
#define FORMAT_FLAG_ZEROPAD	0x20000
#define FORMAT_FLAG_LEFT	0x40000
#define	FORMAT_FLAG_WIDTH	0x100000

#define FORMAT_FLAG(x)	((x)&FORMAT_FLAG_MASK)
#define SET_FORMAT_FLAG(x,f)	((x)|=(f))

#define FORMAT_BASE_MASK	0xff
#define FORMAT_BASE_O		0x08
#define FORMAT_BASE_X		0x10
#define FORMAT_BASE_D		0x0a
#define FORMAT_BASE_B		0x02

#define FORMAT_BASE(x)	(FORMAT_BASE_MASK&(x))
#define SET_FORMAT_BASE(x,t)	do{(x)&=~FORMAT_BASE_MASK;(x)|=(t);}while(0)

#define do_div(n,base) ({ \
	int __res; \
	__res = ((unsigned int) n) % (unsigned int) base; \
	n = ((unsigned int) n) / (unsigned int) base; \
	__res; })

void __put_char(char *p,int num){
	while(*p&&num--){
                *(volatile unsigned int *)0x20201000=*p++;
	};
}

void * memcpy(void * dest,const void *src,unsigned int count)
{
	char *tmp = (char *) dest, *s = (char *) src;
	while (count--)
		*tmp++ = *s++;
	return dest;
}

char *number(char *str, int num,int base,unsigned int flags){
	int i=0;
	int sign=0;

	if(FORMAT_SIGNED(flags)&&(signed int)num<0){
		sign=1;
		num=~num+1;
	}

	do{
		numbers[i++]=digits[do_div(num,base)];
	}while(num!=0);
	

	if(FORMAT_BASE(flags)==FORMAT_BASE_O){
		numbers[i++]='0';
	}else if(FORMAT_BASE(flags)==FORMAT_BASE_X){
		numbers[i++]='x';
		numbers[i++]='0';
	}else if(FORMAT_BASE(flags)==FORMAT_BASE_B){
		numbers[i++]='b';
		numbers[i++]='0';
	}
	if(sign)
		numbers[i++]='-';

	while (i-- > 0)
			*str++ = numbers[i];
	return str;
}

int format_decode(const char *fmt,unsigned int *flags){
	const char *start = fmt;

	*flags &= ~FORMAT_TYPE_MASK;
	*flags |= FORMAT_TYPE_NONE;
	for (; *fmt ; ++fmt) {
		if (*fmt == '%')
		break;
	}

	if (fmt != start || !*fmt)
		return fmt - start;

	do{
		fmt++;
		switch(*fmt){
			case 'l':
				SET_FORMAT_FLAG(*flags,FORMAT_FLAG_WIDTH);
				break;
			default:
				break;
		}
	}while(0);
	
	SET_FORMAT_BASE(*flags,FORMAT_BASE_D);
	switch (*fmt) {
		case 'c':
			SET_FORMAT_TYPE(*flags,FORMAT_TYPE_CHAR);
			break;

		case 's':
			SET_FORMAT_TYPE(*flags,FORMAT_TYPE_STR);
			break;

		case 'o':
			SET_FORMAT_BASE(*flags,FORMAT_BASE_O);
			SET_FORMAT_TYPE(*flags,FORMAT_TYPE_UINT);
			break;

		case 'x':
		case 'X':
			SET_FORMAT_BASE(*flags,FORMAT_BASE_X);
			SET_FORMAT_TYPE(*flags,FORMAT_TYPE_UINT);
			break;

		case 'd':
		case 'i':
			SET_FORMAT_TYPE(*flags,FORMAT_TYPE_INT);
			SET_FORMAT_BASE(*flags,FORMAT_BASE_D);
			break;
		case 'u':
			SET_FORMAT_TYPE(*flags,FORMAT_TYPE_UINT);
			SET_FORMAT_BASE(*flags,FORMAT_BASE_D);
			break;

	default:
		break;
	}
	return ++fmt-start;
}

int vsnprintf(char *buf, int size, const char *fmt, va_list args){
	int num;
	char *str, *end, c,*s;
	int read;
	unsigned int spec=0;

	str = buf;
	end = buf + size;

	if (end < buf) {
		end = ((void *)-1);
		size = end - buf;
	}

	while (*fmt) {
		const char *old_fmt = fmt;

		read = format_decode(fmt, &spec);
		fmt += read;

		if((FORMAT_TYPE(spec))==FORMAT_TYPE_NONE){
			int copy = read;
			if (str < end) {
				if (copy > end - str)
					copy = end - str;
					memcpy(str, old_fmt, copy);
			}
			str += read;

		}else if(spec&FORMAT_FLAG_WIDTH){
			//do nothing
		}else if(FORMAT_TYPE(spec)==FORMAT_TYPE_CHAR){
			c = (unsigned char) va_arg(args, int);
			if (str < end)
				*str = c;
			++str;
		}else if(FORMAT_TYPE(spec)==FORMAT_TYPE_STR){
			s = (char *) va_arg(args, char *);
			while(str<end&&*s!='\0'){
				*str++=*s++;
			}
		}else{
			if(FORMAT_TYPE(spec)==FORMAT_TYPE_INT){
				num = va_arg(args, int);
			}else if(FORMAT_TYPE(spec)==FORMAT_TYPE_ULONG){
				num = va_arg(args, unsigned long);
			}else if(FORMAT_TYPE(spec)==FORMAT_TYPE_LONG){
				num = va_arg(args, long);
			}else if(FORMAT_TYPE(spec)==FORMAT_TYPE_SIZE_T){
				num = va_arg(args, int);
			}else if(FORMAT_TYPE(spec)==FORMAT_TYPE_USHORT){
				num = (unsigned short) va_arg(args, int);
			}else if(FORMAT_TYPE(spec)==FORMAT_TYPE_SHORT){
				num = (short) va_arg(args, int);
			}else{
				num = va_arg(args, unsigned int);
			}
				str=number(str,num,spec&FORMAT_BASE_MASK,spec);
		}
	}
	if (size > 0) {
		if (str < end)
			*str = '\0';
		else
			end[-1] = '\0';
	}
	return str-buf;
}

void printk(const char *fmt, ...)
{
	va_list args;
	unsigned int i;

	va_start (args, fmt);
	i = vsnprintf (print_buf, sizeof(print_buf),fmt, args);
	va_end (args);

	__put_char (print_buf,i);
}

void test_printk(void){
	char *p="this is %s test";
	char c='H';
	int d=-256;
	int k=0;
	printk("testing printk\n");
	printk("test string :::	%s\ntest char ::: %c\ntest digit ::: %d\ntest X ::: %x\ntest unsigned ::: %u\ntest zero ::: %d\n",p,c,d,d,d,k);
}

test_httpd: test_httpd.c test_httpd.h
	gcc -o test_httpd test_httpd.c test_httpd.h


install:
	cp test_httpd.conf /etc/test_httpd.conf
	cp test_httpd /usr/bin/test_httpd

clean:
	rm test_httpd

uninstall:	
	rm /usr/bin/test_httpd


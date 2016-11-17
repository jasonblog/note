#files=zh-tw/*.md
#cmd=opencc -c zht2zhs.ini -i

#hans:
#	@mkdir -p zh-cn
#	@for name in ${files} ; do \
#		${cmd} $$name -o $${name//tw/cn} ; \
#	done

#init:
#	@cp README.md zh-tw/
#	@$(MAKE) hans

init:
	@gitbook init

serve:
	@gitbook serve

build:
	@gitbook build --gitbook=2.6

clean:
	rm -fr _book

github:
	@ghp-import _book -p -n

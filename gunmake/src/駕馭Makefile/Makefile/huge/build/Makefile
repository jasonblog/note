.PHONY: all clean

DIRS = $(ROOT)/source/foo/src \
	$(ROOT)/source/bar/src \
	$(ROOT)/source/huge/src

RM = rm
RMFLAGS = -fr
RMS = $(ROOT)/build/exes $(ROOT)/build/libs

all:
	@set -e; \
	for dir in $(DIRS); \
	do \
		cd $$dir && $(MAKE) ; \
	done
	@echo ""
	@echo ":-) Completed"
	@echo ""
	
clean:
	@set -e; \
	for dir in $(DIRS); \
	do \
		cd $$dir && $(MAKE) clean;\
	done
	$(RM) $(RMFLAGS) $(RMS)
	@echo ""
	@echo ":-) Completed"
	@echo ""

curPath=$(shell pwd)

.PHONY : all clean

all :
	@for n in $(curPath)/**/;\
		do \
			$(MAKE) -C $$n ; \
		done


clean :
	@for n in $(curPath)/**/;\
		do \
			$(MAKE) -C $$n clean; \
		done
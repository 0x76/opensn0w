.SILENT:

serno.h:
	@revh=; \
	if [ -d .git ]; then \
		revh=`git log -1 --pretty=oneline | cut -d' ' -f1 2>/dev/null` || :; \
	fi; \
	[ -z "$$revh" ] || echo "#define SERNO \"$$revh\"" >serno.h

config.h:
	@echo "Run ./autogen.sh first you idiot."
	@exit 1

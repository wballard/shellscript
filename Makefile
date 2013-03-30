DIFF ?= git --no-pager diff --ignore-all-space --color-words --no-index 
.PHONY: test

test: install
	$(MAKE) ascript pidgrep

ascript:
	./test/ascript > /tmp/$@
	$(DIFF) /tmp/$@ test/expected/$@

pidgrep:
	./test/pidgrep > /tmp/$@
	$(DIFF) /tmp/$@ test/expected/$@

install:
	coffee -o ./lib ./src/*.*coffee

clean:
	-rm -rf node_modules
	-rm -rf lib

rebuild: clean
	npm install

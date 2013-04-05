DIFF ?= git --no-pager diff --ignore-all-space --color-words --no-index 
.PHONY: test

test: 
	$(MAKE) ascript pidgrep

ascript:
	./test/ascript > /tmp/$@
	$(DIFF) /tmp/$@ test/expected/$@

pidgrep:
	./test/pidgrep > /tmp/$@
	$(DIFF) /tmp/$@ test/expected/$@

clean:
	-rm -rf node_modules build

rebuild: clean
	npm install

TESTS = test/*.js

all: test

build: clean configure compile

configure:
		node-pre-gyp configure

compile: configure
		node-pre-gyp build
		npm install .

test: build
		@./node_modules/nodeunit/bin/nodeunit \
					$(TESTS)

clean:
		node-pre-gyp clean


.PHONY: clean test build

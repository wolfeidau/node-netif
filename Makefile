TESTS = test/*.js

all: test

build: clean configure compile

configure:
		node-gyp configure

compile: configure
		node-gyp build
			npm install .

test: build list
		@./node_modules/nodeunit/bin/nodeunit \
					$(TESTS)

list:
		/sbin/ifconfig -a

clean:
		node-gyp clean


.PHONY: clean test build

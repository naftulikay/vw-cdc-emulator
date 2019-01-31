#!/usr/bin/make -f

SHELL:=$(shell which bash)

.PHONY: build clean deploy deps python-deps

python-deps:
	@pip2 install -q $(shell test -z "$$TRAVIS" && echo "--user") -r requirements.txt

deps: python-deps

clean: deps
	@platformio run --target clean

build: deps
	@platformio run

test: build
	@( \
		echo ; \
		if [ -t 2 ]; then echo -ne '\033[1;31m' ; fi ; \
		for i in $$(seq 60); do echo -n '-' ; done && echo ; \
		echo "WARNING: platformio unit testing is a paid feature 👎"; \
		for i in $$(seq 60); do echo -n '-' ; done && echo ; \
		if [ -t 2 ]; then echo -e '\033[0m'; fi ; \
		echo ; \
	) >&2
	@# try to run the tests
	@platformio test --verbose

deploy: build
	@platformio run --target upload

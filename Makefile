.PHONY: build upload

upload: build
	ino upload

build:
	ino build

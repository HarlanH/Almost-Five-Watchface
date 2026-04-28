c:
	npm run build
l: c
	deploypebble.sh load `ls build/*.pbw | head -n 1`
d: c
	deploypebble.sh reinstall `ls build/*.pbw | head -n 1`

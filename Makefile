SRC=main.vala ui/main-window.vala ui/ecg-monitor.vala ui/main-menu.vala parsers/ecg-parser.c

ecg-viewer: $(SRC)
	valac -o ecg-viewer -g --vapidir=./parsers/ --pkg gtk+-3.0 --pkg ecg-parser $(SRC) -X -lm -X -I./ -X -I./parsers/


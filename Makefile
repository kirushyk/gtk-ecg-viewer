SRC=main.vala main-window.vala ecg-monitor.vala main-menu.vala ecg-parser.c

../../bin/ecg-viewer: $(SRC)
	valac -o ../../bin/ecg-viewer -g --vapidir=. --pkg gtk+-3.0 --pkg ecg-parser $(SRC) -X -lm -X -I.


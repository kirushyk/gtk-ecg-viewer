SRC=main.vala main-window.vala ecg-monitor.vala main-menu.vala ../parsers/ecg-parser.c ../parsers/tlc5000.ecg.c ../parsers/tlc5000.bin.c

ecg-viewer: $(SRC)
	valac -o ecg-viewer -g --vapidir=../parsers/ --pkg gtk+-3.0 --pkg ecg-parser $(SRC) -X -g -X -lm -X -I./ -X -I../parsers/
clean:
	rm -f ecg-viewer
	rm -f *~


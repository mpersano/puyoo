#!/bin/sh

SHEET=SPRITES

SPRITE_PATH=sprites

FONT=/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf

convert \
	-background Transparent \
	-fill yellow \
	-stroke red \
	-strokewidth 1 \
	-font $FONT \
	-pointsize 18 \
	label:'combo\!' \
	png32:$SPRITE_PATH/combo.png

for i in $( seq 2 5 ); do
	convert \
		-background Transparent \
		-fill yellow \
		-stroke red \
		-strokewidth 1 \
		-font $FONT \
		-pointsize 22 \
		label:$i \
		png32:$SPRITE_PATH/$i.png
done

./pack-sprites -b 0 -w 256 -h 256 $SHEET $SPRITE_PATH && ( \
	convert $SHEET.000.png ../data/$SHEET.TGA;
	mv $SHEET.spr ../data/$SHEET.SPR )

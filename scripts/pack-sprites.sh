#!/bin/sh

SHEET=SPRITES

./pack-sprites $SHEET sprites 256 256 && ( \
	convert $SHEET.000.png ../data/$SHEET.TGA;
	mv $SHEET.spr ../data/$SHEET.SPR )

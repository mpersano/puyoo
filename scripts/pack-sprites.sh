#!/bin/sh

SHEET=SPRITES

./pack-sprites $SHEET sprites 256 256 && ( \
	convert $SHEET.000.png ../$SHEET.TGA;
	mv $SHEET.spr ../$SHEET.SPR )

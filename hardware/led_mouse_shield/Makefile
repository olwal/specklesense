#
# Makefile for generating gerbers and pngs.
# Tested on Advanced Circuits barebones, should work for 33 each, 
# gerbmerge, and Advanced Circuits. Pngs used for the fab modules 
# and the modella. 
#
# Copyright (C) 2011 Andy Bardagjy. 
#
# This makefile is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
# 
# This makefile is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
# Andy Bardagjy
# June 21, 2011
#

PROJECT?= mouse_shield

WHITE?= \#FFFFFF
BLACK?= \#000000

all: gerbers fab parts

# Component Side Gerber
$(PROJECT).cmp: $(PROJECT).brd
	eagle -X -dGERBER_RS274X -o$(PROJECT).cmp $(PROJECT).brd Top Pads Vias

# Solder Side Gerber
$(PROJECT).sol: $(PROJECT).brd
	eagle -X -dGERBER_RS274X -o$(PROJECT).sol $(PROJECT).brd Bottom Pads Vias

# Top Soldermask Gerber
$(PROJECT).stc: $(PROJECT).brd
	eagle -X -dGERBER_RS274X -o$(PROJECT).stc $(PROJECT).brd tStop

# Bottom Soldermask Gerber
$(PROJECT).sts: $(PROJECT).brd
	eagle -X -dGERBER_RS274X -o$(PROJECT).sts $(PROJECT).brd bStop

# Top Silkscreen Gerber
$(PROJECT).plc: $(PROJECT).brd
	eagle -X -dGERBER_RS274X -o$(PROJECT).plc $(PROJECT).brd Dimension tPlace tNames tValues

# Bottom Silkscreen Gerber
$(PROJECT).pls: $(PROJECT).brd
	eagle -X -dGERBER_RS274X -o$(PROJECT).pls $(PROJECT).brd Dimension bPlace bNames bValues

# Drills Xcellon
$(PROJECT).drd: $(PROJECT).brd
	eagle -X -dEXCELLON -o$(PROJECT).drd $(PROJECT).brd Drills Holes

# Board Outline Gerber
$(PROJECT).bor: $(PROJECT).brd
	eagle -X -dGERBER_RS274X -o$(PROJECT).bor $(PROJECT).brd Dimension

# Top Copper PNG
# So you *can* do this with eagle commands, but the holes don't work out right.
# Gotta use gerbv to do this
#$(PROJECT)_cmp.png: $(PROJECT).brd
#	eagle -C "ratsnest; display none Top Pads Vias; export image $(PROJECT)_cmp.png monochrome 600; quit;" $(PROJECT).brd
#	convert $(PROJECT)_cmp.png -negate $(PROJECT)_cmp.png
$(PROJECT)_cmp.png: gerbers
	gerbv gerbers/$(PROJECT).cmp gerbers/$(PROJECT).bor -b$(BLACK) -f$(WHITE) -f$(BLACK) -B0 -D600 -o $(PROJECT)_cmp.png -xpng
	convert $(PROJECT)_cmp.png -colorspace gray +dither -colors 2 -normalize $(PROJECT)_cmp.png
	convert $(PROJECT)_cmp.png -mattecolor white -frame 25x25 $(PROJECT)_cmp.png
	convert $(PROJECT)_cmp.png -strip $(PROJECT)_cmp.png
	convert $(PROJECT)_cmp.png -units PixelsPerInch -density 600 $(PROJECT)_cmp.png
	convert $(PROJECT)_cmp.png -colorspace RGB $(PROJECT)_cmp.png
	convert $(PROJECT)_cmp.png -type TrueColormatte PNG32:$(PROJECT)_cmp.png

# Drills PNG
# So you *can* do this with eagle commands, but the holes don't work out right.
# Gotta use gerbv to do this
#$(PROJECT)_drd.png: $(PROJECT).brd
#	eagle -C "ratsnest; display none Drills Holes Dimension; export image $(PROJECT)_drd.png monochrome 600; quit;" $(PROJECT).brd
$(PROJECT)_drd.png: gerbers
	gerbv gerbers/$(PROJECT).drd gerbers/$(PROJECT).bor -b$(WHITE) -f$(BLACK) -f$(BLACK) -B0 -D600 -o $(PROJECT)_drd.png -xpng
	convert $(PROJECT)_drd.png -colorspace gray +dither -colors 2 -normalize $(PROJECT)_drd.png
	convert $(PROJECT)_drd.png -mattecolor black -frame 25x25 $(PROJECT)_drd.png
	convert $(PROJECT)_drd.png -strip $(PROJECT)_drd.png
	convert $(PROJECT)_drd.png -units PixelsPerInch -density 600 $(PROJECT)_drd.png
	convert $(PROJECT)_drd.png -colorspace RGB $(PROJECT)_drd.png
	convert $(PROJECT)_drd.png -type TrueColormatte PNG32:$(PROJECT)_drd.png

gerbers: $(PROJECT).cmp $(PROJECT).sol $(PROJECT).stc $(PROJECT).sts $(PROJECT).plc $(PROJECT).pls $(PROJECT).drd $(PROJECT).bor
	mkdir -p gerbers
	mv $(PROJECT).{bor,cmp,drd,dri,gpi,sol,stc,sts,plc,pls} gerbers/
	zip gerbers/$(PROJECT)_advanced.zip gerbers/*.{bor,drd,cmp,sol,stc,sts,plc,pls}
	zip gerbers/$(PROJECT)_barebones.zip gerbers/*.{bor,drd,cmp,sol}

fab: $(PROJECT)_cmp.png $(PROJECT)_drd.png
	mkdir -p fab
	mv $(PROJECT){_cmp.png,_drd.png} fab/

view: gerbers
	#gerbv gerbers/$(PROJECT).{bor,drd,stc,plc,cmp,sts,pls,sol} &
	gerbv gerbers/$(PROJECT).{bor,drd,cmp,sol} &

parts: $(PROJECT).sch
	eagle -C "export partlist $(PROJECT).txt; quit;" $(PROJECT).sch
	eagle_parts $(PROJECT).csv $(PROJECT).txt
	mkdir parts
	mv $(PROJECT).{csv,txt} parts

clean:
	rm -rf $(PROJECT).{csv,txt,bor,cmp,drd,dri,gpi,sol,zip,png,path,sts,stc,pls,plc}
	rm -rf gerbers fab parts

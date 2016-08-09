/* Copyright (C) 1998  Thomas Portele

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Thomas Portele
tpo@ikp.uni-bonn.de http://www.ikp.uni-bonn.de/~tpo
IKP Poppelsdorfer Allee 47, D-53115 Bonn Germany

__________________________

*/
#include "PPCtglst.h"
#include <string.h>

PPCategorylist::PPCategorylist(char* in) {
	category = new char[strlen(in)+1] ;
   strcpy(category,in) ; }

PPCategorylist::~PPCategorylist() {
	delete(category) ; }

void PPCategorylist::list(FILE* lf) {
	fprintf(lf,"%s",category) ; }

int PPCategorylist::length() {
	return(strlen(category)) ; }

char* PPCategorylist::get_value() {
	return(category) ; }

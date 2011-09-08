/*
 * Copyright (c) 2002.  Lucas Ryan.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name Lucas Ryan nor the names of contributors may be 
 *    used to endorse or promote products derived from this software 
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

int letters[190][10] = {
	{0,0,0,0,0,0,0,0,0,0},		/*   */
	{0,4,4,4,4,4,0,4,0,0},		/* ! */
	{0,10,10,10,0,0,0,0,0,0}, 	/* " */
	{0,10,10,31,10,31,10,10,0,0}, 	/* # */
	{0,4,14,5,14,20,14,4,0,0}, 	/* $ */
	{0,18,21,10,4,10,21,9,0,0}, 	/* % */
	{0,2,5,5,2,21,9,22,0,0}, 	/* & */
	{0,12,4,2,0,0,0,0,0,0}, 	/* ' */
	{0,8,4,2,2,2,4,8,0,0},		/* ( */
	{0,2,4,8,8,8,4,2,0,0},		/* ) */
	{0,0,17,10,31,10,17,0,0,0}, 	/* * */
	{0,0,4,4,31,4,4,0,0,0}, 	/* + */
	{0,0,0,0,0,0,12,4,2,0}, 	/* , */
	{0,0,0,0,31,0,0,0,0,0}, 	/* - */
	{0,0,0,0,0,0,4,14,4,0}, 	/* . */
	{0,16,16,8,4,2,1,1,0,0}, 	/* / */
	{0,4,10,17,17,17,10,4,0,0}, 	/* 0 */
	{0,4,6,5,4,4,4,31,0,0}, 	/* 1 */
	{0,14,17,16,12,2,1,31,0,0}, 	/* 2 */
	{0,31,16,8,12,16,17,14,0,0}, 	/* 3 */
	{0,8,12,10,9,31,8,8,0,0}, 	/* 4 */
	{0,31,1,13,19,16,17,14,0,0}, 	/* 5 */
	{0,12,2,1,13,19,17,14,0,0}, 	/* 6 */
	{0,31,16,8,8,4,2,2,0,0}, 	/* 7 */
	{0,14,17,17,14,17,17,14,0,0}, 	/* 8 */
	{0,14,17,25,22,16,8,6,0,0}, 	/* 9 */
	{0,0,4,14,4,0,4,14,4,0}, 	/* : */
	{0,0,4,14,4,0,12,4,2,0}, 	/* ; */
	{0,16,8,4,2,4,8,16,0,0}, 	/* < */
	{0,0,0,31,0,31,0,0,0,0}, 	/* = */
	{0,1,2,4,8,4,2,1,0,0},		/* > */
	{0,14,17,8,4,4,0,4,0,0}, 	/* ? */
	{0,14,17,25,21,13,1,14,0,0}, 	/* @ */
	{0,4,10,17,17,31,17,17,0,0}, 	/* A */
	{0,15,18,18,14,18,18,15,0,0}, 	/* B */
	{0,14,17,1,1,1,17,14,0,0}, 	/* C */
	{0,15,18,18,18,18,18,15,0,0}, 	/* D */
	{0,31,1,1,15,1,1,31,0,0}, 	/* E */
	{0,31,1,1,15,1,1,1,0,0}, 	/* F */
	{0,14,17,1,1,25,17,14,0,0}, 	/* G */
	{0,17,17,17,31,17,17,17,0,0}, 	/* H */
	{0,14,4,4,4,4,4,14,0,0}, 	/* I */
	{0,28,16,16,16,16,17,14,0,0}, 	/* J */
	{0,17,9,5,3,5,9,17,0,0}, 	/* K */
	{0,1,1,1,1,1,1,31,0,0}, 	/* L */
	{0,17,17,27,21,17,17,17,0,0}, 	/* M */
	{0,17,17,19,21,25,17,17,0,0}, 	/* N */
	{0,14,17,17,17,17,17,14,0,0}, 	/* O */
	{0,15,17,17,15,1,1,1,0,0}, 	/* P */
	{0,14,17,17,17,17,21,14,16,0}, 	/* Q */
	{0,15,17,17,15,5,9,17,0,0}, 	/* R */
	{0,14,17,1,14,16,17,14,0,0}, 	/* S */
	{0,31,4,4,4,4,4,4,0,0}, 	/* T */
	{0,17,17,17,17,17,17,14,0,0}, 	/* U */
	{0,17,17,17,10,10,10,4,0,0}, 	/* V */
	{0,17,17,17,21,21,27,17,0,0}, 	/* W */
	{0,17,17,10,4,10,17,17,0,0}, 	/* X */
	{0,17,17,10,4,4,4,4,0,0}, 	/* Y */
	{0,31,16,8,4,2,1,31,0,0}, 	/* Z */
	{0,14,2,2,2,2,2,14,0,0}, 	/* [ */
	{0,1,1,2,4,8,16,16,0,0}, 	/* \ */
	{0,14,8,8,8,8,8,14,0,0}, 	/* ] */
	{0,4,10,17,0,0,0,0,0,0}, 	/* ^ */
	{0,0,0,0,0,0,0,0,31,0}, 	/* _ */
	{0,6,4,8,0,0,0,0,0,0},		/* ` */
	{0,0,0,14,16,30,17,30,0,0}, 	/* a */
	{0,1,1,13,19,17,19,13,0,0}, 	/* b */
	{0,0,0,14,17,1,17,14,0,0}, 	/* c */
	{0,16,16,22,25,17,25,22,0,0}, 	/* d */
	{0,0,0,14,17,31,1,14,0,0}, 	/* e */
	{0,12,18,2,15,2,2,2,0,0}, 	/* f */
	{0,0,0,22,9,6,1,14,17,14}, 	/* g */
	{0,1,1,13,19,17,17,17,0,0}, 	/* h */
	{0,4,0,6,4,4,4,14,0,0}, 	/* i */
	{0,8,0,12,8,8,8,9,9,6}, 	/* j */
	{0,1,1,17,9,7,9,17,0,0}, 	/* k */
	{0,6,4,4,4,4,4,14,0,0}, 	/* l */
	{0,0,0,11,21,21,21,17,0,0}, 	/* m */
	{0,0,0,13,19,17,17,17,0,0}, 	/* n */
	{0,0,0,14,17,17,17,14,0,0}, 	/* o */
	{0,0,0,13,19,19,13,1,1,1}, 	/* p */
	{0,0,0,22,25,25,22,16,16,16}, 	/* q */
	{0,0,0,13,19,1,1,1,0,0}, 	/* r */
	{0,0,0,14,1,14,16,15,0,0}, 	/* s */
	{0,2,2,15,2,2,18,12,0,0}, 	/* t */
	{0,0,0,17,17,17,25,22,0,0}, 	/* u */
	{0,0,0,17,17,10,10,4,0,0}, 	/* v */
	{0,0,0,17,17,21,21,10,0,0}, 	/* w */
	{0,0,0,17,10,4,10,17,0,0}, 	/* x */
	{0,0,0,17,17,25,22,16,17,14}, 	/* y */
	{0,0,0,31,8,4,2,31,0,0}, 	/* z */
	{0,24,4,8,6,8,4,24,0,0}, 	/* { */
	{0,4,4,4,4,4,4,4,0,0},		/* | */
	{0,3,4,2,12,2,4,3,0,0}, 	/* } */
	{0,18,21,9,0,0,0,0,0,0}, 	/* ~ */
	{0,4,0,4,4,4,4,4,0,0},		/* � */
	{0,0,4,30,5,5,5,30,4,0}, 	/* � */
	{0,12,18,2,7,2,18,13,0,0}, 	/* � */
	{0,0,0,17,14,10,14,17,0,0}, 	/* � */
	{0,17,17,10,4,31,4,4,4,0}, 	/* � */
	{0,4,4,4,0,4,4,4,0,0},		/* � */
	{0,14,1,7,9,18,28,16,14,0}, 	/* � */
	{10,0,0,0,0,0,0,0,0,0}, 	/* � */
	{0,14,17,21,19,21,17,14,0,0}, 	/* � */
	{0,14,9,13,10,32,15,0,0,0}, 	/* � */
	{0,0,0,18,9,4,9,18,0,0}, 	/* � */
	{0,0,0,0,15,8,0,0,0,0}, 	/* � */
	{0,0,0,0,31,0,0,0,0,0}, 	/* � */
	{0,14,17,23,19,19,17,14,0,0}, 	/* � */
	{31,0,0,0,0,0,0,0,0,0}, 	/* � */
	{0,4,10,4,0,0,0,0,0,0}, 	/* � */
	{0,0,4,4,31,4,4,31,0,0}, 	/* � */
	{6,9,4,2,15,0,0,0,0,0}, 	/* � */
	{7,8,6,8,7,0,0,0,0,0},		/* � */
	{12,6,0,0,0,0,0,0,0,0}, 	/* � */
	{0,0,0,17,17,17,19,13,1,0}, 	/* � */
	{0,30,23,23,22,20,20,20,0,0}, 	/* � */
	{0,0,0,0,4,0,0,0,0,0},		/* � */
	{0,0,0,0,0,0,0,0,4,2},		/* � */
	{4,6,4,4,14,0,0,0,0,0}, 	/* � */
	{0,6,9,41,6,0,15,32,0,0}, 	/* � */
	{0,32,0,4,41,18,9,4,0,0}, 	/* � */
	{1,33,1,1,51,24,20,30,16,0}, 	/* � */
	{1,1,1,1,11,20,16,8,28,0}, 	/* � */
	{3,4,2,4,19,24,20,30,16,0}, 	/* � */
	{0,4,0,4,4,2,17,14,0,0}, 	/* � */
	{2,4,14,17,17,31,17,17,0,0}, 	/* � */
	{8,4,14,17,17,31,17,17,0,0}, 	/* � */
	{4,10,14,17,17,31,17,17,0,0}, 	/* � */
	{18,13,14,17,17,31,17,17,0,0}, 	/* � */
	{10,0,14,17,17,31,17,17,0,0}, 	/* � */
	{4,10,14,49,49,63,49,49,0,0}, 	/* � */
	{0,30,5,4,28,7,4,28,0,0}, 	/* � */
	{0,14,17,1,1,1,17,14,4,2}, 	/* � */
	{2,31,1,1,15,1,1,31,0,0}, 	/* � */
	{8,31,1,1,15,1,1,31,0,0}, 	/* � */
	{4,31,1,1,15,1,1,31,0,0}, 	/* � */
	{10,31,1,1,15,1,1,31,0,0}, 	/* � */
	{2,14,4,4,4,4,4,14,0,0}, 	/* � */
	{8,14,4,4,4,4,4,14,0,0}, 	/* � */
	{4,14,4,4,4,4,4,14,0,0}, 	/* � */
	{10,14,4,4,4,4,4,14,0,0}, 	/* � */
	{0,15,18,18,23,18,18,15,0,0}, 	/* � */
	{12,17,17,19,21,25,17,17,0,0}, 	/* � */
	{2,14,17,17,17,17,17,14,0,0}, 	/* � */
	{8,14,17,17,17,17,17,14,0,0}, 	/* � */
	{4,14,17,17,17,17,17,14,0,0}, 	/* � */
	{14,14,17,17,17,17,17,14,0,0}, 	/* � */
	{10,14,17,17,17,17,17,14,0,0}, 	/* � */
	{0,0,0,17,10,4,10,17,0,0}, 	/* � */
	{0,14,25,25,21,19,19,14,0,0}, 	/* � */
	{2,21,17,17,17,17,17,14,0,0}, 	/* � */
	{8,21,17,17,17,17,17,14,0,0}, 	/* � */
	{4,27,17,17,17,17,17,14,0,0}, 	/* � */
	{10,17,17,17,17,17,17,14,0,0}, 	/* � */
	{8,21,17,10,4,4,4,4,0,0}, 	/* � */
	{0,1,15,17,15,1,1,1,0,0}, 	/* � */
	{0,14,17,17,15,17,17,15,1,0}, 	/* � */
	{0,2,4,14,16,30,17,30,0,0}, 	/* � */
	{0,8,4,14,16,30,17,30,0,0}, 	/* � */
	{0,4,10,14,16,30,17,30,0,0}, 	/* � */
	{0,20,10,14,16,30,17,30,0,0}, 	/* � */
	{0,10,0,14,16,30,17,30,0,0}, 	/* � */
	{4,10,4,14,16,30,49,30,0,0}, 	/* � */
	{0,0,0,15,20,31,4,31,0,0}, 	/* � */
	{0,0,0,14,17,1,17,14,4,2}, 	/* � */
	{0,2,4,14,17,31,1,14,0,0}, 	/* � */
	{0,8,4,14,17,31,1,14,0,0}, 	/* � */
	{0,4,10,14,17,31,1,14,0,0}, 	/* � */
	{0,10,0,14,17,31,1,14,0,0}, 	/* � */
	{2,4,0,6,4,4,4,14,0,0}, 	/* � */
	{4,2,0,6,4,4,4,14,0,0}, 	/* � */
	{4,10,0,6,4,4,4,14,0,0}, 	/* � */
	{0,10,0,6,4,4,4,14,0,0}, 	/* � */
	{0,3,12,14,17,17,17,14,0,0}, 	/* � */
	{0,20,10,13,19,17,17,17,0,0}, 	/* � */
	{0,2,4,14,17,17,17,14,0,0}, 	/* � */
	{0,8,4,14,17,17,17,14,0,0}, 	/* � */
	{0,4,10,14,17,17,17,14,0,0}, 	/* � */
	{0,20,10,14,17,17,17,14,0,0}, 	/* � */
	{0,10,0,14,17,17,17,14,0,0}, 	/* � */
	{0,0,4,0,31,0,4,0,0,0}, 	/* � */
	{0,0,0,30,25,21,19,15,0,0}, 	/* � */
	{0,2,4,17,17,17,25,22,0,0}, 	/* � */
	{0,8,4,17,17,17,25,22,0,0}, 	/* � */
	{0,4,10,17,17,17,25,22,0,0}, 	/* � */
	{0,10,0,17,17,17,25,22,0,0}, 	/* � */
	{0,8,4,17,17,25,22,16,17,14}, 	/* � */
	{0,0,1,15,17,17,17,15,1,1}, 	/* � */
	{0,10,0,17,17,25,22,16,17,14} 	/* � */
};

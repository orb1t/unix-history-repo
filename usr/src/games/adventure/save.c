/*-
 * Copyright (c) 1991, 1993 The Regents of the University of California.
 * All rights reserved.
 *
 * The game adventure was originally written in Fortran by Will Crowther
 * and Don Woods.  It was later translated to C and enhanced by Jim
 * Gillogly.  This code is derived from software contributed to Berkeley
 * by Jim Gillogly at The Rand Corporation.
 *
 * %sccs.include.redist.c%
 */

#ifndef lint
static char sccsid[] = "@(#)save.c	5.2 (Berkeley) %G%";
#endif /* not lint */

#include <stdio.h>
#include "hdr.h"

struct savestruct
{
	void *address;
	int width;
};

struct savestruct save_array[] =
{
	&abbnum,        sizeof(abbnum),
	&attack,        sizeof(attack),
	&blklin,        sizeof(blklin),
	&bonus,         sizeof(bonus),
	&chloc,         sizeof(chloc),
	&chloc2,        sizeof(chloc2),
	&clock1,        sizeof(clock1),
	&clock2,        sizeof(clock2),
	&closed,        sizeof(closed),
	&closng,        sizeof(closng),
	&daltlc,        sizeof(daltlc),
	&demo,          sizeof(demo),
	&detail,        sizeof(detail),
	&dflag,         sizeof(dflag),
	&dkill,         sizeof(dkill),
	&dtotal,        sizeof(dtotal),
	&foobar,        sizeof(foobar),
	&gaveup,        sizeof(gaveup),
	&holdng,        sizeof(holdng),
	&iwest,         sizeof(iwest),
	&k,             sizeof(k),
	&k2,            sizeof(k2),
	&knfloc,        sizeof(knfloc),
	&kq,            sizeof(kq),
	&latncy,        sizeof(latncy),
	&limit,         sizeof(limit),
	&lmwarn,        sizeof(lmwarn),
	&loc,           sizeof(loc),
	&maxdie,        sizeof(maxdie),
	&mxscor,        sizeof(mxscor),
	&newloc,        sizeof(newloc),
	&numdie,        sizeof(numdie),
	&obj,           sizeof(obj),
	&oldlc2,        sizeof(oldlc2),
	&oldloc,        sizeof(oldloc),
	&panic,         sizeof(panic),
	&saved,         sizeof(saved),
	&savet,         sizeof(savet),
	&scorng,        sizeof(scorng),
	&spk,           sizeof(spk),
	&stick,         sizeof(stick),
	&tally,         sizeof(tally),
	&tally2,        sizeof(tally2),
	&tkk,           sizeof(tkk),
	&turns,         sizeof(turns),
	&verb,          sizeof(verb),
	&wd1,           sizeof(wd1),
	&wd2,           sizeof(wd2),
	&wzdark,        sizeof(wzdark),
	&yea,           sizeof(yea),
	atloc,          sizeof(atloc),
	dloc,           sizeof(dloc),
	dseen,          sizeof(dseen),
	fixed,          sizeof(fixed),
	hinted,         sizeof(hinted),
	link,           sizeof(link),
	odloc,          sizeof(odloc),
	place,          sizeof(place),
	prop,           sizeof(prop),
	tk,             sizeof(tk),

	NULL,   0
};

save(outfile)   /* Two passes on data: first to get checksum, second */
char *outfile;  /* to output the data using checksum to start random #s */
{
	FILE *out;
	struct savestruct *p;
	char *s;
	int crc();
	long sum;
	int i;

	crc_start();
	for (p = save_array; p->address != NULL; p++)
		sum = crc(p->address, p->width);
	srandom((int) sum);

	if ((out = fopen(outfile, "wb")) == NULL)
	{
	    fprintf(stderr,
		"Hmm.  The name \"%s\" appears to be magically blocked.\n",
		outfile);
	    return 1;
	}
	fwrite(&sum, sizeof(sum), 1, out);      /* Here's the random() key */
	for (p = save_array; p->address != NULL; p++)
	{
		for (s = p->address, i = 0; i < p->width; i++, s++)
			*s = (*s ^ random()) & 0xFF;      /* Lightly encrypt */
		fwrite(p->address, p->width, 1, out);
	}
	fclose(out);
	return 0;
}

restore(infile)
char *infile;
{
	FILE *in;
	struct savestruct *p;
	char *s;
	long sum, cksum;
	int i;

	if ((in = fopen(infile, "rb")) == NULL)
	{
	    fprintf(stderr,
		"Hmm.  The file \"%s\" appears to be magically blocked.\n",
		infile);
	    return 1;
	}
	fread(&sum, sizeof(sum), 1, in);        /* Get the seed */
	srandom((int) sum);
	for (p = save_array; p->address != NULL; p++)
	{
		fread(p->address, p->width, 1, in);
		for (s = p->address, i = 0; i < p->width; i++, s++)
			*s = (*s ^ random()) & 0xFF;  /* Lightly decrypt */
	}
	fclose(in);

	crc_start();                            /* See if she cheated */
	for (p = save_array; p->address != NULL; p++)
		cksum = crc(p->address, p->width);
	if (sum != cksum)                       /* Tsk tsk */
	    return 2;                           /* Altered the file */
	/* We successfully restored, so this really was a save file */
	/* Get rid of the file, but don't bother checking that we did */
	return 0;
}

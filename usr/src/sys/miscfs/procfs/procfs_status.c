/*
 * Copyright (c) 1993 The Regents of the University of California.
 * Copyright (c) 1993 Jan-Simon Pendry
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Jan-Simon Pendry.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)procfs_status.c	8.1 (Berkeley) %G%
 *
 * From:
 *	$Id: procfs_status.c,v 3.1 1993/12/15 09:40:17 jsp Exp $
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/time.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/vnode.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/resource.h>
#include <sys/resourcevar.h>
#include <miscfs/procfs/procfs.h>

int
procfs_dostatus(curp, p, pfs, uio)
	struct proc *curp;
	struct proc *p;
	struct pfsnode *pfs;
	struct uio *uio;
{
	struct session *sess;
	struct tty *tp;
	struct ucred *cr;
	char *ps;
	char *sep;
	int pid, ppid, pgid, sid;
	int i;
	int xlen;
	int error;
	char psbuf[256];		/* XXX - conservative */

	if (uio->uio_rw != UIO_READ)
		return (EOPNOTSUPP);

	pid = p->p_pid;
	ppid = p->p_pptr ? p->p_pptr->p_pid : 0,
	pgid = p->p_pgrp->pg_id;
	sess = p->p_pgrp->pg_session;
	sid = sess->s_leader ? sess->s_leader->p_pid : 0;

/* comm pid ppid pgid sid maj,min ctty,sldr start ut st wmsg uid groups ... */

	ps = psbuf;
	bcopy(p->p_comm, ps, MAXCOMLEN);
	ps[MAXCOMLEN] = '\0';
	ps += strlen(ps);
	ps += sprintf(ps, " %d %d %d %d ", pid, ppid, pgid, sid);

	if ((p->p_flag&P_CONTROLT) && (tp = sess->s_ttyp))
		ps += sprintf(ps, "%d,%d ", major(tp->t_dev), minor(tp->t_dev));
	else
		ps += sprintf(ps, "%d,%d ", -1, -1);

	sep = "";
	if (sess->s_ttyvp) {
		ps += sprintf(ps, "%sctty", sep);
		sep = ",";
	}
	if (SESS_LEADER(p)) {
		ps += sprintf(ps, "%ssldr", sep);
		sep = ",";
	}
	if (*sep != ',')
		ps += sprintf(ps, "noflags");

	if (p->p_flag & P_INMEM)
		ps += sprintf(ps, " %d %d",
			p->p_stats->p_start.tv_sec,
			p->p_stats->p_start.tv_usec);
	else
		ps += sprintf(ps, " -1 -1");
	
	{
		struct timeval ut, st;

		calcru(p, &ut, &st, (void *) 0);
		ps += sprintf(ps, " %d %d %d %d",
			ut.tv_sec,
			ut.tv_usec,
			st.tv_sec,
			st.tv_usec);
	}

	ps += sprintf(ps, " %s",
		(p->p_wchan && p->p_wmesg) ? p->p_wmesg : "nochan");

	cr = p->p_ucred;

	ps += sprintf(ps, " %d %d", cr->cr_uid, cr->cr_gid);
	for (i = 0; i < cr->cr_ngroups; i++)
		ps += sprintf(ps, " %d", cr->cr_groups[i]);
	ps += sprintf(ps, "\n");

	xlen = ps - psbuf;
	xlen -= uio->uio_offset;
	ps = psbuf + uio->uio_offset;
	xlen = min(xlen, uio->uio_resid);
	if (xlen <= 0)
		error = 0;
	else
		error = uiomove(ps, xlen, uio);

	return (error);
}

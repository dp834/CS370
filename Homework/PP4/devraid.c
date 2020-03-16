#include	"dat.h"
#include	"fns.h"
#include	"error.h"

/** file descriptors for raid0
 *   each file will be a copy of the other
 */
static Chan *chan1;
static Chan *chan2;

enum
{
	Qdir = 0,		/* must start at 0 representing a directory */
	Qdata,
	Qctl
};

Dirtab raidtab[] =
{
    ".",		{Qdir, 0, QTDIR},	0,	0555,
    "data",	    {Qdata}         ,	0,	0666,
    "ctl",	    {Qctl}          ,	0,	0666,
};


static void
setfilesize()
{
    uchar db[128];
    Dir *d;
    vlong l1, l2;

    d = smalloc(sizeof(*d) + 128);


    if(waserror()){
        free(d);
        nexterror();
    }

    if(chan1 != nil){
        devtab[chan1->type]->stat(chan1, db, 128);
        if(convM2D(db, 128, d, (char*)&d[1]) == 0){
            error(Eshortstat);
        }
        l1 = d->length;
    }else{
        l1 = 0;
    }

    if(chan2 != nil){
        devtab[chan2->type]->stat(chan2, db, 128);
        if(convM2D(db, 128, d, (char*)&d[1]) == 0){
            error(Eshortstat);
        }
        l2 = d->length;
    }else{
        l2 = 0;
    }

    raidtab[1].length = (l1 < l2) ? l1 : l2;

    poperror();
    free(d);
}

static Chan*
raidattach(char *spec)
{
    return devattach('R', spec);
}

static Walkqid*
raidwalk(Chan *c, Chan *nc, char **name, int nname)
{
    return devwalk(c, nc, name, nname, raidtab, nelem(raidtab), devgen);
}

static int
raidstat(Chan *c, uchar *db, int n)
{
    return devstat(c, db, n, raidtab, nelem(raidtab), devgen);
}

static Chan*
raidopen(Chan *c, int omode)
{
	return devopen(c, omode, raidtab, nelem(raidtab), devgen);
}

static int ctlsummary(char*, int);

static void
raidclose(Chan *c)
{
	if((c->flag & COPEN) == 0)
		return;

	switch(c->qid.path) {
	case Qdir:
	case Qctl:
	case Qdata:
		break;
	default:
		error(Egreg);
	}
}

static long
raidread(Chan *c, void *va, long n, vlong offset)
{
    char *buf;
    int i;

    if(c->qid.type & QTDIR)
        return devdirread(c, va, n, raidtab, nelem(raidtab), devgen);
    switch(c->qid.path) {
        case Qdata:
            if(chan1 != nil && !waserror()){
                i = devtab[chan1->type]->read(chan1, va, n, offset);
                poperror();
            }else if(chan2 != nil){
                /* pop previous waserror off stack, we are handling here */
                poperror();
                if(!waserror()){
                    i = devtab[chan2->type]->read(chan2, va, n, offset);
                    poperror();
                }else{
                    /* handling error here */
                    poperror();
                    error("Both Mirros failed");
                }
            }else{
                error("Mirror1 failed, Mirror 2 nil");
            }
            return i;
        case Qctl:
            buf = smalloc(READSTR);
            if(waserror()){
                free(buf);
                nexterror();
            }
            i = ctlsummary(buf, READSTR);
            i = readstr(offset, va, i, buf);
            poperror();
            free(buf);
            return i;
    }

    return 0;
}

static long
raidwrite(Chan *c, void *va, long n, vlong offset)
{
	Cmdbuf *cb;
    long i = 0, j = 0;

    switch(c->qid.path) {
        case Qdata:
            if(chan1 != nil){
                i = devtab[chan2->type]->write(chan1, va, n, offset);
            }

            if(chan2 != nil){
                j = devtab[chan2->type]->write(chan2, va, i, offset);
            }
            setfilesize();
            if(i != j){
                /* Drives are no longer matching
                 * Try to recover if fully implementing? */
                return (i < j)? i : j;
            }

            return i;
        case Qctl:
            /* takes a command of the form 'bind file1 file2' */
            cb = parsecmd(va, n);
            if(waserror()){
                free(cb);
                nexterror();
            }

            /* check that we do have 3 arguments and first is bind */
            if(cb->nf != 3 || strcmp(cb->f[0], "bind")!= 0){
                error(Ebadarg);
            }

            if(chan1 != nil){
                cclose(chan1);
            }
            if(chan2 != nil){
                cclose(chan2);
            }



            chan1 = namec(cb->f[1], Aopen, ORDWR, 0);
            chan2 = namec(cb->f[2], Aopen, ORDWR, 0);
            setfilesize();

            poperror();
            free(cb);
    }
    return n;
}

static int
ctlsummary(char *buf, int bsize)
{
    return 0;
}

Dev raiddevtab = {
    'R',
    "raid",

    devinit,
    raidattach,
    raidwalk,
    raidstat,
    raidopen,
    devcreate,
    raidclose,
    raidread,
    devbread,
    raidwrite,
    devbwrite,
    devremove,
    devwstat
};


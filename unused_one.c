#include "cs.h"
#include "mysparse.h"


/*adds an element to matrix - requires 2 allocations*/

int addone(cs *A, int r, int c, double v)
{
	if (!A) return 4; /*return 4 if invalid*/
	if (v==0) return 1; /*return 1 if value is zero*/
	if (errbound(A,r,c)) return 3; /*return 3 if out of bounds*/
	if (acc(A,r,c)) return 2; /*return 2 if existing value*/

	if (A->nz == -1){
		int cp1,cp2,k,t; /*index*/
		int newmax = A->nzmax + 1;
		int ok = 1;

		cp1 = A->p[c]; /*index of Ci and Cx array to start iteration*/
		cp2 = A->p[c+1];
		for (k = cp1; k < cp2; k++){
			if (A->i[k] > r){
				break;
			}
		}
		for (t=c+1; t<A->n+1; t++){ /*assume no change in Cp size with columns*/
			A->p[t]++;
		}
		A->i = cs_realloc (A->i,newmax,sizeof(CS_INT),&ok);
		A->x = cs_realloc (A->x,newmax,sizeof(CS_ENTRY),&ok);
		for (t=A->nzmax; t>k; t--){ /*shift values after index k to right*/
			A->i[t] = A->i[t-1];
			A->x[t] = A->x[t-1];
		}
		A->i[k] = r;
		A->x[k] = v;
		A->nzmax++;
		A->m = CS_MAX (A->m, r+1) ;
		A->n = CS_MAX (A->n, c+1) ;
		return 0; /*return 0 if success*/
	}
	else if (A->nz >= 0){
		cs_entry(A,r,c,v);
		return 0;
	}
	else return 4;
}

/*modify an existing element value, when passed matrix, index, and new value*/

int modone(cs *A, int r, int c, double v)
{
	if (!A) return 4; /*return 4 if invalid*/
	if (errbound(A,r,c)) return 3; /*return 3 if outside bounds*/
	if (v==0) return 1; /*return 1 if value is zero*/
	if (!acc(A,r,c)) return 2; /*return 2 if no value at index*/	

	int *i, *p, a, b, k, nz;
	double *x;
	i = A->i; p = A->p; x = A->x; nz = A->nz;

	if (nz == -1)
	{
		for (k=p[c];k<p[c+1];k++) { if (r == i[k]) break; }
		x[k] = v;
		return 0; /*return 0 if successful*/
	}
	else if (nz >= 0)
	{
		for (k=0; k<nz; k++){ if (i[k] == r && p[k] == c) break; }
		x[k] = v;
		return 0;
	}
	else return 4; /*return 4 if invalid*/
}

/*removes value from matrix*/

int delone(cs *A, int r, int c)
{
	if (!A) return 4; /*return 4 if invalid*/
	if (errbound(A,r,c)) return 3; /*return 3 if outside bounds*/
	if (!(acc(A,r,c))) return 2; /*return 2 if no value at index*/

	int k,s,a,b,nz,*i, *p;
	double *x;
	i = A->i; p = A->p; x = A->x; nz = A->nz;
	if (nz == -1)
	{
		a = p[c]; b = p[c+1];
		for (k=0;k<b-a;k++) { if (r == i[k+a]) break; }
		x[p[c]+k] = 0;
		cs_dropzeros (A); /*works only on compressed*/
		return 0; /*return 0 if success*/
	}
	else if (nz >= 0)
	{
		for (k=0; k<nz; k++){ if (i[k] == r && p[k] == c) break; }
		for (s=k;s<nz-1;s++)
		{
			A->i[s] = A->i[s+1];
			A->p[s] = A->p[s+1];
			A->x[s] = A->x[s+1];
		}
		A->nz--;
		return 0;
	}
	else return 4; /*return 4 if invalid matrix*/
}


/*less efficient version of add - requires 17 allocations*/

cs * addone2(cs *A, int r, int c, double v)
{
	cs *B, *BC, *C;
	int m, n;
	m = A->m; n = A->n;

	B = cs_spalloc (m,n,m*n,1,1);
	cs_entry(B,r,c,v);
	BC = cs_compress (B);
	cs_spfree (B);

	C = cs_add (A, BC, 1, 1);
	cs_spfree(A);	
	cs_spfree(BC);
	return C;
}



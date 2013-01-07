#include "lap.hpp"

void lap(int size, Real **C, int *col, int *row)
{
  int i,j,k,l;
  int m = size, n = size;
  Real s;
  int t, q, unmatched;
  Real total;
  
  /* intitialize the row and col */
  for (i=0;i<m;++i) 
    col[i]=0;
  for (i=0;i<n;++i) 
    row[i]=0;
  
  /* create and initialize parent_row & unchosen_row */
  int *parent_row = (int *)malloc(sizeof(int)*n);
  for (i=0;i<n;++i) 
    parent_row[i] = 0;
  int *unchosen_row = (int *)malloc(sizeof(int)*n); 
  for (i=0;i<m;++i) 
    unchosen_row[i]=0;
  
  /* create and initialize row_dec, col_inc & slack*/
  Real *row_dec = (Real*)malloc(sizeof(Real)*m); 
  for (i=0;i<m;++i) 
    row_dec[i] = 0;
  Real *col_inc = (Real*)malloc(sizeof(Real)*n); 
  for (i=0;i<n;++i) 
    col_inc[i] = 0;
  Real *slack = (Real*)malloc(sizeof(Real)*n);  
  for (i=0;i<n;++i) 
    slack[i] = 0;

  /* create and initialize slack_row */
  int *slack_row = (int *)malloc(sizeof(int)*n); 
  for (i=0;i<n;++i) slack_row[i] = 0;
  
  for(l=0;l<n;l++){
    s = C[0][l];
    for(k=1;k<n;k++)
      if(C[k][l]<s)
	s = C[k][l];
    if(s!=0)
      for(k=0;k<n;k++)
	C[k][l] -= s;
  }
  
  t = 0;
  for(l= 0;l<n;l++){
    row[l] = -1;
    parent_row[l] = -1;
    col_inc[l] = 0;
    slack[l] = INF;
  }
 for(k=0;k<m;k++){
   s = C[k][0];
   for(l=1;l<n;l++)
     if(C[k][l]<s) s = C[k][l];
   row_dec[k] = s;
   for(l=0;l<n;l++)
     if((s==C[k][l])&&(row[l]<0)){
       col[k] = l;
       row[l] = k;
       goto row_done;
     }
   col[k] = -1;
   unchosen_row[t++] = k;
 row_done:;
 }
 
 if(t==0)goto done;
 unmatched = t;
 while(1){
   q= 0;
   while(1){
    while(q<t){
      {
	k = unchosen_row[q];
	s = row_dec[k];
	for(l= 0;l<n;l++)
	  if(slack[l]){
	    
	    total = C[k][l]-s+col_inc[l];
	    if(total<slack[l]){
	      if(total==0){
		if(row[l]<0)
		  goto breakthrough;
		slack[l] = 0;
		parent_row[l] = k;
		unchosen_row[t++] = row[l];
	      }else{
		slack[l] = total;
		slack_row[l] = k;
	      }
	    }
	  }
      }
      q++;
    }
    
    s= INF;
    for(l=0;l<n;l++)
      if(slack[l]&&slack[l]<s)
	s = slack[l];
    for(q=0;q<t;q++)
      row_dec[unchosen_row[q]] += s;
    for(l=0;l<n;l++)
      if(slack[l]){
	slack[l] -= s;
	if(slack[l]==0)
	  {
	    k=slack_row[l];
	    if(row[l]<0){
	      for(j=l+1;j<n;j++)
		if(slack[j]==0)
		  col_inc[j] += s;
	      goto breakthrough;
	    }else{
	      parent_row[l] = k;
	      unchosen_row[t++] = row[l];
	    }
	  }
      }else col_inc[l] += s;
   }
 breakthrough:  
   while(1){
     j = col[k];
     col[k] = l;
     row[l] = k;
     if(j<0) break;
     k = parent_row[j];
     l = j;
   }
   if(--unmatched==0)goto done;
   t= 0;
   for(l=0;l<n;l++){
     parent_row[l] = -1;
     slack[l] = INF;
   }
   for(k=0;k<m;k++)
     if(col[k]<0){
       unchosen_row[t++] = k;
     }
 }
 done: 
 free(parent_row);
 free(unchosen_row);
 free(row_dec);
 free(col_inc);
 free(slack);
 free(slack_row);
}


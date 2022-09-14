/*
	kalign2_distance_calculation.c 
	
	Released under GPL - see the 'COPYING' file   
	
	Copyright (C) 2006 Timo Lassmann <timolassmann@gmail.com>
	
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
	Please send bug reports, comments etc. to:
	timolassmann@gmail.com
*/

#include "kalign2.h"

float** protein_pairwise_alignment_distance(struct alignment* aln,float** dm,struct parameters* param,float**subm, int nj)
{
	int i,j,c;
	int * path = 0;
	int len_a = 0;
	int len_b = 0;
	struct dp_matrix *dp = 0;
	int a,b;
	
	unsigned int numseq;
	unsigned int numprofiles;

	struct kalign_context *ctx = get_kalign_context();
	numseq = ctx->numseq;
	numprofiles = ctx->numprofiles;

	k_printf("Distance Calculation:\n");
	
	b = (numseq*(numseq-1))/2;
	a = 1;	
	
	
	
	dp = dp_matrix_alloc(dp,511,511);
	
	if (nj){
		dm = malloc (sizeof(float*)*numprofiles);
		for (i = numprofiles;i--;){
			dm[i] = malloc (sizeof (float)*(numprofiles));
			for (j = numprofiles;j--;){
				dm[i][j] = 0.0f;
			}
		}
	}else{
		dm = malloc (sizeof(float*)*numseq);
		for (i = numseq;i--;){
			dm[i] = malloc (sizeof (float)*(numseq));
			for (j = numseq;j--;){
				dm[i][j] = 0.0f;
			}
		}
	}
	
	/*dm = malloc (sizeof(float*)*numprofiles);
	for (i = numprofiles;i--;){
		dm[i] = malloc (sizeof (float)*(numprofiles));
		for (j = numprofiles;j--;){
			dm[i][j] = 0.0f;
		}
	}*/
	for (i = 0; i < numseq-1;i++){
		len_a = aln->sl[i];
		for(j = i+1; j < numseq;j++){
			
			len_b = aln->sl[j];
			path = malloc(sizeof(int) * (len_a+len_b+2));
			for (c = len_a+len_b+2;c--;){
				path[c] = 0;
			}
			dp = dp_matrix_realloc(dp,len_a,len_b);
			path = ss_dyn(subm,path,dp,aln->s[i],aln->s[j],len_a,len_b);
			dm[i][j] = get_distance_from_pairwise_alignment(path,aln->s[i],aln->s[j]);
			dm[j][i] = dm[i][j];
			k_printf("Distance Calculation: %8.0f percent done",(float)a /(float)b * 100);
			set_task_progress((float)a /(float)b * 50);
			a++;
			
			free(path);
		}
	}
	dp_matrix_free(dp);
	return dm;
}

float get_distance_from_pairwise_alignment(int* path,int* seq1,int* seq2)
{
	float dist = 0;
 	int i,j,c;
 	int pairs = 0;
 	int identical = 0;
 	i = 0;
 	j = 0;
 	c = 1;
	while(path[c] != 3){		
		if (!path[c]){
			if (seq1[i] == seq2[j]){
				identical++;
			}
			pairs++;
			i++;
			j++;	
		}
		if (path[c] & 1){
			j++;
		}
		if (path[c] & 2){
			i++;		
		}
		c++;
	}
	dist = (float)identical/(float)pairs*100;
	//dist = (float)identical/(float)c;//pairs*100;
	return dist; 
}


float** protein_wu_distance2(struct alignment* aln,float** dm,struct parameters* param)
{
	struct node* hash[1024];
	int i,j;
	unsigned int hv;
	int*p =0;

	unsigned int numseq;
	unsigned int numprofiles;

	struct kalign_context *ctx = get_kalign_context();
	numseq = ctx->numseq;
	numprofiles = ctx->numprofiles;

	for (i = 0;i < 1024;i++){
		hash[i] = 0;
	}	
	
	if(!aln->ft){
		aln->ft =  malloc(sizeof(struct feature* ) * (numseq));

		for(i =0;i < numseq;i++){
			aln->ft[i] = 0;
		}
	}
	dm = malloc (sizeof(float*)*numprofiles);
	for (i = numprofiles;i--;){
		dm[i] = malloc (sizeof (float)*(numprofiles));
		for (j = numprofiles;j--;){
			dm[i][j] = 0.0f;
		}
	}

	
	for (i = 0; i < numseq-1;i++){
		p = aln->s[i];
				
		for (j = aln->sl[i]-2;j--;){
			hv = (p[j] << 5) + p[j+1];
			hash[hv] = insert_hash(hash[hv],j+1);
		//	hash[hv] = insert_hash(hash[hv],j+1);
			hv = (p[j] << 5) + p[j+2];
			hash[hv] = insert_hash(hash[hv],j+1);
			hv = (p[j+1] << 5) + p[j+2];
			hash[hv] = insert_hash(hash[hv],j+1);
		}

		for (j = i+1; j < numseq;j++){
			dm[i][j] = protein_wu_distance_calculation3(hash,aln->s[j],aln->sl[j],aln->sl[j]+aln->sl[i],param->zlevel);
		//	aln = protein_wu_sw2(hash,aln,i,j);
			dm[i][j] /= (aln->sl[i] > aln->sl[j]) ? aln->sl[j] : aln->sl[i];
			dm[j][i] = dm[i][j];
		}
		
		for (j = 1024;j--;){
			if (hash[j]){
				remove_nodes(hash[j]);
				hash[j] = 0;
			}
		}	
	}
	return dm;
}


float protein_wu_distance_calculation2(struct node* hash[],int* seq,int seqlen,int diagonals,int mode)
{

	struct node* node_p;
	int* d = 0;
	float out = 0.0;
	int i;
	unsigned int hv;

	d = malloc(sizeof(int)*diagonals);
	//for (i = diagonals;i--;){
	for (i = 0;i < diagonals;i++){
		d[i] = 0;
	}
	for (i = seqlen-2;i--;){

		hv = (seq[i] << 5) + seq[i+1];
		//printf("3:%d\n",hv);
		if (hash[hv]){
			node_p = hash[hv];
			while(node_p){
				d[node_p->pos]++;
		//		d[node_p->pos+1]++;
				node_p = node_p->next;
			}
		}


		hv = (seq[i] << 5) + seq[i+2];
		//printf("3:%d\n",hv);
		if (hash[hv]){
			node_p = hash[hv];
			while(node_p){
				d[node_p->pos]++;
		//		d[node_p->pos+1]++;
				node_p = node_p->next;
			}
		}
		hv = (seq[i+1] << 5) + seq[i+2];
		if (hash[hv]){
			node_p = hash[hv];
			while(node_p){
				d[node_p->pos]++;
				node_p = node_p->next;
			}
		}	
		d++;
	}

	//exit(0);
	d -= (seqlen-2);

	for (i = diagonals;i--;){
		//printf("%d ",d[i]);
		if(d[i] > mode){
			out += d[i];
		}
	}
	free(d);
	return out;
}



struct alignment* protein_wu_sw(struct node* hash[],struct alignment* aln,int a,int b)
{
	int*seq = aln->s[b];
	int len_a = aln->sl[b];
	int len_b = aln->sl[a];
	struct node* node_p = 0;
	int i,c;
	unsigned int hv;
	//int notel = aln->lsn[a] + aln->lsn[b];
	
	
	
	struct feature *n = 0;
	
	//float counta[1024];
	//float countb[1024];

	int *weight = 0;
	int *len = 0;
	int* added = 0;
	
	weight = malloc(sizeof(int*)*(len_a+len_b-1));
	len = malloc(sizeof(int*)*(len_a+len_b-1));
	added = malloc(sizeof(int*)*(len_a+len_b-1));
	for (i = 0; i <(len_a+len_b-1);i++){
		weight[i] = 0;
		len[i] = 0;
		added[i] = 0;
	}
	

	
	//for (i = 0; i <1024;i++){
	//	counta[i] = 0;
	//	countb[i] = 0;
	//	if(hash[i]){
	//		node_p = hash[i];
	//		while(node_p){
	//			countb[i]++;
	//			node_p = node_p->next;
	//		}
	//		k_printf("COUNT:%d	%f\n",i,countb[i]);
	//	}
		
	//}
	//for (i = len_a-2;i--;){
	//	hv = (seq[i+1] << 5) + seq[i+2];
	//	counta[hv]++;
	//	hv = (seq[i] << 5) + seq[i+1];
	//	counta[hv]++;
	//	hv = (seq[i] << 5) + seq[i+2];
	//	counta[hv]++;
	//}

	c = 1;
	for (i =  len_a-2;i--;){
		for (hv = 0; hv <(len_a+len_b-1);hv++){
		added[hv] = 0;
		}
	
		hv = (seq[i] << 5) + seq[i+1];
		//printf("3:%d\n",hv);
		if (hash[hv]){
			node_p = hash[hv];
			while(node_p){
				added[node_p->pos+c] = 1; 
				weight[node_p->pos+c]++;
	//			len[node_p->pos+c] = 1 + len[node_p->pos+c];
				node_p = node_p->next;
			}
		}


		hv = (seq[i] << 5) + seq[i+2];
		//printf("3:%d\n",hv);
		if (hash[hv]){
			node_p = hash[hv];
			while(node_p){
				added[node_p->pos+c] = 1; 
				weight[node_p->pos+c]++;
	//			len[node_p->pos+c] = 1 + len[node_p->pos+c];
				node_p = node_p->next;
			}
		}
		hv = (seq[i+1] << 5) + seq[i+2];
		//printf("3:%d\n",hv);
		if (hash[hv]){
			node_p = hash[hv];
			while(node_p){
				added[node_p->pos+c] = 1; 
				weight[node_p->pos+c]++;
	//			len[node_p->pos+c] = 1 + len[node_p->pos+c];
				node_p = node_p->next;
			}
		}
	//	k_printf("pos_a:%d	",i+1);
		
		for (hv = 0; hv <(len_a+len_b-1);hv++){
			len[hv] += added[hv];
			if(!added[hv] && len[hv]){
				if(len[hv] > 10){
					n = malloc(sizeof(struct feature));
					n->next = 0;
					n->color = 0;
					n->type = malloc(sizeof(char)*9);
					n->type[0] = 'w';
					n->type[1] = 'u';
					n->type[2] = 'm';
					n->type[3] = 'a';
					n->type[4] = 'n';
					n->type[5] = 'b';
					n->type[6] = 'e';
					n->type[7] = 'r';
					n->type[8] = 0;
					
					n->start = i+2;
					n->end = len[hv]+n->start -1;
					
					n->note = malloc(sizeof(char)*(2));
					n->note[0] = 'w';
					n->note[1] = 0;
					
					/*n->note = malloc(sizeof(char)*(notel+1));
					for (j = 0;j < aln->lsn[a];j++){
						n->note[j] = aln->sn[a][j];
					}
					while(j < notel){
						n->note[j] = aln->sn[b][j-aln->lsn[a]];
						j++;
					}
					n->note[notel] = 0;*/
					//n->note[0] = 'w';
					//n->note[1] = 0;
					
					
					if(! aln->ft[b]){
						 aln->ft[b] = n;
					}else{
						n->next = aln->ft[b];
						aln->ft[b] = n;
					}
					//if((old_n =  aln->ft[b])!= 0){
					//	while(old_n->next!=0){
					//		old_n = old_n->next;
					//	}
					//	old_n->next = n;
					//}else{
					//	 aln->ft[b] = n;
					//}
					n = 0;
					n = malloc(sizeof(struct feature));
					n->next = 0;
					n->color = 0;
					
					n->type = malloc(sizeof(char)*9);
					n->type[0] = 'w';
					n->type[1] = 'u';
					n->type[2] = 'm';
					n->type[3] = 'a';
					n->type[4] = 'n';
					n->type[5] = 'b';
					n->type[6] = 'e';
					n->type[7] = 'r';
					n->type[8] = 0;
					

					n->start = (hv - (len_a))+i+3;
					n->end = len[hv]+n->start -1;
					
					n->note = malloc(sizeof(char)*(2));
					n->note[0] = 'w';
					n->note[1] = 0;
					
					/*n->note = malloc(sizeof(char)*(notel+1));
					for (j = 0;j < aln->lsn[a];j++){
						n->note[j] = aln->sn[a][j];
					}
					while(j < notel){
						n->note[j] = aln->sn[b][j-aln->lsn[a]];
						j++;
					}
					n->note[notel] = 0;*/

					
					if(! aln->ft[a]){
						 aln->ft[a] = n;
					}else{
						n->next = aln->ft[a];
						aln->ft[a] = n;
					}
					
					//if((old_n =  aln->ft[a])!= 0){
					//	while(old_n->next!=0){
					//		old_n = old_n->next;
					//	}
					//	old_n->next = n;
					//}else{
					//	 aln->ft[a] = n;
					//}
					n = 0;
	//				k_printf("\nDiagonal found A:%d	%d\n",i+2,len[hv]);
	//				k_printf("Diagonal found	B:%d	%d\n",(hv - (len_a))+i+3,len[hv]);
				}
				len[hv] = 0;
				weight[hv] = 0;
			}
	//		k_printf("%d,%d	",hv,(hv - (len_a))+i+3);
		}
	//	k_printf("\n");
		c++;
	}
	i++;
	
	//k_printf("pos_a:%d	",i);
	for (hv = 0; hv <(len_a+len_b-1);hv++){
		if(len[hv]){
			if(len[hv] > 10){
				n = malloc(sizeof(struct feature));
				n->next = 0;
				n->color = 0;
				
				n->type = malloc(sizeof(char)*9);
				n->type[0] = 'w';
				n->type[1] = 'u';
				n->type[2] = 'm';
				n->type[3] = 'a';
				n->type[4] = 'n';
				n->type[5] = 'b';
				n->type[6] = 'e';
				n->type[7] = 'r';
				n->type[8] = 0;

				n->start = i+1;
				n->end = len[hv]+n->start-1;
				/*
				n->note = malloc(sizeof(char)*(notel+1));
				for (j = 0;j < aln->lsn[a];j++){
					n->note[j] = aln->sn[a][j];
				}
				while(j < notel){
					n->note[j] = aln->sn[b][j-aln->lsn[a]];
					j++;
				}
				n->note[notel] = 0;*/
				
				n->note = malloc(sizeof(char)*(2));
				n->note[0] = 'w';
				n->note[1] = 0;
				
				if(! aln->ft[b]){
					 aln->ft[b] = n;
				}else{
					n->next = aln->ft[b];
					aln->ft[b] = n;
				}
				
				/*
				if((old_n =  aln->ft[b])!= 0){
					while(old_n->next!=0){
						old_n = old_n->next;
					}
					old_n->next = n;
				}else{
					 aln->ft[b] = n;
				}*/
				n = 0;
				n = malloc(sizeof(struct feature));
				n->next = 0;
				n->color = 0;
				
				n->type = malloc(sizeof(char)*9);
				n->type[0] = 'w';
				n->type[1] = 'u';
				n->type[2] = 'm';
				n->type[3] = 'a';
				n->type[4] = 'n';
				n->type[5] = 'b';
				n->type[6] = 'e';
				n->type[7] = 'r';
				n->type[8] = 0;

				n->start = hv - len_a+i+2;
				n->end = len[hv]+n->start-1;
				
				n->note = malloc(sizeof(char)*(2));
				n->note[0] = 'w';
				n->note[1] = 0;
				/*
				n->note = malloc(sizeof(char)*(notel+1));
				for (j = 0;j < aln->lsn[a];j++){
					n->note[j] = aln->sn[a][j];
				}
				while(j < notel){
					n->note[j] = aln->sn[b][j-aln->lsn[a]];
					j++;
				}
				n->note[notel] = 0;*/

				if(! aln->ft[a]){
					 aln->ft[a] = n;
				}else{
					n->next = aln->ft[a];
					aln->ft[a] = n;
				}
				
				/*if((old_n = aln->ft[a])!= 0){
					while(old_n->next!=0){
						old_n = old_n->next;
					}
					old_n->next = n;
				}else{
					 aln->ft[a] = n;
				}*/
				n = 0;			
			
	//			k_printf("\nDiagonal found A:%d	%d\n",i+1,len[hv]);
	//			k_printf("Diagonal found	B:%d	%d\n",hv - len_a+i+2,len[hv]);
		
			}
			len[hv] = 0;
			weight[hv] = 0;
		}
	//	k_printf("%d,%d	",hv,hv - len_a+i+2);
	}
	//k_printf("\n");
	free(weight);
	free(len);
	free(added);

	//n =aln->ft[a];
	//while(n){
	//	k_printf("%s	%s	%d-%d\n",n->type,n->note,n->start,n->end);
	//	n = n->next;
	//}
	
	
	
	
	//exit(0);
	return aln;
}



float protein_wu_distance_calculation3(struct node* hash[],int* seq,int seqlen,int diagonals,int mode)
{
	struct node* node_p = 0;
	int i,c;
	unsigned int hv;
	int dlen = 0;
	
	
	int *weight = 0;
	int *len = 0;
	int* added = 0;
	
	weight = malloc(sizeof(int*)*diagonals);
	len = malloc(sizeof(int*)*diagonals);
	added = malloc(sizeof(int*)*diagonals);
	for (i = 0; i < diagonals;i++){
		weight[i] = 0;
		len[i] = 0;
		added[i] = 0;
	}

	c = 1;
	for (i =  seqlen-2;i--;){
		for (hv = 0; hv < diagonals;hv++){
			added[hv] = 0;
		}
	
		hv = (seq[i] << 5) + seq[i+1];
		//printf("3:%d\n",hv);
		if (hash[hv]){
			node_p = hash[hv];
			while(node_p){
				added[node_p->pos+c] = 1; 
				weight[node_p->pos+c]++;
	//			len[node_p->pos+c] = 1 + len[node_p->pos+c];
				node_p = node_p->next;
			}
		}


		hv = (seq[i] << 5) + seq[i+2];
		//printf("3:%d\n",hv);
		if (hash[hv]){
			node_p = hash[hv];
			while(node_p){
				added[node_p->pos+c] = 1; 
				weight[node_p->pos+c]++;
	//			len[node_p->pos+c] = 1 + len[node_p->pos+c];
				node_p = node_p->next;
			}
		}
		hv = (seq[i+1] << 5) + seq[i+2];
		//printf("3:%d\n",hv);
		if (hash[hv]){
			node_p = hash[hv];
			while(node_p){
				added[node_p->pos+c] = 1; 
				weight[node_p->pos+c]++;
	//			len[node_p->pos+c] = 1 + len[node_p->pos+c];
				node_p = node_p->next;
			}
		}
	//	k_printf("pos_a:%d	",i+1);
		
		for (hv = 0; hv < diagonals ;hv++){
			len[hv] += added[hv];
			if(!added[hv] && len[hv]){
				if (len[hv] > dlen){
					dlen = len[hv];
				}
				len[hv] = 0;
				weight[hv] = 0;
			}
	//		k_printf("%d,%d	",hv,(hv - (len_a))+i+3);
		}
	//	k_printf("\n");
		c++;
	}
	i++;
	
	//k_printf("pos_a:%d	",i);
	for (hv = 0; hv < diagonals;hv++){
		if(len[hv]){
			if (len[hv] > dlen){
				dlen = len[hv];
			}
			len[hv] = 0;
			weight[hv] = 0;
		}

	}
	free(weight);
	free(len);
	free(added);
	return dlen;
}

float** protein_wu_distance(struct alignment* si,float** dm,struct parameters* param, int nj)
{
	struct bignode* hash[1024];
	int*p =0;
	int i,j,a,b;
	unsigned int hv;
	float min;
	float cutoff;

	unsigned int numseq;
	unsigned int numprofiles;
	
	struct kalign_context *ctx = get_kalign_context();
	numseq = ctx->numseq;
	numprofiles = ctx->numprofiles;
	
	for (i = 0;i < 1024;i++){
		hash[i] = 0;
	}	

	if (nj){
		dm = malloc (sizeof(float*)*numprofiles);
		for (i = numprofiles;i--;){
			dm[i] = malloc (sizeof (float)*(numprofiles));
			for (j = numprofiles;j--;){
				dm[i][j] = 0.0f;
			}
		}
	}else{
		dm = malloc (sizeof(float*)*numseq);
		for (i = numseq;i--;){
			dm[i] = malloc (sizeof (float)*(numseq));
			for (j = numseq;j--;){
				dm[i][j] = 0.0f;
			}
		}
	}
	k_printf("Distance Calculation:\n");
	b = (numseq*(numseq-1))/2;
	a = 1;	
	
	for (i = 0; i < numseq-1;i++){
		if(check_task_canceled(ctx)) {
			break;
		}
		p = si->s[i];
				
		for (j = si->sl[i]-2;j--;){
		//for(j = 0; j < si->sl[i]-2;j++){
			//hv = (p[j+1] << 5) + p[j+2];
			//hash[hv] = big_insert_hash(hash[hv],j);
            long tmp = (p[j] << 5) + p[j+1];
            if (tmp < 0) {
                throwKalignException("Sequences are too long for alignment");
            }
			hv = tmp;
			hash[hv] = big_insert_hash(hash[hv],j);
            tmp = (p[j] << 5) + p[j+2];
            if (tmp < 0) {
                throwKalignException("Sequences are too long for alignment");
            }
			hv = tmp;
			hash[hv] = big_insert_hash(hash[hv],j);
		}
		for (j = i+1; j < numseq;j++){
			min =  (si->sl[i] > si->sl[j]) ? si->sl[j] :si->sl[i];
			cutoff = param->internal_gap_weight *min + param->zlevel;
			//cutoff = param->zlevel;
			p = si->s[j];
			dm[i][j] = protein_wu_distance_calculation(hash,p,si->sl[j],si->sl[j]+si->sl[i],cutoff);
			//k_printf("%d-%d:%f\n",i,j,dm[i][j]);
			//exit(0);
			//dm[i][j] /= min;
			//dm[i][j] /= (si->sl[i] > si->sl[j]) ? si->sl[j] :si->sl[i];
			dm[j][i] = dm[i][j];
			k_printf("Distance Calculation: %8.0f percent done",(float)a /(float)b * 100);
			set_task_progress((float)a /(float)b * 50);
			a++;
		}
		
		
		for (j = 1024;j--;){
			if (hash[j]){
				big_remove_nodes(hash[j]);
				hash[j] = 0;
			}
		}	
	}
	return dm;
}


float protein_wu_distance_calculation(struct bignode* hash[],const int* seq,const int seqlen,const int diagonals,const float mode)
{

	struct bignode* node_p;
	unsigned int* d = 0;
	unsigned int* tmp = 0;
	float out = 0.0;
	register int i,j;
	register int c;
	register int num;
	register unsigned int hv;

	d = malloc(sizeof(unsigned int)*diagonals);
	//for (i = diagonals;i--;){
	for (i = 0;i < diagonals;i++){
		d[i] = 0;
	}
	for (i = seqlen-2;i--;){
	//for(i = 0; i < seqlen-2;i++){
		/*hv = (seq[i+1] << 5) + seq[i+2];
		
		node_p = hash[hv];
		while(node_p){
			tmp = node_p->pos;
			for(j = 0;j < node_p->num;j++){
				d[tmp[j]]++;
			}
			node_p = node_p->next;
		}*/
		hv = (seq[i] << 5) + seq[i+1];
		//printf("3:%d\n",hv);
		node_p = hash[hv];
		while(node_p){
			tmp = node_p->pos;
			num = node_p->num;
			for(j = 0;j < num;j++){
				c = tmp[j];
				d[c]++;
				c++;
				d[c]++;
			}
			node_p = node_p->next;
		}
		hv = (seq[i] << 5) + seq[i+2];

		node_p = hash[hv];
					
		while(node_p){
			tmp = node_p->pos;
			num = node_p->num;
			for(j = 0;j < num;j++){
				c = tmp[j];
				d[c]++;
			}
			node_p = node_p->next;
		}		
		d++;
	}
	//exit(0);
	d -= (seqlen-2);

	for (i = diagonals;i--;){
		//d[i] /= minlen;
		
		//k_printf("%d ",d[i]);
		if(d[i] > mode){
			out += d[i];
		//	printf("%f	%d\n",d[i]/ minlen,d[i]);
		}
	}
	free(d);
	return out;
}

float** dna_distance(struct alignment* si,float** dm,struct parameters* param, int nj)
{
	struct bignode* hash[1024];
		
	int *p = 0;
	int i,j,a,b;
	unsigned int hv;

	unsigned int numseq;
	unsigned int numprofiles;

	struct kalign_context *ctx = get_kalign_context();
	numseq = ctx->numseq;
	numprofiles = ctx->numprofiles;
	
	assert(nj==0);
	
	k_printf("Distance Calculation:\n");
	
	
	for (i = 0;i < 1024;i++){
		hash[i] = 0;
	}	

	if (nj){
		dm = malloc (sizeof(float*)*numprofiles);
		for (i = numprofiles;i--;){
			dm[i] = malloc (sizeof (float)*(numprofiles));
			for (j = numprofiles;j--;){
				dm[i][j] = 0.0f;
			}
		}
	}else{
		dm = malloc (sizeof(float*)*numseq);
		for (i = numseq;i--;){
			dm[i] = malloc (sizeof (float)*(numseq));
			for (j = numseq;j--;){
				dm[i][j] = 0.0f;
			}
		}
	}

	b = (numseq*(numseq-1))/2;
	a = 1;	
	
	for (i = 0; i < numseq-1;i++){
		if(check_task_canceled(ctx)) {
			break;
		}
		p = si->s[i];
		for (j = si->sl[i]-5;j--;){
			hv = ((p[j]&3)<<8) + ((p[j+1]&3)<<6) + ((p[j+2]&3)<<4)  + ((p[j+3]&3)<<2) + (p[j+4]&3);//ABCDE
			hash[hv] = big_insert_hash(hash[hv],j);
			hv = ((p[j]&3)<<8) + ((p[j+1]&3)<<6) + ((p[j+2]&3)<<4)  + ((p[j+3]&3)<<2) + (p[j+5]&3);//ABCDF
			hash[hv] = big_insert_hash(hash[hv],j);
			hv = ((p[j]&3)<<8) + ((p[j+1]&3)<<6) + ((p[j+2]&3)<<4)  + ((p[j+4]&3)<<2) + (p[j+5]&3);//ABCEF
			hash[hv] = big_insert_hash(hash[hv],j);
			hv = ((p[j]&3)<<8) + ((p[j+1]&3)<<6) + ((p[j+3]&3)<<4)  + ((p[j+4]&3)<<2) + (p[j+5]&3);//ABDEF
			hash[hv] = big_insert_hash(hash[hv],j);
			hv = ((p[j]&3)<<8) + ((p[j+2]&3)<<6) + ((p[j+3]&3)<<4) + ((p[j+4]&3)<<2) + (p[j+5]&3);//ACDEF
			hash[hv] = big_insert_hash(hash[hv],j);
		}
		for (j = i+1; j < numseq;j++){
			if(check_task_canceled(ctx)) {
				break;
			}
		
			//min =  (si->sl[i] > si->sl[j]) ?si->sl[j] :si->sl[i];
			dm[i][j] = dna_distance_calculation(hash,si->s[j],si->sl[j],si->sl[j]+si->sl[i],param->zlevel);
			dm[i][j] /= (si->sl[i] > si->sl[j]) ?si->sl[j] :si->sl[i];
			dm[j][i] = dm[i][j];
			k_printf("Distance Calculation: %8.0f percent done",(float)a /(float)b * 100);
			set_task_progress((float)a /(float)b * 50);
			a++;
		}
		
		for (j = 1024;j--;){
			if (hash[j]){
				big_remove_nodes(hash[j]);
				hash[j] = 0;
			}
		}
	}
	return dm;
}

float dna_distance_calculation(struct bignode* hash[],int* p,int seqlen,int diagonals,float mode)
{

	struct bignode* node_p;
	float out = 0.0;
	unsigned int* tmp = 0;
    unsigned int* d = 0;
    unsigned int* d_array_start = 0;
	int i,j;
	unsigned int hv;
	kalign_context* ctx = get_kalign_context();
	
    d_array_start = d = malloc(sizeof(int)*diagonals);
    checkAllocatedMemory(d);
	for (i = 0;i < diagonals;i++){
		d[i] = 0;
    }
	for (i = seqlen-5;i--;){
		if(check_task_canceled(ctx)) {
			break;
		}
	
		hv = ((p[i]&3)<<8) + ((p[i+1]&3)<<6) + ((p[i+2]&3)<<4)  + ((p[i+3]&3)<<2) + (p[i+4]&3);//ABCDE
		if (hash[hv]){
			node_p = hash[hv];
			while(node_p){
				tmp = node_p->pos;
				for(j = 0;j < node_p->num;j++){
					d[tmp[j]]++;
				}
				node_p = node_p->next;
			}
		}	
			

		hv = ((p[i]&3)<<8) + ((p[i+1]&3)<<6) + ((p[i+2]&3)<<4)  + ((p[i+3]&3)<<2) + (p[i+5]&3);//ABCDF
		if (hash[hv]){
			node_p = hash[hv];
			while(node_p){
				tmp = node_p->pos;
				for(j = 0;j < node_p->num;j++){
					d[tmp[j]]++;
				}
				node_p = node_p->next;
			}
		}	
		hv = ((p[i]&3)<<8) + ((p[i+1]&3)<<6) + ((p[i+2]&3)<<4)  + ((p[i+4]&3)<<2) + (p[i+5]&3);//ABCEF
		if (hash[hv]){
			node_p = hash[hv];
			while(node_p){
				tmp = node_p->pos;
				for(j = 0;j < node_p->num;j++){
					d[tmp[j]]++;
				}
				node_p = node_p->next;
			}
		}	
		hv = ((p[i]&3)<<8) + ((p[i+1]&3)<<6) + ((p[i+3]&3)<<4)  + ((p[i+4]&3)<<2) + (p[i+5]&3);//ABDEF
		if (hash[hv]){
			node_p = hash[hv];
			while(node_p){
				tmp = node_p->pos;
				for(j = 0;j < node_p->num;j++){
					d[tmp[j]]++;
				}
				node_p = node_p->next;
			}
		}	
		hv = ((p[i]&3)<<8) + ((p[i+2]&3)<<6) + ((p[i+3]&3)<<4) + ((p[i+4]&3)<<2) + (p[i+5]&3);//ACDEF
		if (hash[hv]){
			node_p = hash[hv];
			while(node_p){
				tmp = node_p->pos;
				for(j = 0;j < node_p->num;j++){
					d[tmp[j]]++;
				}
				node_p = node_p->next;
			}
		}	
	
        d++;
	}
	//exit(0);
    d -= (seqlen-5);
    //for (i = seqlen-1;i--;){
    if (!check_task_canceled(ctx)) {
        for (i = diagonals;i--;){
            //d[i] /= minlen;

            //printf("%d ",d[i]);

            if(d[i] > mode){
            //k_printf("%f	%d\n",d[i]/ minlen,d[i]);
                out += d[i];
            }
        }
    }
    free(d_array_start);
	return out;
}



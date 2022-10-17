/*
	kalign2_tree.c 
	
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
#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "kalign2.h"

struct aln_tree_node* real_upgma(float **dm,int ntree)
{
	int i,j;
	int *as = 0;
	float max;
	int node_a = 0;
	int node_b = 0;
	kalign_context* ctx = get_kalign_context();
	unsigned int numseq = ctx->numseq;
	unsigned int numprofiles = ctx->numprofiles;
	int cnode = ctx->numseq;
	

	
	struct aln_tree_node** tree = 0;
	struct aln_tree_node* tmp = 0;
	
	as = malloc(sizeof(int)*numseq);
	for (i = numseq; i--;){
		as[i] = i+1;
	}
	
	tree = malloc(sizeof(struct aln_tree_node*)*numseq);
	for (i=0;i < numseq;i++){
		tree[i] = malloc(sizeof(struct aln_tree_node));
		tree[i]->done = 1;
		tree[i]->num = i;
		tree[i]->path = 0;
		tree[i]->profile = 0;
		tree[i]->seq = 0;//seq[i];
		tree[i]->len = 0;//len[i]; 
		/*
		Needs to be +2 because:
		at n = 3 is is possible to get a perfectly balanced binary tree with 4 sequences at intermediate nodes
		*/
		/*tree[i]->links = malloc(sizeof(struct aln_tree_node*)*2);
		
		for ( j =0;j < 2;j++){
			tree[i]->links[j] = 0;
		}*/
		
		tree[i]->internal_lables = malloc(sizeof(int)*(ntree+(ntree-1)));
		tree[i]->links = malloc(sizeof(struct aln_tree_node*)*(ntree+(ntree-1)));
		
		for ( j =0;j < (ntree+(ntree-1));j++){
			tree[i]->links[j] = 0;
			tree[i]->internal_lables[j] = 0;
		}
	}
	
	while (cnode != numprofiles){
		max = -INFTY;
		for (i = 0;i < numseq-1; i++){
			if (as[i]){
			for ( j = i + 1;j < numseq;j++){
				if (as[j]){
				if (dm[i][j] > max){
					max = dm[i][j];
					node_a = i;
					node_b = j;
				}
				}
			}
			}
		}
		tmp = malloc(sizeof(struct aln_tree_node));
		tmp->done = 0;
		tmp->path = 0;
		tmp->profile = 0;
		tmp->num = cnode;
		tmp->seq = 0;
		tmp->len = 0;
		
		tmp->links = malloc(sizeof(struct aln_tree_node*)*(ntree+(ntree-1)));
		tmp->internal_lables = malloc(sizeof(int)*(ntree+(ntree-1)));
		tmp->links[0] = tree[node_a];
		tmp->links[1] = tree[node_b];
		tmp->internal_lables[0] = cnode;
		tmp->internal_lables[1] = 0;
		
		for ( i =2;i < (ntree+(ntree-1));i++){
			tmp->links[i] = 0;
			tmp->internal_lables[i] = 0;
			
		}
		
		
		tree[node_a] = tmp;
		tree[node_b] = 0;
				
		/*deactivate  sequences to be joined*/
		as[node_a] = cnode+1;
		as[node_b] = 0;
		cnode++;    
		
		/*calculate new distances*/
		for (j = numseq;j--;){
			if (j != node_b){
				dm[node_a][j] = (dm[node_a][j] + dm[node_b][j])*0.5;
			}
		}
		dm[node_a][node_a] = 0.0f;
		for (j = numseq;j--;){
			dm[j][node_a] = dm[node_a][j];
			dm[j][node_b] = 0.0f;
			dm[node_b][j] = 0.0f;
		}		
	}
	tmp = tree[node_a];
	
	for (i = numseq;i--;){
		free(dm[i]);
	}
	free(dm);
	
	
	free(tree);
	free(as);
	return tmp;
}

struct aln_tree_node* real_nj(float **dm,int ntree)
{
	int i,j;
	//float **dm = 0;
	float *r = 0;
	float *r_div = 0;
	int *active = 0;
	int node = 0;
	float min = 0;
	int join_a = 0;
	int join_b = 0;
	int leaves = 0;
	
	kalign_context* ctx = get_kalign_context();
	unsigned int numseq = ctx->numseq;
	unsigned int numprofiles = ctx->numprofiles;

	struct aln_tree_node** tree = 0;
	struct aln_tree_node* tmp = 0;

	leaves = numseq;
	
	r = malloc ((numseq*2-1) *sizeof(float));
	r_div = malloc ((numseq*2-1) *sizeof(float));
	active = malloc((numseq*2-1)*sizeof(int));
	for ( i = 0;i < numseq*2-1;i++){
		active[i] = 0;
	}
	for ( i = 0;i < numseq;i++){
		active[i] = 1;
	}
	
	
	tree = malloc(sizeof(struct aln_tree_node*)*(numseq*2-1));
	for (i=0;i < numseq*2-1;i++){
		tree[i] = malloc(sizeof(struct aln_tree_node));
		tree[i]->done = 1;
		tree[i]->num = i;
		tree[i]->path = 0;
		tree[i]->profile = 0;
		tree[i]->seq = 0;//seq[i];
		tree[i]->len = 0;//len[i]; 	
		tree[i]->internal_lables = malloc(sizeof(int)*(ntree+(ntree-1)));
		tree[i]->links = malloc(sizeof(struct aln_tree_node*)*(ntree+(ntree-1)));
		
		for ( j =0;j < (ntree+(ntree-1));j++){
			tree[i]->links[j] = 0;
			tree[i]->internal_lables[j] = 0;
		}
	}
	
	node = numseq;
	while (node != numseq*2 -1){
		for (i = 0;i<numseq*2-1;i++){
			if (active[i]){
				r[i] = 0;
				for (j = 0;j < numseq*2-1;j++){
					if (active[j]){
						r[i] += (i<j) ?dm[i][j]:dm[j][i];
					}
				}
				r_div[i] = r[i] / (leaves-2);
			}
		}
		for ( j = 0;j < numseq*2-1;j++){
			if (active[j]){
			for ( i = j+1;i < numseq*2-1;i++){
				if (active[i]){
				dm[i][j] = dm[j][i] - (r[i] + r[j])/2;
				}
			}
			}
		}
		min = -INFTY;
		for ( j = 0;j < numseq*2-1;j++){
			if (active[j]){
			for ( i = j+1;i < numseq*2-1;i++){
				if (active[i]){
					if (dm[i][j] > min){
						min = dm[i][j];
						join_a = j;
						join_b = i;
					}
				}
			}
			}
		}
		//join_a always smaller than join_b && both smaller than node
		dm[join_a][node] =  dm[join_a][join_b]/2 + (r_div[join_a] - r_div[join_b])/2;
		dm[join_b][node] =  dm[join_a][join_b] - dm[join_a][node];
		
		tree[node]->num = node;
		tree[node]->links[0] = tree[join_a];
		tree[node]->links[1] = tree[join_b];
		tree[node]->internal_lables[0] = node;
		tree[node]->internal_lables[1] = 0;


		active[join_a] = 0;
		active[join_b] = 0;
		
		for (i = 0;i<numseq*2-1;i++){
			if (active[i]){
				dm[i][node] = (i>join_a) ? dm[join_a][i]: dm[i][join_a];
				dm[i][node] -= dm[join_a][node];
				dm[i][node] += (i > join_b) ? dm[join_b][i] : dm[i][join_b] ;
				dm[i][node] -= dm[join_b][node];
				dm[i][node] /= 2;
			}
		}
		active[node] = 1;
		node++;
	}

	for (i = numprofiles;i--;){
		free(dm[i]);
	}
	free(dm);

	free(r);
	free(r_div);
	free(active);
	tmp = tree[node-1];
	free(tree);
	return tmp;
}

struct ntree_data* alignntree(struct ntree_data* ntree_data,struct aln_tree_node* p)
{
	int i = 0;
	int ntree = ntree_data->ntree;
	int* leaves = 0;
	
	leaves = malloc(sizeof(int)* (ntree+(ntree-1)));

	while(p->links[i]){
		alignntree(ntree_data,p->links[i]);
		i++;	
	}
	i = 0;
	if (p->links[i]){
		k_printf("Aligning subtree: at node:%d\n",p->num);
		while(p->links[i]){
			leaves[i] = p->links[i]->num;
			i++;
		}
		leaves[i] = -1;
	//	k_printf("NODES:%d\n",i);
		ntree_data =  find_best_topology(ntree_data,leaves,p->internal_lables);
	//	exit(0);
	}
	free(leaves);
	
	return ntree_data;
}


void print_simple_phylip_tree(struct aln_tree_node* p)
{
	if(p->links[0]){
	
		k_printf("(");
		print_simple_phylip_tree(p->links[0]);
	}
	if(p->num < get_kalign_context()->numseq){
		k_printf("%d",p->num);
	}else{
		k_printf(",");
	}
	if(p->links[1]){
		print_simple_phylip_tree(p->links[1]);
		k_printf(")");
	}
}


void printtree(struct aln_tree_node* p)
{
	int i = 0;

	while(p->links[i]){
		printtree(p->links[i]);
		i++;	
	}
	i = 0;
	if (p->links[i]){
		printf("Aligning: at node:%d\n",p->num);
		while(p->links[i]){
			printf("%d\n",p->links[i]->num);
			i++;
		}
		i = 0;
		while(p->internal_lables[i]){
			printf("%d ",p->internal_lables[i]);
			i++;
		}
		printf("\n");
	}
}

void ntreeify(struct aln_tree_node* p,int ntree)
{
	int i = 0;
	int c = 0;
	struct aln_tree_node* tmp1 = 0;
	struct aln_tree_node* tmp2 = 0;
	if (p->links[0]){
		ntreeify(p->links[0],ntree);
	}
	if (p->links[1]){
		ntreeify(p->links[1],ntree);
	}
	
	if (!p->done){
		tmp1 = p->links[0];
		tmp2 = p->links[1];
		
		p->done = tmp1->done + tmp2->done;
		i = 0;
		c = 0;
		if(tmp1->done != 1){

			while(tmp1->internal_lables[i]){
				p->internal_lables[c] = tmp1->internal_lables[i];
				i++;
				c++;
			}
			if(tmp2->done != 1){
				i = 0;
				while(tmp2->internal_lables[i]){
					p->internal_lables[c] = tmp2->internal_lables[i];
					c++;
					i++;
				}
			}
		}else if(tmp2->done != 1){
			i = 0;
			while(tmp2->internal_lables[i]){
				p->internal_lables[c] = tmp2->internal_lables[i];
				c++;
				i++;
			}
		}
		p->internal_lables[c] = p->num;
		
		//k_printf("%d:%d	%d:%d		%d\n",tmp1->num,tmp1->internal_lables[0],tmp2->num,tmp2->internal_lables[0],p->num);
		/*for (i = 0; i< c;i++){
			k_printf("il:%d ",p->internal_lables[i]);
		}
		k_printf("\n");*/
	
		
		if (tmp1->done > 1){
			for ( i = 0;i < tmp1->done;i++){
				p->links[i] = tmp1->links[i];
				tmp1->links[i] = 0;				
			}
		}
		
		if (tmp2->done > 1){
			for ( i = 0; i < tmp2->done;i++){
				p->links[tmp1->done+i] = tmp2->links[i];
				tmp2->links[i] = 0;
			}
			free(tmp2->internal_lables);
			free(tmp2->links);
			free(tmp2);
		}else{
			p->links[tmp1->done] = tmp2;
		}
	//	k_printf("p->num:%d\n",p->num);
		p->links[p->done] = 0;
		
		if (tmp1->done > 1){
			free(tmp1->internal_lables);
			free(tmp1->links);
			free(tmp1);
		}
		
		if (p->done >= ntree){ 
			p->done = 1;
			/*i = 0;
			while(p->internal_lables[i]){
				i++;
			}
			p->internal_lables[i] = p->num;*/
		}
	}
}

struct ntree_data* find_best_topology(struct ntree_data* ntree_data,int* leaves,int* nodes)
{
	int i,c;
	int elements = 0;
	//int num_topologies =0;
	int* milometer = 0; //DURBIN
	struct tree_node* tree = 0;
	struct tree_node* tmp = 0;
	int newnode = 0;
	int local_ntree = 0;
	
	int *tmp_tree = 0;

	while(leaves[local_ntree] != -1){
		local_ntree++;
	}
	//k_printf("REALKDASF KJAF SA:%d\n",local_ntree);

	//for (i = 0; i < local_ntree-1;i++){
	//	k_printf("nodes:%d\n",nodes[i]);
	//}

    assert(local_ntree>0);
	tmp_tree = malloc(sizeof(int)*(local_ntree+local_ntree-1)*3);
	for (c = 0; c < (local_ntree+local_ntree-1)*3;c++){
		tmp_tree[c] = 0;
	}
	
	tmp_tree[0] =1;
	
	
	if (local_ntree < 3){
		//printf("ORDER1: %d	and	%d\n",leaves[0],leaves[1]);
		tmp_tree[0] =1;
	
		tmp = malloc(sizeof(struct tree_node));
		tmp->left = 0;
		tmp->right = 0;
		tmp->label = -1;
		tmp->edge = 0;
	
		tmp->left = malloc(sizeof(struct tree_node));
		tmp->left->left = 0;
		tmp->left->right = 0;
		tmp->left->edge = 1;
		tmp->left->label = leaves[0];
		tmp->right = malloc(sizeof(struct tree_node));
		tmp->right->left = 0;
		tmp->right->right = 0;
		tmp->right->edge = 2;
		tmp->right->label = leaves[1];
		tree = malloc(sizeof(struct tree_node));
		tree->left =tmp;
		tree->right = 0;
		tree->edge = -1;
		tree->label = -1;

		c =  add_label_simpletree(tree,nodes,0);
		readsimpletree(tree,tmp_tree);
		/*for (c = 1; c < tmp_tree[0];c++){
			k_printf("%d ",tmp_tree[c]);
		}
		k_printf("\n\n");*/
  		ntree_data =ntree_sub_alignment(ntree_data,tmp_tree,local_ntree);
		free(tmp_tree);
		
	}else{
		elements = local_ntree-2;
		milometer = malloc(sizeof(int)*(elements));
		for ( i = 0; i < elements;i++){
			milometer[i] = 0;
		}

		i = 0;
		while(milometer[0] != -1){
	
			tmp_tree[0] =1;
	
			tmp = malloc(sizeof(struct tree_node));
			tmp->left = 0;
			tmp->right = 0;
			tmp->label = -1;
			tmp->edge = 0;
	
			tmp->left = malloc(sizeof(struct tree_node));
			tmp->left->left = 0;
			tmp->left->right = 0;
			tmp->left->edge = 1;
			tmp->left->label = leaves[0];
			tmp->right = malloc(sizeof(struct tree_node));
			tmp->right->left = 0;
			tmp->right->right = 0;
			tmp->right->edge = 2;
			tmp->right->label = leaves[1];
			tree = malloc(sizeof(struct tree_node));
			tree->left =tmp;
			tree->right = 0;
			tree->edge = -1;
			tree->label = -1;
		
			//printsimpleTree(tree);
			//tree = simpleinsert(tree,0,3,-3);
			//k_printf("\n\n");
			//printsimpleTree(tree);
			newnode = 3;
			for(c = 0; c < elements;c++){			
			//	printf("%d ",milometer[c]);
				tree = simpleinsert(tree,milometer[c],newnode,leaves[2+c]);
				newnode+=2;
			} 
			k_printf("Topology:%d	",i);
			//printsimpleTree(tree);
			c = add_label_simpletree(tree,nodes,0);
			
			readsimpletree(tree,tmp_tree);
			freesimpletree(tree);
			/*for (c = 1; c < tmp_tree[0];c++){
				k_printf("%d ",tmp_tree[c]);
			}
			k_printf("\n\n");*/
			ntree_data =ntree_sub_alignment(ntree_data,tmp_tree,local_ntree);
			
			//exit(0);
			//for (c = 0;c < ntree -1;c++){
			//	k_printf("%d ",nodes[c]);
			//}
			//k_printf("\n\n");
			i++;
			milometer = ticker(milometer,elements);
		}
	
		free(milometer);
		free(tmp_tree);
	}
	return ntree_data;
}

int add_label_simpletree(struct tree_node* p,int* nodes,int i)
{
	if(p->left){
		i = add_label_simpletree(p->left,nodes,i);
	}
	if(p->right){
		i = add_label_simpletree(p->right,nodes,i);
	}
	if(p->left){
		if(p->right){
			p->label = nodes[i];
			i++;
			return i;
		}
	}
	return i;
}

int* readsimpletree(struct tree_node* p,int* tree)
{
	if(p->left){
		tree = readsimpletree(p->left,tree);
	}
	if(p->right){
		tree = readsimpletree(p->right,tree);
	}
	if(p->left){
		if(p->right){
			tree[tree[0]] = p->left->label;
			tree[tree[0]+1] = p->right->label;
			tree[tree[0]+2] = p->label;
			tree[0] +=3;
	//		free(p->left);
	//		free(p->right);
	//	}else{
	//		free(p->left);
		}
	}//else{
	//	free(p->right);
	//}
	return tree;
}

void printsimpleTree(struct tree_node* p)
{
	if(p->left){
	printsimpleTree(p->left);
	}
	//k_printf("%d\n",p->label);
	if(p->right){
	printsimpleTree(p->right);
	}
	if(p->left){
		if(p->right){
			k_printf("%d %d -> %d\n",p->left->label,p->right->label,p->label);
			free(p->left);
			free(p->right);
		}else{
			free(p->left);
		}
	}else{
		free(p->right);
	}
	
//	k_printf("Edge:%d	Label:%d\n",p->edge,p->label);
}


struct tree_node* simpleinsert(struct tree_node* p,int target, int new_edge,int leaf_label)
{
	struct tree_node* tmp = 0;
	struct tree_node* tmp2 = 0;

	if(p->left){
		if(p->left->edge == target){
			tmp = malloc(sizeof(struct tree_node));
			tmp->left = 0;
			tmp->right = 0;
			tmp->label = leaf_label;
			tmp->edge = new_edge+1;
			
			tmp2 = malloc(sizeof(struct tree_node));
			tmp2->left = tmp;
			tmp2->right = p->left;
			tmp2->label = -1;
			tmp2->edge = p->left->edge;

			p->left->edge = new_edge;

			p->left = tmp2;


			return p;
		}else{
			p->left = simpleinsert(p->left,target,new_edge,leaf_label);
		}
	}

	if(p->right){
		if(p->right->edge == target){
			tmp = malloc(sizeof(struct tree_node));
			tmp->left = 0;
			tmp->right = 0;
			tmp->label = leaf_label;
			tmp->edge = new_edge+1;
			
			tmp2 = malloc(sizeof(struct tree_node));
			tmp2->left = tmp;
			tmp2->right = p->right;
			tmp2->label = -1;
			tmp2->edge = p->right->edge;

			p->right->edge = new_edge;

			p->right = tmp2;


			return p;
		}else{
			p->right = simpleinsert(p->right,target,new_edge,leaf_label);
		}
	}
	return p;
}


int* ticker(int* milometer,int elements)
{
	while(elements){
		if (milometer[elements-1] < (2*elements)){
			milometer[elements-1]++;
			return milometer;
		}else{
			milometer[elements-1] = 0;
			elements--;
		}
	}
	milometer[0] = -1;
	return milometer;
}




int* upgma(float **dm,int* tree)
{
	int i,j,t;
	int *as = 0;
	float max;
	int node_a = 0;
	int node_b = 0;
	kalign_context* ctx = get_kalign_context();
	unsigned int numseq = ctx->numseq;
	unsigned int numprofiles = ctx->numprofiles;
	int cnode = numseq;

	as = malloc(sizeof(int)*numseq);
	for (i = numseq; i--;){
		as[i] = i+1;
	}

	
	t = 0;
	while (cnode != numprofiles){
		max = -INFTY;
		for (i = 0;i < numseq-1; i++){
			if (as[i]){
			for ( j = i + 1;j < numseq;j++){
				if (as[j]){
				if (dm[i][j] > max){
					max = dm[i][j];
					node_a = i;
					node_b = j;
				}
				}
			}
			}
		}
		
		tree[t] = as[node_a]-1;
		tree[t+1] = as[node_b]-1;
		tree[t+2] = cnode;
		t += 3;	
		
		/*deactivate  sequences to be joined*/
		as[node_a] = cnode+1;
		as[node_b] = 0;
		cnode++;    
		
		/*calculate new distances*/
		for (j = numseq;j--;){
			if (j != node_b){
				dm[node_a][j] = (dm[node_a][j] + dm[node_b][j])/2;
			}
		}
		dm[node_a][node_a] = 0.0f;
		for (j = numseq;j--;){
			dm[j][node_a] = dm[node_a][j];
			dm[j][node_b] = 0.0f;
			dm[node_b][j] = 0.0f;
		}		
	}
	free(as);
	return tree;
}



int* nj(float **dm,int* tree)
{
	int i,j;
	//float **dm = 0;
	float *r = 0;
	float *r_div = 0;
	int *active = 0;
	int node = 0;
	float min = 0;
	int join_a = 0;
	int join_b = 0;
	int leaves = 0;
	int c =0;

	kalign_context* ctx = get_kalign_context();
	unsigned int numseq = ctx->numseq;
	unsigned int numprofiles = ctx->numprofiles;

	leaves = numseq;
	
	r = malloc ((numseq*2-1) *sizeof(float));
	r_div = malloc ((numseq*2-1) *sizeof(float));
	active = malloc((numseq*2-1)*sizeof(int));
	for ( i = 0;i < numseq*2-1;i++){
		active[i] = 0;
	}
	for ( i = 0;i < numseq;i++){
		active[i] = 1;
	}
	
	node = numseq;
	while (node != numseq*2 -1){
		for (i = 0;i<numseq*2-1;i++){
			if (active[i]){
				r[i] = 0;
				for (j = 0;j < numseq*2-1;j++){
					if (active[j]){
						r[i] += (i<j) ?dm[i][j]:dm[j][i];
					}
				}
				r_div[i] = r[i] / (leaves-2);
			}
		}
		for ( j = 0;j < numseq*2-1;j++){
			if (active[j]){
			for ( i = j+1;i < numseq*2-1;i++){
				if (active[i]){
				dm[i][j] = dm[j][i] - (r[i] + r[j])/2;
				}
			}
			}
		}
		min = -INFTY;
		for ( j = 0;j < numseq*2-1;j++){
			if (active[j]){
			for ( i = j+1;i < numseq*2-1;i++){
				if (active[i]){
					if (dm[i][j] > min){
						min = dm[i][j];
						join_a = j;
						join_b = i;
					}
				}
			}
			}
		}
		//join_a always smaller than join_b && both smaller than node
		dm[join_a][node] =  dm[join_a][join_b]/2 + (r_div[join_a] - r_div[join_b])/2;
		dm[join_b][node] =  dm[join_a][join_b] - dm[join_a][node];

		active[join_a] = 0;
		active[join_b] = 0;
		tree[c] = join_a;
		tree[c+1] = join_b;
		tree[c+2] = node;

		for (i = 0;i<numseq*2-1;i++){
			if (active[i]){
				dm[i][node] = (i>join_a) ? dm[join_a][i]: dm[i][join_a];
				dm[i][node] -= dm[join_a][node];
				dm[i][node] += (i > join_b) ? dm[join_b][i] : dm[i][join_b] ;
				dm[i][node] -= dm[join_b][node];
				dm[i][node] /= 2;
			}
		}
		active[node] = 1;
		c += 3;
		node++;

	}


	for (i = numprofiles;i--;){
		free(dm[i]);
	}
	free(dm);

	free(r);
	free(r_div);
	free(active);

	return tree;
}



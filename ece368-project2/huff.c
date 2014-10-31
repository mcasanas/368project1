#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "huff.h"

int countCharacters(char * filename, CharNode * node)
{
  FILE * pt = fopen(filename, "r");
  int count = 0;
  if(pt == NULL) return 0;
  
  while (!feof(pt)) {
    int letter = fgetc(pt);
    if(letter != EOF){
      count++;
      node[letter].character = (char) letter;
      node[letter].weight++;
    }
  }
  fclose(pt);
  return 1;
}

static int compareCharacter(const void *p1, const void *p2)
{
  const CharNode * ip1 = (const CharNode *) p1;
  const CharNode * ip2 = (const CharNode *) p2;

  const int iv1 = ip1 -> weight;
  const int iv2 = ip2 -> weight;
  return (iv1 - iv2);
}

void sortCharacters(CharNode * node)
{
  qsort(node, 128, sizeof(CharNode), compareCharacter);
}

TreeNode * TreeNodeCreate(char character, int weight)
{
  TreeNode * tn = malloc(sizeof(TreeNode));
  tn -> left = NULL;
  tn -> right = NULL;
  tn -> value = character;
  tn -> freq = weight;
  return tn;
}

TreeNode * TreeMerge(TreeNode * tree1, TreeNode * tree2)
{
  TreeNode * tn = malloc(sizeof(TreeNode));
  tn -> left = tree1;
  tn -> right = tree2;
  tn -> value = 0;
  tn -> freq = ((tree1 -> freq) + (tree2 -> freq));
  return tn;
}

ListNode * ListNodeCreate(TreeNode * tree)
{
  ListNode * ln = malloc(sizeof(ListNode));
  ln -> next = NULL;
  ln -> tnptr = tree;
  return ln;
}

ListNode * ListInsert(ListNode * head, ListNode * ln)
{
  if (head == NULL) return ln;
  //if (ln == NULL) return head;
  //if ((ln -> next) != NULL) return head;

  int freq1 = (head -> tnptr) -> freq;
  int freq2 = (ln -> tnptr) -> freq;
  if(freq1 > freq2) {
    ln -> next = head;
    return ln;
  }
  head -> next = ListInsert(head -> next, ln);
  return head;
}

ListNode * ListBuild(CharNode * frequency)
{
  int ind = 0;
  while(frequency[ind].weight == 0)
    {
      ind++;
    }
  if(ind == 128) return NULL;
  
  ListNode * head = NULL;
  while(ind < 128){
    TreeNode * tn = TreeNodeCreate(frequency[ind].character, frequency[ind].weight);
    ListNode * ln = ListNodeCreate(tn);
    head = ListInsert(head, ln);
    ind++;
  }
  return head;
}



static int findHeight(TreeNode * tn, int height)
{
  if(tn == 0) return height;
  int lh = findHeight(tn -> left, height+1);
  int rh = findHeight(tn -> right, height+1);
  if(lh < rh) return rh;
  if(lh > rh) return lh;
}

int Tree_height(TreeNode * tn)
{
  return findHeight(tn, 0);
}

static void Tree_leafHelper(TreeNode * tn, int * num)
{
  if(tn == 0) return;
  
  TreeNode * lc = tn -> left;
  TreeNode * rc = tn -> right;
  if ((lc == NULL)&&(rc == NULL)){
    (*num)++;
    return;
  }
  
  Tree_leafHelper(lc, num);
  Tree_leafHelper(rc, num);
}

int Tree_leaf(TreeNode * tn)
{
  int num = 0;
  Tree_leafHelper(tn, & num);
  return num;
}

void buildCodeBookHelper(TreeNode * tn, int ** codebook, int * row, int col)
{
  if(tn == NULL) return;
  
  TreeNode * lc = tn -> left;
  TreeNode * rc = tn -> right;
  if ((lc == NULL)&&(rc == NULL)){
    codebook[*row][0] = tn -> value;
    (*row)++;
    return;
  }
  
  if(lc != NULL){
    int numRow = Tree_leaf(lc);
    int ind;
    for(ind = (*row); ind < (*row) + numRow; ind++){
      codebook[ind][col] = 0;
    }
    buildCodeBookHelper(lc, codebook, row, col + 1);
  }
  
  if(rc != NULL){
    int numRow = Tree_leaf(rc);
    int ind;
    for(ind = (*row); ind < (*row) + numRow; ind++){
      codebook[ind][col] = 1;
    }
    buildCodeBookHelper(rc, codebook, row, col + 1);
  }
}

void buildCodeBook(TreeNode * root, int ** codebook)
{
  int row = 0;
  buildCodeBookHelper(root, codebook, & row, 1);
}

static ListNode * MergeListNode(ListNode * head)
{
  ListNode * second = head -> next;
  ListNode * third = second -> next;
  TreeNode * tn1 = head -> tnptr;
  TreeNode * tn2 = second -> tnptr;
  free(head);
  free(second);
  head = third;
  TreeNode * mrg;
  mrg = TreeMerge(tn1, tn2);
  ListNode * ln = ListNodeCreate(mrg);
  head = ListInsert(head, ln);
  return head;
}

static TreeNode * list2Tree(ListNode * head)
{
  while ((head -> next) != NULL){
    head = MergeListNode(head);
  }
  TreeNode * root = head -> tnptr;
  free(head);
  return root;
}

int encode(char * infile)
{
  CharNode frequencies[128];
  
  bzero(frequencies, sizeof(CharNode) * 128);
  if(countCharacters(infile, frequencies) == 0) return 0;
  
  unsigned int numChar = countCharacters(infile, frequencies);
  if(numChar == 0) return 0;

  sortCharacters(frequencies);

  ListNode * head = ListBuild(frequencies);
  if(head == NULL) return 0;

  /*while((head -> next) != NULL)
    {
      ListNode * second = head -> next;
      ListNode * third = second -> next;
      TreeNode * tree1 = head -> tnptr;
      TreeNode * tree2 = second -> tnptr;
      free(head);
      free(second);
      head = third;
      TreeNode * merge = TreeMerge(tree1, tree2);
      ListNode * ln = ListNodeCreate(merge);
      }*/

  TreeNode * root = list2Tree(head);
  free(head);
  
  int numRow = Tree_leaf(root);
  int numCol = Tree_height(root);
  numCol++;
  
  int ** codebook = malloc(sizeof(int*)*numRow);
  int row;
  for(row = 0; row < numRow; row++){
    codebook[row] = malloc(sizeof(int) * numCol);
    int col;
    for(col = 0; col < numCol; col++){
      codebook[row][col] = -1;
    }
  }
  buildCodeBook(root, codebook);
  
  return 1;
}


int main(int argc, char ** argv)
{
  if(argc != 2) return EXIT_FAILURE;
  encode(argv[1]);
  return EXIT_SUCCESS;
}

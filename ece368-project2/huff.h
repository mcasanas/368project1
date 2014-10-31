#include <stdio.h>
#include <stdlib.h>


//Node for individual character
typedef struct
{
  char character;
  int weight;
} CharNode;

int countCharacters(char * filename, CharNode * node);

void sortCharacters(CharNode * node);


//TreeNode for character

typedef struct treenode
{
  struct treenode * left;
  struct treenode * right;
  char value;
  int freq;
} TreeNode;

TreeNode * TreeNodeCreate(char character, int weight);
TreeNode * TreeMerge(TreeNode * tree1, TreeNode * tree2);

//List for separated characters

typedef struct listnode
{
  struct listnode * next;
  TreeNode * tnptr;  //treenode pointer
}ListNode;

ListNode * ListBuild(CharNode * frequency);
ListNode * ListNodeCreate(TreeNode * tree);
ListNode * ListInsert(ListNode * head, ListNode * ln);

int encode(char * infile);

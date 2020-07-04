#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
static double elapsed_time(void)
{
    static struct timespec last_time, current_time;
    last_time = current_time;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &current_time) != 0)
        return -1.0; // clock_gettime() failed!!!
    return ((double)current_time.tv_sec - (double)last_time.tv_sec) + 1.0e-9 * ((double)current_time.tv_nsec - (double)last_time.tv_nsec);
}

///////////////////HASH-FUNCTION////////////////////

unsigned int hash_function(const char *str, unsigned int s)
{
    unsigned int h;
    for (h = 0u; *str != '\0'; str++)
        h = 157u * h + (0xFFu & (unsigned int)*str); // arithmetic overflow may occur here (just ignore it!)
    return h % s;                                    // due to the unsigned int data type, it is guaranteed that 0 <= h % s < s
}

/////////////////ORDERED-BINARY-TREE///////////////

typedef struct tree_node
{
    char word[64];
    long last_pos;
    long count;
    long minDst;
    long maxDst;
    long totalDst;
    struct tree_node *right;
    struct tree_node *left;
} tree_node;

static tree_node *newTreeWord(int pos, char *word)
{
    tree_node *t_node = (tree_node *)calloc(1, sizeof(tree_node));
    if (t_node == NULL)
    {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    strcpy(t_node->word, word);
    t_node->count = 1;
    t_node->last_pos = pos;
    return t_node;
}

void updTreenode(int pos, tree_node *tmp)
{
    tmp->count++;
    int dst = pos - tmp->last_pos;
    if (tmp->count == 2)
        tmp->minDst = dst;
    if (dst < tmp->minDst)
    {
        tmp->minDst = dst;
    }
    if (dst > tmp->maxDst)
    {
        tmp->maxDst = dst;
    }
    tmp->totalDst += dst;
    tmp->last_pos = pos;
}

static tree_node *insert(tree_node *head, int pos, char *word)
{
    if (head == NULL)
    {
        head = newTreeWord(pos, word);
    }
    else if (strcmp(head->word, word) < 0)
    {
        head->left = insert(head->left, pos, word);
    }
    else if (strcmp(head->word, word) > 0)
    {
        head->right = insert(head->right, pos, word);
    }
    else
    {
        updTreenode(pos, head);
    }
    return head;
}

static tree_node *insert_Resize(tree_node *head, tree_node *to_put)
{
    if (head == NULL)
    {
        head = to_put;
    }
    else if (strcmp(head->word, to_put->word) < 0)
    {
        head->left = insert_Resize(head->left, to_put);
    }
    else if (strcmp(head->word, to_put->word) > 0)
    {
        head->right = insert_Resize(head->right, to_put);
    }
    return head;
}

static void travel_Tree(tree_node **new_table, tree_node *tmp, int hash_size)
{
    if (tmp == NULL)
        return;
    travel_Tree(new_table, tmp->left, hash_size);
    travel_Tree(new_table, tmp->right, hash_size);
    int new_hash = hash_function(tmp->word, (hash_size * 1.5));
    tree_node *head = new_table[new_hash];
    tmp->left = NULL;
    tmp->right = NULL;
    if (head == NULL)
    {
        new_table[new_hash] = tmp;
    }
    else
    {
        insert_Resize(head, tmp);
    }
}

static int count2 = 0;
static void printTree(tree_node *head)
{
    if (head != NULL)
    {
        printTree(head->left);
        printf("%-20s Count: %-8ld FirstP: %-13ld LastP: %-13ld MaxD: %-13ld MinD: %-13ld TotalD: %-13ld AveD: %-ld\n", head->word, head->count, head->last_pos - head->totalDst, head->last_pos, head->maxDst, head->minDst, head->totalDst, head->totalDst / head->count);
        count2++;
        printTree(head->right);
    }
}

////////////////////LINKED-LIST////////////////////

typedef struct node
{
    char word[64];
    long last_pos;
    long count;
    long minDst;
    long maxDst;
    long totalDst;
    struct node *next;
} node;

static node *newLinkedWord(int pos, char *word)
{
    node *n = (node *)malloc(sizeof(node));
    memset(n, 0, sizeof(node));
    if (n == NULL)
    {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    strcpy(n->word, word);
    n->count = 1;
    n->last_pos = pos;
    return n;
}

void updNode(int pos, node *tmp)
{
    tmp->count++;
    int dst = pos - tmp->last_pos;
    if (tmp->count == 2)
        tmp->minDst = dst;
    if (dst < tmp->minDst)
    {
        tmp->minDst = dst;
    }
    if (dst > tmp->maxDst)
    {
        tmp->maxDst = dst;
    }
    tmp->totalDst += dst;
    tmp->last_pos = pos;
}

static int count1 = 0;
static void printLinkedList(node *tmp)
{
    while (tmp != NULL)
    {
        count1++;
        printf("%-20s Count: %-8ld FirstP: %-13ld LastP: %-13ld MaxD: %-13ld MinD: %-13ld TotalD: %-13ld AveD: %-ld\n", tmp->word, tmp->count, tmp->last_pos - tmp->totalDst, tmp->last_pos, tmp->maxDst, tmp->minDst, tmp->totalDst, tmp->totalDst / tmp->count);
        tmp = tmp->next;
    }
    return;
}

/////////////////////READ-FILE/////////////////////

typedef struct file_data
{                  // public data
    long word_pos; // zero-based
    long word_num; // zero-based
    char word[64];
    // private data
    FILE *fp;
    long current_pos; // zero-based
} file_data;

int isalpha(int c);
int tolower(int c);
int open_text_file(char *file_name, file_data *fd)
{
    fd->fp = fopen(file_name, "rb");
    if (fd->fp == NULL)
        return -1;
    fd->word_pos = -1;
    fd->word_num = -1;
    fd->word[0] = '\0';
    fd->current_pos = -1;
    return 0;
}

void close_text_file(file_data *fd)
{
    fclose(fd->fp);
    fd->fp = NULL;
}

int read_word(file_data *fd)
{
    int i, c;
    // skip white spaces
    do
    {
        c = fgetc(fd->fp);
        if (c == EOF)
            return -1;
        fd->current_pos++;
    } while (!isalpha(c));
    // record word
    fd->word_pos = fd->current_pos;
    fd->word_num++;
    fd->word[0] = tolower(c);
    for (i = 1; i < (int)sizeof(fd->word) - 1; i++)
    {
        c = fgetc(fd->fp);
        if (c == EOF)
            break; // end of file
        fd->current_pos++;
        if (!isalpha(c))
            break; // terminate word
        fd->word[i] = tolower(c);
    }
    fd->word[i] = '\0';
    return 0;
}

/////////////////GLOBAL-VARIABLES///////////////////

static node **hash_table_link;
static int hash_size_link = 1000u;
static tree_node **hash_table_tree;
static int hash_size_tree = 1000u;

/////////RESIZE-HASH-TABLE-WITH-LINKED-LISTS////////

void resize_LinkedList()
{
    node **new_table = (node **)calloc(hash_size_link * 1.5, sizeof(node *));
    for (int i = 0; i < hash_size_link; i++)
    {
        if (hash_table_link[i] != NULL)
        {
            node *tmp = hash_table_link[i];
            while (tmp != NULL)
            {
                node *tmp2 = tmp->next;
                int new_hash = hash_function(tmp->word, (hash_size_link * 1.5));
                tmp->next = new_table[new_hash];
                new_table[new_hash] = tmp;
                tmp = tmp2;
            }
        }
    }
    hash_size_link = hash_size_link * 1.5;

    hash_table_link = (node **)realloc(hash_table_link, hash_size_link * sizeof(node *));
    memset(hash_table_link, 0, hash_size_link * sizeof(node *));
    for (int i = 0; i < hash_size_link; i++)
    {
        hash_table_link[i] = new_table[i];
    }
    free(new_table);
}

/////RESIZE-HASH-TABLE-WITH-ORDERED-BINARY-TREES////

void resize_OBTree()
{
    tree_node **new_table = (tree_node **)calloc(hash_size_tree * 1.5, sizeof(tree_node *));
    for (int i = 0; i < hash_size_tree; i++)
    {
        tree_node *head = hash_table_tree[i];
        travel_Tree(new_table, head, hash_size_tree);
    }
    hash_size_tree = hash_size_tree * 1.5;
    hash_table_tree = (tree_node **)realloc(hash_table_tree, hash_size_tree * sizeof(tree_node *));
    memset(hash_table_tree, 0, hash_size_tree * sizeof(tree_node *));
    for (int i = 0; i < hash_size_tree; i++)
    {
        hash_table_tree[i] = new_table[i];
    }
    free(new_table);
}


///////////////////MAIN-FUNCTION////////////////////

int main(int argc, char **argv)
{
    (void)elapsed_time();

    //////////Hash-Table-With-Linked-Lists//////////////

    file_data *fd = (file_data *)malloc(sizeof(file_data));
    memset(fd, 0, sizeof(file_data));
    if (open_text_file("SherlockHolmes.txt", fd) == -1)
    {
        perror("Ficheiro inexistente");
        exit(1);
    }
    hash_table_link = (node **)calloc(hash_size_link, sizeof(node *));
    while (read_word(fd) == 0)
    {
        if (fd->word_num > hash_size_link * 2)
        {
            // Code to See How Hash Grows Dinnamicly

            printf("Size Before Resizing: %d\n", hash_size_link);
            resize_LinkedList();
            printf("Size After Resizing: %d\n", hash_size_link);
        }
        int hashVal = hash_function(fd->word, hash_size_link);
        node *tmp = hash_table_link[hashVal];
        bool found_flag = false;
        while (tmp != NULL)
        {
            if (strcmp(tmp->word, fd->word) == 0)
            {
                found_flag = true;
                updNode(fd->word_pos, tmp);
                break;
            }
            tmp = tmp->next;
        }
        if (found_flag == false)
        {
            tmp = newLinkedWord(fd->word_pos, fd->word);
            tmp->next = hash_table_link[hashVal];
            hash_table_link[hashVal] = tmp;
        }
    }

    // Code to Print Hash Table With Linked Lists

    for (int i = 0; i < hash_size_link; i++)
    {
        node *tmp = hash_table_link[i];
        if (tmp != NULL)
        {
            printf("New Linked List:\n");
            printLinkedList(tmp);
            printf("\n");
        }
    }
    double cpu_time1 = elapsed_time();

    ////////Hash-Table-With-Ordered-Binary-Trees////////

    (void)elapsed_time();
    file_data *fd2 = (file_data *)malloc(sizeof(file_data));
    memset(fd2, 0, sizeof(file_data));
    if (open_text_file("SherlockHolmes.txt", fd2) == -1)
    {
        perror("Ficheiro inexistente");
        exit(1);
    }
    hash_table_tree = (tree_node **)calloc(hash_size_tree, sizeof(tree_node *));

    while (read_word(fd2) == 0)
    {
        if (fd2->word_num > hash_size_tree * 3)
        {
            printf("Size Before Resizing: %d\n", hash_size_tree);
            resize_OBTree();
            printf("Size After Resizing: %d\n", hash_size_tree);
        }
        int hashVal = hash_function(fd2->word, hash_size_tree);
        if (hash_table_tree[hashVal] == NULL)
        {
            hash_table_tree[hashVal] = newTreeWord(fd2->word_pos, fd2->word);
        }
        else
        {
            tree_node *head = hash_table_tree[hashVal];
            insert(head, fd2->word_pos, fd2->word);
        }
    }

    // Code to Print Hash Table With Ordered Binary Trees
    for (int i = 0; i < hash_size_tree; i++)
    {
        tree_node *head = hash_table_tree[i];
        if (head != NULL)
        {
            printf("New Ordered Binary Tree:\n");
            printTree(head);
            printf("\n");
        }
    }
    double cpu_time2 = elapsed_time();
    printf("Time to implement hash table using linked lists: %f\n", cpu_time1);
    printf("Time to implement hash table using ordered binary trees: %f\n", cpu_time2);
    printf("Number of different elements using linked lists: %d\n", count1);
    printf("Number of different elements using ordered binary trees: %d\n", count2);
    return 0;
}
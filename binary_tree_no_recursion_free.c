
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct node
{
    int data;
    struct node * left;
    struct node * right;
};

struct context
{
    int memory_used;
};

struct node * node_alloc(struct context * ctx)
{
    ctx->memory_used += 1;
    return (struct node *)malloc(sizeof(struct node));
}

void node_free(struct context * ctx, struct node * p)
{
    ctx->memory_used -= 1;
    free(p);
}

int add_node(struct context * ctx, struct node * root, int data1, int data2)
{

    root->left = node_alloc(ctx);
    root->left->data = data1;
    root->left->left = NULL;
    root->left->right = NULL;


    root->right = node_alloc(ctx);
    root->right->data = data2;
    root->right->left = NULL;
    root->right->right = NULL;

    return 0;
}


void free_tree(struct context * ctx, struct node * t)
{
    struct node * bl;
    for (bl = t; bl != NULL && bl->left != NULL; bl = bl->left);

    for (; t != NULL;)
    {
        for (bl->left = t->right; bl != NULL && bl->left != NULL; bl = bl->left);

        struct node * old = t;
        t = t->left;
        printf("%d ", old->data);
        node_free(ctx, old);
    }
}

int main(int argc, const char ** argv)
{

    struct node * root;
    struct context ctx;
    memset(&ctx, 0, sizeof(ctx));

    root = node_alloc(&ctx);
    root->data = 1;

    add_node(&ctx,root, 2, 3);
    add_node(&ctx, root->left, 4, 5);
    add_node(&ctx, root->right, 6, 7);

    //          1
    //        /   \
    //       2     3
    //     /   \  /  \
    //    4     5 6   7

    // free 1 2 4 3 6 5 7
    free_tree(&ctx, root);

    if (ctx.memory_used != 0)
    {
        fprintf(stderr, "Memory leak node count = %d\n", ctx.memory_used);
        fflush(stderr);
    }


    return 0;
}

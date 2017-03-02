#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <fs_stub/fs_stub.h>

#define ASSERT_TRUE(expr) \
    do { \
        assert((expr)); \
        if (!(expr)) {\
            fprintf(stderr, "%s:%d:%s. ASSERT_TRUE '%s' failed\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, #expr); \
            exit(-1); \
        } \
    } while (0)

static void test1()
{
    struct FsStubNode *top = fsStubCreateTopLevel();
    struct FsStubNode *bb;
    struct FsStubNode *subPath2;
    struct FsStubNode *nn;

    bb = FsStubNode_add(top, "aa/bb", dir, 660, 0);
    FsStubNode_add(bb, "gg", file, 777, 8768);

    FsStubNode_add(top, "aa/cc", dir, 660, 0);
    FsStubNode_add(top, "aa/cc/ff", file, 660, 0);
    nn = FsStubNode_add(top, "aa/cc/nn", file, 660, 0);
    FsStubNode_add(top, "aa/cc/gg", file, 660, 0);

    FsStubNode_add(top, "path2/subpath2/subfile1", file, 660, 0);
    subPath2 = FsStubNode_find(top, "path2/subpath2");

    FsStubNode_add(top, "aa/cc/gg", dir, 777, 0);
    ASSERT_TRUE(FsStubNode_find(top, "/aa/cc/gg") != NULL);
    ASSERT_TRUE(FsStubNode_find(top, "/aa/cc/gg")->type == file);
    ASSERT_TRUE(FsStubNode_find(top, "/aa/cc/gg")->mask == 660);

    ASSERT_TRUE(nn);
    ASSERT_TRUE(strcmp(nn->name, "nn") == 0);
    ASSERT_TRUE(strcmp(nn->prev->name, "ff") == 0);
    ASSERT_TRUE(strcmp(nn->next->name, "gg") == 0);

    ASSERT_TRUE(strcmp(FsStubNode_find(bb, "gg")->name, "gg" ) == 0);
    ASSERT_TRUE(bb->parent->type == dir);
    ASSERT_TRUE(bb->child->type == file);
    ASSERT_TRUE(strcmp(bb->parent->name, "aa") == 0);
    ASSERT_TRUE(bb->parent->type == dir);


    ASSERT_TRUE(FsStubNode_find(top, "/aa/bb/gg") != NULL);
    ASSERT_TRUE(FsStubNode_find(top, "/aa/cc/ss") == NULL);
    ASSERT_TRUE(FsStubNode_find(bb, "gg") != NULL);

    ASSERT_TRUE(subPath2->size == 0);
    ASSERT_TRUE(subPath2->type == dir);
    ASSERT_TRUE(strcmp(subPath2->name, "subpath2") == 0);

    FsStubNode_rename(nn, "NewNNName");
    ASSERT_TRUE(FsStubNode_find(top, "/aa/cc/nn") == NULL);
    ASSERT_TRUE(strcmp(FsStubNode_find(top, "/aa/cc/NewNNName")->name, "NewNNName" ) == 0);

    FsStubNode_remove(FsStubNode_find(top, "path2/subpath2"));

    ASSERT_TRUE(FsStubNode_find(top, "path2/subpath2") == NULL);
    ASSERT_TRUE(FsStubNode_find(top, "path2/subpath2/subfile1") == NULL);

    FsStubNode_remove(top);
}

static char *memdup(const void *src, int srcLen)
{
    char *dst = malloc(srcLen + 1);
    memcpy(dst, src, srcLen);
    dst[srcLen] = '\0';

    return dst;
}

struct TestNode 
{
    char *name;
    struct TestNode *next;
};

struct TestNode *TestNode_insertBefore(
    struct TestNode *currentNode, 
    const char *name)
{
    struct TestNode *newNode;

    newNode = malloc(sizeof(*newNode));
    newNode->name = memdup(name, strlen(name));
    if (currentNode)
        newNode->next = currentNode;
    else
        newNode->next = NULL;

    return newNode;
}

int TestNode_exists(struct TestNode *root, const char *name)
{
    for ( ; root; root = root->next)
    {
        if (strcmp(root->name, name) == 0)
            return 1;
    }

    return 0;
}

void TestNode_forEach(
    struct TestNode *root, 
    void *ctx,
    void (*action)(void *, struct TestNode *))
{
    for (; root; root = root->next)
        action(ctx, root);
}

void TestNode_freeRecursive(struct TestNode *head)
{
    struct TestNode *tmp;

    while (head)
    {
        tmp = head->next;
        free(head->name);
        free(head);
        head = tmp;
    }
}

static void addNodeNameToTestList(void *ctx, struct FsStubNode *node)
{
    struct TestNode **testNode = (struct TestNode **)ctx;

    *testNode = TestNode_insertBefore(*testNode, node->name); 
}

static void printTestNodeAction(void *ctx, struct TestNode *node)
{
    printf("%s\n", node->name);
}

static int existsInStringList(const char *pattern, const char **stringList)
{
    const char **cur;

    for (cur = stringList; *cur; ++cur)
        if (strstr(*cur, pattern))
            return 1;

    return 0;
}

static void assertNameIsInPathsAction(void *ctx, struct TestNode *node)
{
    const char **stringList = (const char **)ctx;

    ASSERT_TRUE(existsInStringList(node->name, stringList));
}

static void forEachNodeTest()
{
    struct FsStubNode *top = fsStubCreateTopLevel();
    const char* paths[] = {
        "/some/path/file1",
        "/some/path/file1",
        "/some/path/file3",
        "/some/path/subpath/file4",
        NULL,
    };
    const char **currentPath;
    struct TestNode *testHead;

    for (currentPath = paths; *currentPath; currentPath++)
        FsStubNode_add(top, *currentPath, file, 0600, 111);

    testHead = TestNode_insertBefore(NULL, "");
    FsStubNode_forEach(top, &testHead, addNodeNameToTestList);

    TestNode_forEach(testHead, paths, assertNameIsInPathsAction);
    TestNode_forEach(testHead, NULL, printTestNodeAction);

    FsStubNode_remove(top);
    TestNode_freeRecursive(testHead);
}

static void removeTest()
{
    struct FsStubNode *top = fsStubCreateTopLevel();
    struct FsStubNode* cur;
    
    FsStubNode_add(top, "some/path/1", file, 660, 1);
    FsStubNode_add(top, "some/path/2", file, 660, 1);
    FsStubNode_add(top, "some/path/3", file, 660, 1);
    FsStubNode_add(top, "some/path/4", file, 660, 1);
    
    FsStubNode_remove(top, "some/path/1");
    ASSERT_TRUE(FsStubNode_find(top, "some/path/2"));
    ASSERT_TRUE(FsStubNode_find(top, "some/path/4"));
    
    FsStubNode_remove(top, "some/path/3");
    ASSERT_TRUE(FsStubNode_find(top, "some/path/2"));
    ASSERT_TRUE(FsStubNode_find(top, "some/path/4"));
}

int main()
{
    test1();
    forEachNodeTest();
    removeTest();
    return 0;
}

#include <stdio.h>
#include <assert.h>
#include <fs_stub/fs_stub.h>

#define ASSERT_TRUE(expr) \
    do { \
        assert((expr)); \
        if (!(expr)) {\
            exit(-1); \
            fprintf(stderr, "%s:%d:%s. ASSERT_TRUE '%s' failed\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, #expr); \
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

int main()
{
    test1();
    return 0;
}

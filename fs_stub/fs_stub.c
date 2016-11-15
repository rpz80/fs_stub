#include <string.h>
#include <stdlib.h>
#include "fs_stub.h"

static int createStringCopy(const char *source, const char **target)
{
    int sourceLen;
    char *stringCopy;

    sourceLen = strlen(source);
    stringCopy = malloc(sourceLen + 1);
    if (stringCopy == NULL)
        return -1;

    memcpy(stringCopy, source, sourceLen);
    stringCopy[sourceLen] = '\0';
    *target = stringCopy;

    return 0;
}

struct SubString
{
    const char *start;
    int len;
};

static struct SubString section(const char *source, char separator, int startIndex)
{
    struct SubString result;

    memset(&result, 0, sizeof(result));
    source = source + startIndex;

    while (*source == separator)
        ++source;

    result.start = source;

    while (*result.start && *result.start != separator)
        ++result.start;

    result.len = result.start - source;

    return result;
}

static struct FsStubNode *createNode(
        struct FsStubNode *parent,
        struct SubString substring,
        int type,
        int mask,
        int64_t size)
{
    struct FsStubNode *fsNode;
    struct FsStubNode *iteratorNode;
    struct FsStubNode *prevIteratorNode;

    fsNode = malloc(sizeof(*fsNode));
    memset(fsNode, 0, sizeof(*fsNode));

    if (fsNode == NULL)
        return NULL;

    fsNode->child = NULL;
    fsNode->mask = mask;

    if (createStringCopy(name, &fsNode->name) != 0)
        return NULL;

    fsNode->next = NULL;
    fsNode->parent = parent;

    if (parent != NULL)
    {
        prevIteratorNode = iteratorNode = parent->child;
        while (1)
        {
            if (iteratorNode == NULL)
                break;
            prevIteratorNode = iteratorNode;
            iteratorNode = iteratorNode->next;
        }
        fsNode->prev = prevIteratorNode;
        prevIteratorNode->next = fsNode;
    }
    else
        fsNode->prev = NULL;

    fsNode->size = size;
    fsNode->type = type;

    return fsNode;
}

struct FsStubNode *fsStubCreateTopLevel()
{
    return createNode(NULL, "/", dir, 0, 0);
}

struct FsStubNode *FsStubNode_add(
        struct FsStubNode *parent,
        const char *path,
        int type,
        int mask,
        int64_t size)
{
    struct FsStubNode *fsNode = NULL;
    struct SubString substring;

    while (1)
    {
        substring = section(path, '/', 0);
        if (!substring)
            break;
        fsNode = createNode(parent, )
    }

    return fsNode;
}

struct FsStubNode *FsStubNode_find(struct FsStubNode *topLevelNode, const char *path);
int FsStubNode_rename(struct FsStubNode *node, const char *newName);
int FsStubNode_remove(struct FsStubNode *fsNode);

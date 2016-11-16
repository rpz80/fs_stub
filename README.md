[![Build Status](https://travis-ci.org/rpz80/fs_stub.svg?branch=master)](https://travis-ci.org/rpz80/fs_stub)
# Fs stub library
Might be used to emulate file system sructure. I use it for testing purposes. There are no correctenss checks in a sense that you can create file, which has another file (not directory) as parent and so on. Refer to fs_stub.h for interface.

## Sample usage
``` C
// create root ('/') folder first
struct FsStubNode *top = fsStubCreateTopLevel(); 
struct FsStubNode *bb;
struct FsStubNode *subPath2;
struct FsStubNode *nn;

// all nested nodes are created automatically
bb = FsStubNode_add(top, "aa/bb", dir, 660, 0); 
FsStubNode_add(bb, "gg", file, 777, 8768);

FsStubNode_add(top, "aa/cc", dir, 660, 0);
FsStubNode_add(top, "aa/cc/ff", file, 660, 0);
nn = FsStubNode_add(top, "aa/cc/nn", file, 660, 0);
FsStubNode_add(top, "aa/cc/gg", file, 660, 0);

FsStubNode_add(top, "path2/subpath2/subfile1", file, 660, 0);
subPath2 = FsStubNode_find(top, "path2/subpath2");

FsStubNode_add(top, "aa/cc/gg", dir, 777, 0);

FsStubNode_rename(nn, "NewNNName");
FsStubNode_remove(FsStubNode_find(top, "path2/subpath2"));

FsStubNode_remove(top);
```

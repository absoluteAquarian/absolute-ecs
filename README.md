# absolute-ecs
An implementation of the Entity, Component System (ECS) style in C.

---
Building the project is done via the makefile.
```
$ make
```
Two targets, `TARGET` and `TOS` can be defined.
* `TARGET`: Whether the project builds a `.exe` or a `.dll`/`.so`
* `TOS`: Whether the project is for Windows (`OS_WIN`) or Unix (`OS_UNIX`), such as Linux.

**Example (Windows, EXE):**
```
$ make TARGET=exe TOS=OS_WIN
```
**Example (Linux, SO):**
```
$ make TARGET=dll TOS=OS_UNIX
```
---
The `bin/` and `obj/` folders created by the makefile can be deleted by running this command:
```
$ make clean
```

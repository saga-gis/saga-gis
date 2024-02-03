# Pretty-Printer for [GDB](https://www.sourceware.org/gdb/)

## Using Pretty-Printer in GDB. 

Simply add the following line to your `gdbinit` file. This file is typically located in `XDG_CONFIG_HOMR/gdb/` or `$HOME/` as `.gdbinit`.
```
source path/to/gdb_saga_api_pp.py 
```
To obtain information about the Pretty-Printer and en/disable them use: 
```
(gdb) info pretty-printer
(gdb) enable pretty-printer
(gdb) disable pretty-printer
```


## Debugging process for Pretty-Printer

Edit the pretty-printer as you like and simply resource the file within GDB with:
```
(gdb) source ~/path/to/gdb_saga_api_pp.py
```




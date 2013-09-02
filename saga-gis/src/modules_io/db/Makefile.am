if HAVE_POSTGRESQL
if WITH_ODBC
SUBDIRS = io_odbc io_pgsql
else
SUBDIRS = io_pgsql
endif
else
if WITH_ODBC
SUBDIRS = io_odbc
else
SUBDIRS =
endif
endif

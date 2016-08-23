if HAVE_POSTGRESQL
if WITH_ODBC
SUBDIRS = db_odbc db_pgsql
else
SUBDIRS = db_pgsql
endif
else
if WITH_ODBC
SUBDIRS = db_odbc
else
SUBDIRS =
endif
endif

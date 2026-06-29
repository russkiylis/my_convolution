#!/bin/bash
set -e		# Stop on errors

# -v for variables in .sql file, -f for using a file
psql -f drop_database.sql

echo "Database successfully dropped"

#!/bin/bash
set -e		# Stop on errors

read -s -p "Enter stc database password:" $DB_PASSWORD		# -s not to show password, -p for text
echo

# -U for user selection, -v for variables in .sql file, -f for using a file
sudo psql -U postgres -v db_password="$DB_PASSWORD" -f create_database.sql

echo "Database for stc_pelengator successfully created"

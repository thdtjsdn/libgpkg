/*
 * Copyright 2013 Luciad (http://www.luciad.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include "check.h"
#include "sql.h"

static int integrity_check_row(sqlite3 *db, sqlite3_stmt *stmt, void *data) {
  const char *row = (const char *)sqlite3_column_text(stmt, 0);
  if (sqlite3_strnicmp(row, "ok", 3) != 0) {
    error_append((error_t *)data, "integrity: %s", row);
  }
  return SQLITE_OK;
}

static int integrity_check(sqlite3 *db, const char *db_name, error_t *error) {
  return sql_exec_stmt(db, integrity_check_row, NULL, error, "PRAGMA integrity_check");
}

static int foreign_key_check_row(sqlite3 *db, sqlite3_stmt *stmt, void *data) {
  char* table = sqlite3_mprintf("%s", sqlite3_column_text(stmt, 0));
  char* rowId = sqlite3_mprintf("%s", sqlite3_column_text(stmt, 1));
  char* referred = sqlite3_mprintf("%s", sqlite3_column_text(stmt, 2));
  char* index = sqlite3_mprintf("%s", sqlite3_column_text(stmt, 3));

  error_append((error_t *)data, "Reference error in table=%s, rowId=%s, referred table=%s, index foreign key=%s",
               table, rowId, referred, index);
  sqlite3_free(table);
  sqlite3_free(rowId);
  sqlite3_free(referred);
  sqlite3_free(index);

  return SQLITE_OK;
}

static int foreign_key_check(sqlite3 *db, const char *db_name, error_t *error) {
  return sql_exec_stmt(db, foreign_key_check_row, NULL, error,
                       "PRAGMA foreign_key_check");
}

typedef int(*check_func)(sqlite3 *db, const char *db_name, error_t *error);

static check_func checks[] = {
  integrity_check,
  foreign_key_check,
  NULL
};

int check_integrity(sqlite3 *db, const char *db_name, error_t *error) {
  int result = SQLITE_OK;

  check_func *current_func = checks;
  while (*current_func != NULL) {
    result = (*current_func)(db, db_name, error);
    if (result != SQLITE_OK) {
      break;
    }
    current_func++;
  }

  return result;
}

int check_database(sqlite3 *db, const char *db_name, const table_info_t *const *tables, error_t *error) {
int result = SQLITE_OK;

  const table_info_t *const *table = tables;
  while (*table != NULL) {
    result = sql_check_table(db, db_name, *table, error);
    if (result != SQLITE_OK) {
      break;
    }
    table++;
  }

  return result;
}

int init_database(sqlite3 *db, const char *db_name, const table_info_t *const *tables, error_t *error) {
  int result = SQLITE_OK;
  const table_info_t *const *table = tables;

  while (*table != NULL) {
    result = sql_init_table(db, db_name, *table, error);
    if (result != SQLITE_OK) {
      break;
    }
    table++;
  }

  if (result == SQLITE_OK && error_count(error) > 0) {
    return SQLITE_ERROR;
  } else {
    return result;
  }
}
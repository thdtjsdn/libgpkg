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
#ifndef GPKG_CHECK_H
#define GPKG_CHECK_H

#include "sqlite.h"
#include "error.h"
#include "sql.h"

int init_database(sqlite3 *db, const char *db_name, const table_info_t *const *tables, error_t *error);

int check_database(sqlite3 *db, const char *db_name, const table_info_t *const *tables, error_t *error);

int check_integrity(sqlite3 *db, const char *db_name, error_t *error);

#endif

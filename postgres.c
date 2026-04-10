#define HL_NAME(n) postgres_##n

#include <hl.h>
#include <stdlib.h>
#include <string.h>

#if defined(__has_include)
#  if __has_include(<libpq-fe.h>)
#    include <libpq-fe.h>
#  elif __has_include(<postgresql/libpq-fe.h>)
#    include <postgresql/libpq-fe.h>
#  else
#    error "Could not find libpq-fe.h"
#  endif
#else
#  include <libpq-fe.h>
#endif

typedef struct hl_postgres {
    void (*finalize)(struct hl_postgres*);
    PGconn* conn;
} hl_postgres;

typedef struct hl_postgres_result {
    void (*finalize)(struct hl_postgres_result*);
    PGresult* result;
} hl_postgres_result;

#define _POSTGRES _ABSTRACT(hl_postgres)
#define _POSTGRES_RESULT _ABSTRACT(hl_postgres_result)

static void postgres_handle_finalizer(hl_postgres* h) {
    if (h && h->conn) {
        PQfinish(h->conn);
        h->conn = NULL;
    }
}

static void postgres_result_handle_finalizer(hl_postgres_result* h) {
    if (h && h->result) {
        PQclear(h->result);
        h->result = NULL;
    }
}

static hl_postgres_result* alloc_result(PGresult* result) {
    hl_postgres_result* h;

    if (!result) return NULL;

    h = (hl_postgres_result*)hl_gc_alloc_finalizer(sizeof(hl_postgres_result));
    h->finalize = postgres_result_handle_finalizer;
    h->result = result;
    return h;
}

HL_PRIM hl_postgres* HL_NAME(postgres_connect)(vbyte* conninfo) {
    PGconn* conn = PQconnectdb((const char*)conninfo);
    hl_postgres* h;

    if (!conn) return NULL;
    if (PQstatus(conn) != CONNECTION_OK) {
        PQfinish(conn);
        return NULL;
    }

    h = (hl_postgres*)hl_gc_alloc_finalizer(sizeof(hl_postgres));
    h->finalize = postgres_handle_finalizer;
    h->conn = conn;
    return h;
}

HL_PRIM void HL_NAME(postgres_close)(hl_postgres* h) {
    if (!h || !h->conn) return;
    PQfinish(h->conn);
    h->conn = NULL;
}

HL_PRIM vbyte* HL_NAME(postgres_last_error)(hl_postgres* h) {
    const char* msg;
    size_t len;

    if (!h || !h->conn) return NULL;

    msg = PQerrorMessage(h->conn);
    if (!msg || msg[0] == '\0') return NULL;

    len = strlen(msg);
    while (len > 0 && (msg[len - 1] == '\r' || msg[len - 1] == '\n'))
        len--;

    return hl_copy_bytes((const vbyte*)msg, (int)len);
}

HL_PRIM int HL_NAME(postgres_last_error_length)(hl_postgres* h) {
    const char* msg;
    size_t len;

    if (!h || !h->conn) return 0;

    msg = PQerrorMessage(h->conn);
    if (!msg || msg[0] == '\0') return 0;

    len = strlen(msg);
    while (len > 0 && (msg[len - 1] == '\r' || msg[len - 1] == '\n'))
        len--;

    return (int)len;
}

HL_PRIM bool HL_NAME(postgres_is_ok)(hl_postgres* h) {
    if (!h || !h->conn) return false;
    return PQstatus(h->conn) == CONNECTION_OK;
}

HL_PRIM hl_postgres_result* HL_NAME(postgres_query)(hl_postgres* h, vbyte* sql) {
    if (!h || !h->conn) return NULL;
    return alloc_result(PQexec(h->conn, (const char*)sql));
}

HL_PRIM int HL_NAME(postgres_result_status)(hl_postgres_result* h) {
    if (!h || !h->result) return -1;
    return (int)PQresultStatus(h->result);
}

HL_PRIM vbyte* HL_NAME(postgres_result_error)(hl_postgres_result* h) {
    const char* msg;
    size_t len;

    if (!h || !h->result) return NULL;

    msg = PQresultErrorMessage(h->result);
    if (!msg || msg[0] == '\0') return NULL;

    len = strlen(msg);
    while (len > 0 && (msg[len - 1] == '\r' || msg[len - 1] == '\n'))
        len--;

    return hl_copy_bytes((const vbyte*)msg, (int)len);
}

HL_PRIM int HL_NAME(postgres_result_error_length)(hl_postgres_result* h) {
    const char* msg;
    size_t len;

    if (!h || !h->result) return 0;

    msg = PQresultErrorMessage(h->result);
    if (!msg || msg[0] == '\0') return 0;

    len = strlen(msg);
    while (len > 0 && (msg[len - 1] == '\r' || msg[len - 1] == '\n'))
        len--;

    return (int)len;
}

HL_PRIM int HL_NAME(postgres_num_rows)(hl_postgres_result* h) {
    if (!h || !h->result) return 0;
    return PQntuples(h->result);
}

HL_PRIM int HL_NAME(postgres_num_fields)(hl_postgres_result* h) {
    if (!h || !h->result) return 0;
    return PQnfields(h->result);
}

HL_PRIM vbyte* HL_NAME(postgres_field_name)(hl_postgres_result* h, int field) {
    const char* name;

    if (!h || !h->result) return NULL;
    if (field < 0 || field >= PQnfields(h->result)) return NULL;

    name = PQfname(h->result, field);
    if (!name) return NULL;

    return hl_copy_bytes((const vbyte*)name, (int)strlen(name));
}

HL_PRIM int HL_NAME(postgres_field_name_length)(hl_postgres_result* h, int field) {
    const char* name;

    if (!h || !h->result) return 0;
    if (field < 0 || field >= PQnfields(h->result)) return 0;

    name = PQfname(h->result, field);
    if (!name) return 0;

    return (int)strlen(name);
}

HL_PRIM bool HL_NAME(postgres_is_null)(hl_postgres_result* h, int row, int field) {
    if (!h || !h->result) return true;
    if (row < 0 || row >= PQntuples(h->result)) return true;
    if (field < 0 || field >= PQnfields(h->result)) return true;

    return PQgetisnull(h->result, row, field) != 0;
}

HL_PRIM vbyte* HL_NAME(postgres_get_value)(hl_postgres_result* h, int row, int field) {
    char* value;
    int len;

    if (!h || !h->result) return NULL;
    if (row < 0 || row >= PQntuples(h->result)) return NULL;
    if (field < 0 || field >= PQnfields(h->result)) return NULL;
    if (PQgetisnull(h->result, row, field)) return NULL;

    value = PQgetvalue(h->result, row, field);
    len = PQgetlength(h->result, row, field);
    return hl_copy_bytes((const vbyte*)value, len);
}

HL_PRIM int HL_NAME(postgres_get_length)(hl_postgres_result* h, int row, int field) {
    if (!h || !h->result) return 0;
    if (row < 0 || row >= PQntuples(h->result)) return 0;
    if (field < 0 || field >= PQnfields(h->result)) return 0;
    if (PQgetisnull(h->result, row, field)) return 0;

    return PQgetlength(h->result, row, field);
}

HL_PRIM int HL_NAME(postgres_cmd_tuples)(hl_postgres_result* h) {
    const char* s;
    int value;

    if (!h || !h->result) return -1;

    s = PQcmdTuples(h->result);
    if (!s || s[0] == '\0') return 0;

    value = atoi(s);
    return value;
}

HL_PRIM void HL_NAME(postgres_result_clear)(hl_postgres_result* h) {
    if (!h || !h->result) return;
    PQclear(h->result);
    h->result = NULL;
}

DEFINE_PRIM(_POSTGRES,        postgres_connect, _BYTES);
DEFINE_PRIM(_VOID,            postgres_close, _POSTGRES);
DEFINE_PRIM(_BYTES,           postgres_last_error, _POSTGRES);
DEFINE_PRIM(_I32,             postgres_last_error_length, _POSTGRES);
DEFINE_PRIM(_BOOL,            postgres_is_ok, _POSTGRES);
DEFINE_PRIM(_POSTGRES_RESULT, postgres_query, _POSTGRES _BYTES);

DEFINE_PRIM(_I32,             postgres_result_status, _POSTGRES_RESULT);
DEFINE_PRIM(_BYTES,           postgres_result_error, _POSTGRES_RESULT);
DEFINE_PRIM(_I32,             postgres_result_error_length, _POSTGRES_RESULT);
DEFINE_PRIM(_I32,             postgres_num_rows, _POSTGRES_RESULT);
DEFINE_PRIM(_I32,             postgres_num_fields, _POSTGRES_RESULT);
DEFINE_PRIM(_BYTES,           postgres_field_name, _POSTGRES_RESULT _I32);
DEFINE_PRIM(_I32,             postgres_field_name_length, _POSTGRES_RESULT _I32);
DEFINE_PRIM(_BOOL,            postgres_is_null, _POSTGRES_RESULT _I32 _I32);
DEFINE_PRIM(_BYTES,           postgres_get_value, _POSTGRES_RESULT _I32 _I32);
DEFINE_PRIM(_I32,             postgres_get_length, _POSTGRES_RESULT _I32 _I32);
DEFINE_PRIM(_I32,             postgres_cmd_tuples, _POSTGRES_RESULT);
DEFINE_PRIM(_VOID,            postgres_result_clear, _POSTGRES_RESULT);

/*PGR-GNU*****************************************************************
File: trsp_withPoints.c

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2022 Celia Virginia Vergara Castillo
Mail: vicky at georepublic.de

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 ********************************************************************PGR-GNU*/

#include <stdbool.h>
#include "c_common/postgres_connection.h"

#include "c_types/path_rt.h"
#include "c_common/debug_macro.h"
#include "c_common/e_report.h"
#include "c_common/time_msg.h"
#include "drivers/withPoints/get_new_queries.h"
#include "drivers/trsp/trsp_withPoints_driver.h"

PGDLLEXPORT Datum _pgr_trsp_withpoints(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(_pgr_trsp_withpoints);


static
void
process(
        char* edges_sql,
        char* restrictions_sql,
        char* points_sql,
        char* combinations_sql,

        ArrayType *starts,
        ArrayType *ends,

        bool directed,
        char *driving_side,
        bool details,

        Path_rt **result_tuples,
        size_t *result_count) {
    pgr_SPI_connect();
    char* log_msg = NULL;
    char* notice_msg = NULL;
    char* err_msg = NULL;


    /*
     * Estimate driving side
     */
    driving_side[0] = estimate_drivingSide(driving_side[0]);
    if (driving_side[0] != 'r' && driving_side[0] != 'l') {
        driving_side[0] = 'l';
    }

    char *edges_of_points_query = NULL;
    char *edges_no_points_query = NULL;
    get_new_queries(
            edges_sql, points_sql,
            &edges_of_points_query,
            &edges_no_points_query);


    clock_t start_t = clock();
    pgr_do_trsp_withPoints(
            edges_no_points_query,
            restrictions_sql,
            points_sql,
            edges_of_points_query,
            combinations_sql,
            starts, ends,

            directed,

            driving_side[0],
            details,

            result_tuples, result_count,
            &log_msg,
            &notice_msg,
            &err_msg);

    time_msg("processing pgr_trps_withPoints", start_t, clock());

    if (err_msg && (*result_tuples)) {
        pfree(*result_tuples);
        (*result_tuples) = NULL;
        (*result_count) = 0;
    }

    pgr_global_report(&log_msg, &notice_msg, &err_msg);

    if (edges_of_points_query) {
        pfree(edges_of_points_query); edges_of_points_query = NULL;
    }
    if (edges_no_points_query) {
        pfree(edges_no_points_query); edges_no_points_query = NULL;
    }

    pgr_SPI_finish();
}




PGDLLEXPORT Datum
_pgr_trsp_withpoints(PG_FUNCTION_ARGS) {
    FuncCallContext     *funcctx;
    TupleDesc            tuple_desc;

    Path_rt *result_tuples = 0;
    size_t result_count = 0;

    if (SRF_IS_FIRSTCALL()) {
        MemoryContext   oldcontext;
        funcctx = SRF_FIRSTCALL_INIT();
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);


        if (PG_NARGS() == 8) {
            /*
             * many to many
             */

            process(
                text_to_cstring(PG_GETARG_TEXT_P(0)),
                text_to_cstring(PG_GETARG_TEXT_P(1)),
                text_to_cstring(PG_GETARG_TEXT_P(2)),
                NULL,  // combinations
                PG_GETARG_ARRAYTYPE_P(3),
                PG_GETARG_ARRAYTYPE_P(4),
                PG_GETARG_BOOL(5),
                text_to_cstring(PG_GETARG_TEXT_P(6)),
                PG_GETARG_BOOL(7),
                &result_tuples,
                &result_count);

        } else if (PG_NARGS() == 7) {
            /*
             * Combinations
             */

            process(
                text_to_cstring(PG_GETARG_TEXT_P(0)),
                text_to_cstring(PG_GETARG_TEXT_P(1)),
                text_to_cstring(PG_GETARG_TEXT_P(2)),
                text_to_cstring(PG_GETARG_TEXT_P(3)),
                NULL, NULL,  // starts & ends
                PG_GETARG_BOOL(5),
                text_to_cstring(PG_GETARG_TEXT_P(5)),
                PG_GETARG_BOOL(6),
                &result_tuples,
                &result_count);
        }

        funcctx->max_calls = result_count;
        funcctx->user_fctx = result_tuples;
        if (get_call_result_type(fcinfo, NULL, &tuple_desc)
                != TYPEFUNC_COMPOSITE)
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("function returning record called in context "
                         "that cannot accept type record")));

        funcctx->tuple_desc = tuple_desc;
        MemoryContextSwitchTo(oldcontext);
    }

    funcctx = SRF_PERCALL_SETUP();
    tuple_desc = funcctx->tuple_desc;
    result_tuples = (Path_rt*) funcctx->user_fctx;

    if (funcctx->call_cntr < funcctx->max_calls) {
        HeapTuple    tuple;
        Datum        result;
        Datum        *values;
        bool*        nulls;
        size_t call_cntr = funcctx->call_cntr;

        size_t numb = 8;
        values = palloc(numb * sizeof(Datum));
        nulls = palloc(numb * sizeof(bool));

        size_t i;
        for (i = 0; i < numb; ++i) {
            nulls[i] = false;
        }

        int64_t seq = call_cntr == 0? 1 : result_tuples[call_cntr - 1].start_id;
        values[0] = Int32GetDatum((int32_t)call_cntr + 1);
        values[1] = Int32GetDatum((int32_t)seq);
        values[2] = Int64GetDatum(result_tuples[call_cntr].start_id);
        values[3] = Int64GetDatum(result_tuples[call_cntr].end_id);
        values[4] = Int64GetDatum(result_tuples[call_cntr].node);
        values[5] = Int64GetDatum(result_tuples[call_cntr].edge);
        values[6] = Float8GetDatum(result_tuples[call_cntr].cost);
        values[7] = Float8GetDatum(result_tuples[call_cntr].agg_cost);

        result_tuples[call_cntr].start_id = result_tuples[call_cntr].edge < 0? 1 : seq + 1;

        tuple = heap_form_tuple(tuple_desc, values, nulls);
        result = HeapTupleGetDatum(tuple);
        SRF_RETURN_NEXT(funcctx, result);
    } else {
        SRF_RETURN_DONE(funcctx);
    }
}

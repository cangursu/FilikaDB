/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   flowlog_utilities.cpp
 * Author: can.gursu
 * 
 * Created on 25 April 2018, 10:06
 */


#include "flowlog_utilities.h"


#ifdef __cplusplus
extern "C" {
#endif

#include <catalog/pg_type.h>
#include <fmgr.h>

#ifdef __cplusplus
}
#endif


#include <sstream>


std::string text_enum(CmdType val)
{
    const char *rs = "NA";
    switch (val)
    {
        case CMD_UNKNOWN : rs = "CMD_UNKNOWN";    break;
        case CMD_SELECT  : rs = "CMD_SELECT" ;    break;
        case CMD_UPDATE  : rs = "CMD_UPDATE" ;    break;
        case CMD_INSERT  : rs = "CMD_INSERT" ;    break;
        case CMD_DELETE  : rs = "CMD_DELETE" ;    break;
        case CMD_UTILITY : rs = "CMD_UTILITY";    break;
        case CMD_NOTHING : rs = "CMD_NOTHING";    break;
    }
    return std::move(std::string(rs));
}


std::string text_enum(NodeTag val)
{
    const char *rs = "NA";
    switch (val)
    {
        case    T_Invalid                                                                  :  rs =  "T_Invalid";                   break;

                /*                                         
                 * TAGS FOR EXECUTOR NODES (execnodes.h)   
                 */                                        
        case    T_IndexInfo                                                                :  rs =  "T_IndexInfo";                 break;
        case    T_ExprContext                                                              :  rs =  "T_ExprContext";               break;
        case    T_ProjectionInfo                                                           :  rs =  "T_ProjectionInfo";            break;
        case    T_JunkFilter                                                               :  rs =  "T_JunkFilter";                break;
        case    T_ResultRelInfo                                                            :  rs =  "T_ResultRelInfo";             break;
        case    T_EState                                                                   :  rs =  "T_EState";                    break;
        case    T_TupleTableSlot                                                           :  rs =  "T_TupleTableSlot";            break;

                /*                                   
                 * TAGS FOR PLAN NODES (plannodes.h) 
                 */                                  
        case    T_Plan                                                                     :  rs =  "T_Plan";                      break;
        case    T_Result                                                                   :  rs =  "T_Result";                    break;
        case    T_ProjectSet                                                               :  rs =  "T_ProjectSet";                break;
        case    T_ModifyTable                                                              :  rs =  "T_ModifyTable";               break;
        case    T_Append                                                                   :  rs =  "T_Append";                    break;
        case    T_MergeAppend                                                              :  rs =  "T_MergeAppend";               break;
        case    T_RecursiveUnion                                                           :  rs =  "T_RecursiveUnion";            break;
        case    T_BitmapAnd                                                                :  rs =  "T_BitmapAnd";                 break;
        case    T_BitmapOr                                                                 :  rs =  "T_BitmapOr";                  break;
        case    T_Scan                                                                     :  rs =  "T_Scan";                      break;
        case    T_SeqScan                                                                  :  rs =  "T_SeqScan";                   break;
        case    T_SampleScan                                                               :  rs =  "T_SampleScan";                break;
        case    T_IndexScan                                                                :  rs =  "T_IndexScan";                 break;
        case    T_IndexOnlyScan                                                            :  rs =  "T_IndexOnlyScan";             break;
        case    T_BitmapIndexScan                                                          :  rs =  "T_BitmapIndexScan";           break;
        case    T_BitmapHeapScan                                                           :  rs =  "T_BitmapHeapScan";            break;
        case    T_TidScan                                                                  :  rs =  "T_TidScan";                   break;
        case    T_SubqueryScan                                                             :  rs =  "T_SubqueryScan";              break;
        case    T_FunctionScan                                                             :  rs =  "T_FunctionScan";              break;
        case    T_ValuesScan                                                               :  rs =  "T_ValuesScan";                break;
        case    T_TableFuncScan                                                            :  rs =  "T_TableFuncScan";             break;
        case    T_CteScan                                                                  :  rs =  "T_CteScan";                   break;
        case    T_NamedTuplestoreScan                                                      :  rs =  "T_NamedTuplestoreScan";       break;
        case    T_WorkTableScan                                                            :  rs =  "T_WorkTableScan";             break;
        case    T_ForeignScan                                                              :  rs =  "T_ForeignScan";               break;
        case    T_CustomScan                                                               :  rs =  "T_CustomScan";                break;
        case    T_Join                                                                     :  rs =  "T_Join";                      break;
        case    T_NestLoop                                                                 :  rs =  "T_NestLoop";                  break;
        case    T_MergeJoin                                                                :  rs =  "T_MergeJoin";                 break;
        case    T_HashJoin                                                                 :  rs =  "T_HashJoin";                  break;
        case    T_Material                                                                 :  rs =  "T_Material";                  break;
        case    T_Sort                                                                     :  rs =  "T_Sort";                      break;
        case    T_Group                                                                    :  rs =  "T_Group";                     break;
        case    T_Agg                                                                      :  rs =  "T_Agg";                       break;
        case    T_WindowAgg                                                                :  rs =  "T_WindowAgg";                 break;
        case    T_Unique                                                                   :  rs =  "T_Unique";                    break;
        case    T_Gather                                                                   :  rs =  "T_Gather";                    break;
        case    T_GatherMerge                                                              :  rs =  "T_GatherMerge";               break;
        case    T_Hash                                                                     :  rs =  "T_Hash";                      break;
        case    T_SetOp                                                                    :  rs =  "T_SetOp";                     break;
        case    T_LockRows                                                                 :  rs =  "T_LockRows";                  break;
        case    T_Limit                                                                    :  rs =  "T_Limit";                     break;
                /* these aren't subclasses of Plan: */
        case    T_NestLoopParam                                                            :  rs =  "T_NestLoopParam";             break;
        case    T_PlanRowMark                                                              :  rs =  "T_PlanRowMark";               break;
        case    T_PlanInvalItem                                                            :  rs =  "T_PlanInvalItem";             break;

                /*                                                         
                 * TAGS FOR PLAN STATE NODES (execnodes.h)                 
                 *                                                         
                 * These should correspond one-to-one with Plan node types.
                 */                                                        
        case    T_PlanState                                                                :  rs =  "T_PlanState";                 break;
        case    T_ResultState                                                              :  rs =  "T_ResultState";               break;
        case    T_ProjectSetState                                                          :  rs =  "T_ProjectSetState";           break;
        case    T_ModifyTableState                                                         :  rs =  "T_ModifyTableState";          break;
        case    T_AppendState                                                              :  rs =  "T_AppendState";               break;
        case    T_MergeAppendState                                                         :  rs =  "T_MergeAppendState";          break;
        case    T_RecursiveUnionState                                                      :  rs =  "T_RecursiveUnionState";       break;
        case    T_BitmapAndState                                                           :  rs =  "T_BitmapAndState";            break;
        case    T_BitmapOrState                                                            :  rs =  "T_BitmapOrState";             break;
        case    T_ScanState                                                                :  rs =  "T_ScanState";                 break;
        case    T_SeqScanState                                                             :  rs =  "T_SeqScanState";              break;
        case    T_SampleScanState                                                          :  rs =  "T_SampleScanState";           break;
        case    T_IndexScanState                                                           :  rs =  "T_IndexScanState";            break;
        case    T_IndexOnlyScanState                                                       :  rs =  "T_IndexOnlyScanState";        break;
        case    T_BitmapIndexScanState                                                     :  rs =  "T_BitmapIndexScanState";      break;
        case    T_BitmapHeapScanState                                                      :  rs =  "T_BitmapHeapScanState";       break;
        case    T_TidScanState                                                             :  rs =  "T_TidScanState";              break;
        case    T_SubqueryScanState                                                        :  rs =  "T_SubqueryScanState";         break;
        case    T_FunctionScanState                                                        :  rs =  "T_FunctionScanState";         break;
        case    T_TableFuncScanState                                                       :  rs =  "T_TableFuncScanState";        break;
        case    T_ValuesScanState                                                          :  rs =  "T_ValuesScanState";           break;
        case    T_CteScanState                                                             :  rs =  "T_CteScanState";              break;
        case    T_NamedTuplestoreScanState                                                 :  rs =  "T_NamedTuplestoreScanState";  break;
        case    T_WorkTableScanState                                                       :  rs =  "T_WorkTableScanState";        break;
        case    T_ForeignScanState                                                         :  rs =  "T_ForeignScanState";          break;
        case    T_CustomScanState                                                          :  rs =  "T_CustomScanState";           break;
        case    T_JoinState                                                                :  rs =  "T_JoinState";                 break;
        case    T_NestLoopState                                                            :  rs =  "T_NestLoopState";             break;
        case    T_MergeJoinState                                                           :  rs =  "T_MergeJoinState";            break;
        case    T_HashJoinState                                                            :  rs =  "T_HashJoinState";             break;
        case    T_MaterialState                                                            :  rs =  "T_MaterialState";             break;
        case    T_SortState                                                                :  rs =  "T_SortState";                 break;
        case    T_GroupState                                                               :  rs =  "T_GroupState";                break;
        case    T_AggState                                                                 :  rs =  "T_AggState";                  break;
        case    T_WindowAggState                                                           :  rs =  "T_WindowAggState";            break;
        case    T_UniqueState                                                              :  rs =  "T_UniqueState";               break;
        case    T_GatherState                                                              :  rs =  "T_GatherState";               break;
        case    T_GatherMergeState                                                         :  rs =  "T_GatherMergeState";          break;
        case    T_HashState                                                                :  rs =  "T_HashState";                 break;
        case    T_SetOpState                                                               :  rs =  "T_SetOpState";                break;
        case    T_LockRowsState                                                            :  rs =  "T_LockRowsState";             break;
        case    T_LimitState                                                               :  rs =  "T_LimitState";                break;

                /*                                       
                 * TAGS FOR PRIMITIVE NODES (primnodes.h)
                 */                                      
        case    T_Alias                                                                    :  rs =  "T_Alias";                     break;
        case    T_RangeVar                                                                 :  rs =  "T_RangeVar";                  break;
        case    T_TableFunc                                                                :  rs =  "T_TableFunc";                 break;
        case    T_Expr                                                                     :  rs =  "T_Expr";                      break;
        case    T_Var                                                                      :  rs =  "T_Var";                       break;
        case    T_Const                                                                    :  rs =  "T_Const";                     break;
        case    T_Param                                                                    :  rs =  "T_Param";                     break;
        case    T_Aggref                                                                   :  rs =  "T_Aggref";                    break;
        case    T_GroupingFunc                                                             :  rs =  "T_GroupingFunc";              break;
        case    T_WindowFunc                                                               :  rs =  "T_WindowFunc";                break;
        case    T_ArrayRef                                                                 :  rs =  "T_ArrayRef";                  break;
        case    T_FuncExpr                                                                 :  rs =  "T_FuncExpr";                  break;
        case    T_NamedArgExpr                                                             :  rs =  "T_NamedArgExpr";              break;
        case    T_OpExpr                                                                   :  rs =  "T_OpExpr";                    break;
        case    T_DistinctExpr                                                             :  rs =  "T_DistinctExpr";              break;
        case    T_NullIfExpr                                                               :  rs =  "T_NullIfExpr";                break;
        case    T_ScalarArrayOpExpr                                                        :  rs =  "T_ScalarArrayOpExpr";         break;
        case    T_BoolExpr                                                                 :  rs =  "T_BoolExpr";                  break;
        case    T_SubLink                                                                  :  rs =  "T_SubLink";                   break;
        case    T_SubPlan                                                                  :  rs =  "T_SubPlan";                   break;
        case    T_AlternativeSubPlan                                                       :  rs =  "T_AlternativeSubPlan";        break;
        case    T_FieldSelect                                                              :  rs =  "T_FieldSelect";               break;
        case    T_FieldStore                                                               :  rs =  "T_FieldStore";                break;
        case    T_RelabelType                                                              :  rs =  "T_RelabelType";               break;
        case    T_CoerceViaIO                                                              :  rs =  "T_CoerceViaIO";               break;
        case    T_ArrayCoerceExpr                                                          :  rs =  "T_ArrayCoerceExpr";           break;
        case    T_ConvertRowtypeExpr                                                       :  rs =  "T_ConvertRowtypeExpr";        break;
        case    T_CollateExpr                                                              :  rs =  "T_CollateExpr";               break;
        case    T_CaseExpr                                                                 :  rs =  "T_CaseExpr";                  break;
        case    T_CaseWhen                                                                 :  rs =  "T_CaseWhen";                  break;
        case    T_CaseTestExpr                                                             :  rs =  "T_CaseTestExpr";              break;
        case    T_ArrayExpr                                                                :  rs =  "T_ArrayExpr";                 break;
        case    T_RowExpr                                                                  :  rs =  "T_RowExpr";                   break;
        case    T_RowCompareExpr                                                           :  rs =  "T_RowCompareExpr";            break;
        case    T_CoalesceExpr                                                             :  rs =  "T_CoalesceExpr";              break;
        case    T_MinMaxExpr                                                               :  rs =  "T_MinMaxExpr";                break;
        case    T_SQLValueFunction                                                         :  rs =  "T_SQLValueFunction";          break;
        case    T_XmlExpr                                                                  :  rs =  "T_XmlExpr";                   break;
        case    T_NullTest                                                                 :  rs =  "T_NullTest";                  break;
        case    T_BooleanTest                                                              :  rs =  "T_BooleanTest";               break;
        case    T_CoerceToDomain                                                           :  rs =  "T_CoerceToDomain";            break;
        case    T_CoerceToDomainValue                                                      :  rs =  "T_CoerceToDomainValue";       break;
        case    T_SetToDefault                                                             :  rs =  "T_SetToDefault";              break;
        case    T_CurrentOfExpr                                                            :  rs =  "T_CurrentOfExpr";             break;
        case    T_NextValueExpr                                                            :  rs =  "T_NextValueExpr";             break;
        case    T_InferenceElem                                                            :  rs =  "T_InferenceElem";             break;
        case    T_TargetEntry                                                              :  rs =  "T_TargetEntry";               break;
        case    T_RangeTblRef                                                              :  rs =  "T_RangeTblRef";               break;
        case    T_JoinExpr                                                                 :  rs =  "T_JoinExpr";                  break;
        case    T_FromExpr                                                                 :  rs =  "T_FromExpr";                  break;
        case    T_OnConflictExpr                                                           :  rs =  "T_OnConflictExpr";            break;
        case    T_IntoClause                                                               :  rs =  "T_IntoClause";                break;

                /*                                                                        
                 * TAGS FOR EXPRESSION STATE NODES (execnodes.h)                          
                 *                                                                        
                 * ExprState represents the evaluation state for a whole expression tree. 
                 * Most Expr-based plan nodes do not have a corresponding expression state
                 * node they're fully handled within execExpr* - but sometimes the state 
                 * needs to be shared with other parts of the executor as for example    
                 * with AggrefExprState which nodeAgg.c has to modify.                   
                 */                                                                       
        case    T_ExprState                                                                :  rs =  "T_ExprState";                 break;
        case    T_AggrefExprState                                                          :  rs =  "T_AggrefExprState";           break;
        case    T_WindowFuncExprState                                                      :  rs =  "T_WindowFuncExprState";       break;
        case    T_SetExprState                                                             :  rs =  "T_SetExprState";              break;
        case    T_SubPlanState                                                             :  rs =  "T_SubPlanState";              break;
        case    T_AlternativeSubPlanState                                                  :  rs =  "T_AlternativeSubPlanState";   break;
        case    T_DomainConstraintState                                                    :  rs =  "T_DomainConstraintState";     break;

                /*                                    
                 * TAGS FOR PLANNER NODES (relation.h)
                 */                                   
        case    T_PlannerInfo                                                              :  rs =  "T_PlannerInfo";               break;
        case    T_PlannerGlobal                                                            :  rs =  "T_PlannerGlobal";             break;
        case    T_RelOptInfo                                                               :  rs =  "T_RelOptInfo";                break;
        case    T_IndexOptInfo                                                             :  rs =  "T_IndexOptInfo";              break;
        case    T_ForeignKeyOptInfo                                                        :  rs =  "T_ForeignKeyOptInfo";         break;
        case    T_ParamPathInfo                                                            :  rs =  "T_ParamPathInfo";             break;
        case    T_Path                                                                     :  rs =  "T_Path";                      break;
        case    T_IndexPath                                                                :  rs =  "T_IndexPath";                 break;
        case    T_BitmapHeapPath                                                           :  rs =  "T_BitmapHeapPath";            break;
        case    T_BitmapAndPath                                                            :  rs =  "T_BitmapAndPath";             break;
        case    T_BitmapOrPath                                                             :  rs =  "T_BitmapOrPath";              break;
        case    T_TidPath                                                                  :  rs =  "T_TidPath";                   break;
        case    T_SubqueryScanPath                                                         :  rs =  "T_SubqueryScanPath";          break;
        case    T_ForeignPath                                                              :  rs =  "T_ForeignPath";               break;
        case    T_CustomPath                                                               :  rs =  "T_CustomPath";                break;
        case    T_NestPath                                                                 :  rs =  "T_NestPath";                  break;
        case    T_MergePath                                                                :  rs =  "T_MergePath";                 break;
        case    T_HashPath                                                                 :  rs =  "T_HashPath";                  break;
        case    T_AppendPath                                                               :  rs =  "T_AppendPath";                break;
        case    T_MergeAppendPath                                                          :  rs =  "T_MergeAppendPath";           break;
        case    T_ResultPath                                                               :  rs =  "T_ResultPath";                break;
        case    T_MaterialPath                                                             :  rs =  "T_MaterialPath";              break;
        case    T_UniquePath                                                               :  rs =  "T_UniquePath";                break;
        case    T_GatherPath                                                               :  rs =  "T_GatherPath";                break;
        case    T_GatherMergePath                                                          :  rs =  "T_GatherMergePath";           break;
        case    T_ProjectionPath                                                           :  rs =  "T_ProjectionPath";            break;
        case    T_ProjectSetPath                                                           :  rs =  "T_ProjectSetPath";            break;
        case    T_SortPath                                                                 :  rs =  "T_SortPath";                  break;
        case    T_GroupPath                                                                :  rs =  "T_GroupPath";                 break;
        case    T_UpperUniquePath                                                          :  rs =  "T_UpperUniquePath";           break;
        case    T_AggPath                                                                  :  rs =  "T_AggPath";                   break;
        case    T_GroupingSetsPath                                                         :  rs =  "T_GroupingSetsPath";          break;
        case    T_MinMaxAggPath                                                            :  rs =  "T_MinMaxAggPath";             break;
        case    T_WindowAggPath                                                            :  rs =  "T_WindowAggPath";             break;
        case    T_SetOpPath                                                                :  rs =  "T_SetOpPath";                 break;
        case    T_RecursiveUnionPath                                                       :  rs =  "T_RecursiveUnionPath";        break;
        case    T_LockRowsPath                                                             :  rs =  "T_LockRowsPath";              break;
        case    T_ModifyTablePath                                                          :  rs =  "T_ModifyTablePath";           break;
        case    T_LimitPath                                                                :  rs =  "T_LimitPath";                 break;
                /* these aren't subclasses of Path: */
        case    T_EquivalenceClass                                                         :  rs =  "T_EquivalenceClass";          break;
        case    T_EquivalenceMember                                                        :  rs =  "T_EquivalenceMember";         break;
        case    T_PathKey                                                                  :  rs =  "T_PathKey";                   break;
        case    T_PathTarget                                                               :  rs =  "T_PathTarget";                break;
        case    T_RestrictInfo                                                             :  rs =  "T_RestrictInfo";              break;
        case    T_PlaceHolderVar                                                           :  rs =  "T_PlaceHolderVar";            break;
        case    T_SpecialJoinInfo                                                          :  rs =  "T_SpecialJoinInfo";           break;
        case    T_AppendRelInfo                                                            :  rs =  "T_AppendRelInfo";             break;
        case    T_PartitionedChildRelInfo                                                  :  rs =  "T_PartitionedChildRelInfo";   break;
        case    T_PlaceHolderInfo                                                          :  rs =  "T_PlaceHolderInfo";           break;
        case    T_MinMaxAggInfo                                                            :  rs =  "T_MinMaxAggInfo";             break;
        case    T_PlannerParamItem                                                         :  rs =  "T_PlannerParamItem";          break;
        case    T_RollupData                                                               :  rs =  "T_RollupData";                break;
        case    T_GroupingSetData                                                          :  rs =  "T_GroupingSetData";           break;
        case    T_StatisticExtInfo                                                         :  rs =  "T_StatisticExtInfo";          break;

                /*                                   
                 * TAGS FOR MEMORY NODES (memnodes.h)
                 */                                  
        case    T_MemoryContext                                                            :  rs =  "T_MemoryContext";             break;
        case    T_AllocSetContext                                                          :  rs =  "T_AllocSetContext";           break;
        case    T_SlabContext                                                              :  rs =  "T_SlabContext";               break;

                /*                              
                 * TAGS FOR VALUE NODES (value.h)
                 */                             
        case    T_Value                                                                    :  rs =  "T_Value";                     break;
        case    T_Integer                                                                  :  rs =  "T_Integer";                   break;
        case    T_Float                                                                    :  rs =  "T_Float";                     break;
        case    T_String                                                                   :  rs =  "T_String";                    break;
        case    T_BitString                                                                :  rs =  "T_BitString";                 break;
        case    T_Null                                                                     :  rs =  "T_Null";                      break;

                /*                                
                 * TAGS FOR LIST NODES (pg_list.h)
                 */                               
        case    T_List                                                                     :  rs =  "T_List";                      break;
        case    T_IntList                                                                  :  rs =  "T_IntList";                   break;
        case    T_OidList                                                                  :  rs =  "T_OidList";                   break;

                /*                                         
                 * TAGS FOR EXTENSIBLE NODES (extensible.h)
                 */                                        
        case    T_ExtensibleNode                                                           :  rs =  "T_ExtensibleNode";            break;

                /*                                                  
                 * TAGS FOR STATEMENT NODES (mostly in parsenodes.h)
                 */                                                 
        case    T_RawStmt                                                                  :  rs =  "T_RawStmt";                     break;
        case    T_Query                                                                    :  rs =  "T_Query";                       break;
        case    T_PlannedStmt                                                              :  rs =  "T_PlannedStmt";                 break;
        case    T_InsertStmt                                                               :  rs =  "T_InsertStmt";                  break;
        case    T_DeleteStmt                                                               :  rs =  "T_DeleteStmt";                  break;
        case    T_UpdateStmt                                                               :  rs =  "T_UpdateStmt";                  break;
        case    T_SelectStmt                                                               :  rs =  "T_SelectStmt";                  break;
        case    T_AlterTableStmt                                                           :  rs =  "T_AlterTableStmt";              break;
        case    T_AlterTableCmd                                                            :  rs =  "T_AlterTableCmd";               break;
        case    T_AlterDomainStmt                                                          :  rs =  "T_AlterDomainStmt";             break;
        case    T_SetOperationStmt                                                         :  rs =  "T_SetOperationStmt";            break;
        case    T_GrantStmt                                                                :  rs =  "T_GrantStmt";                   break;
        case    T_GrantRoleStmt                                                            :  rs =  "T_GrantRoleStmt";               break;
        case    T_AlterDefaultPrivilegesStmt                                               :  rs =  "T_AlterDefaultPrivilegesStmt";  break;
        case    T_ClosePortalStmt                                                          :  rs =  "T_ClosePortalStmt";             break;
        case    T_ClusterStmt                                                              :  rs =  "T_ClusterStmt";                 break;
        case    T_CopyStmt                                                                 :  rs =  "T_CopyStmt";                    break;
        case    T_CreateStmt                                                               :  rs =  "T_CreateStmt";                  break;
        case    T_DefineStmt                                                               :  rs =  "T_DefineStmt";                  break;
        case    T_DropStmt                                                                 :  rs =  "T_DropStmt";                    break;
        case    T_TruncateStmt                                                             :  rs =  "T_TruncateStmt";                break;
        case    T_CommentStmt                                                              :  rs =  "T_CommentStmt";                 break;
        case    T_FetchStmt                                                                :  rs =  "T_FetchStmt";                   break;
        case    T_IndexStmt                                                                :  rs =  "T_IndexStmt";                   break;
        case    T_CreateFunctionStmt                                                       :  rs =  "T_CreateFunctionStmt";          break;
        case    T_AlterFunctionStmt                                                        :  rs =  "T_AlterFunctionStmt";           break;
        case    T_DoStmt                                                                   :  rs =  "T_DoStmt";                      break;
        case    T_RenameStmt                                                               :  rs =  "T_RenameStmt";                  break;
        case    T_RuleStmt                                                                 :  rs =  "T_RuleStmt";                    break;
        case    T_NotifyStmt                                                               :  rs =  "T_NotifyStmt";                  break;
        case    T_ListenStmt                                                               :  rs =  "T_ListenStmt";                  break;
        case    T_UnlistenStmt                                                             :  rs =  "T_UnlistenStmt";                break;
        case    T_TransactionStmt                                                          :  rs =  "T_TransactionStmt";             break;
        case    T_ViewStmt                                                                 :  rs =  "T_ViewStmt";                    break;
        case    T_LoadStmt                                                                 :  rs =  "T_LoadStmt";                    break;
        case    T_CreateDomainStmt                                                         :  rs =  "T_CreateDomainStmt";            break;
        case    T_CreatedbStmt                                                             :  rs =  "T_CreatedbStmt";                break;
        case    T_DropdbStmt                                                               :  rs =  "T_DropdbStmt";                  break;
        case    T_VacuumStmt                                                               :  rs =  "T_VacuumStmt";                  break;
        case    T_ExplainStmt                                                              :  rs =  "T_ExplainStmt";                 break;
        case    T_CreateTableAsStmt                                                        :  rs =  "T_CreateTableAsStmt";           break;
        case    T_CreateSeqStmt                                                            :  rs =  "T_CreateSeqStmt";               break;
        case    T_AlterSeqStmt                                                             :  rs =  "T_AlterSeqStmt";                break;
        case    T_VariableSetStmt                                                          :  rs =  "T_VariableSetStmt";             break;
        case    T_VariableShowStmt                                                         :  rs =  "T_VariableShowStmt";            break;
        case    T_DiscardStmt                                                              :  rs =  "T_DiscardStmt";                 break;
        case    T_CreateTrigStmt                                                           :  rs =  "T_CreateTrigStmt";              break;
        case    T_CreatePLangStmt                                                          :  rs =  "T_CreatePLangStmt";             break;
        case    T_CreateRoleStmt                                                           :  rs =  "T_CreateRoleStmt";              break;
        case    T_AlterRoleStmt                                                            :  rs =  "T_AlterRoleStmt";               break;
        case    T_DropRoleStmt                                                             :  rs =  "T_DropRoleStmt";                break;
        case    T_LockStmt                                                                 :  rs =  "T_LockStmt";                    break;
        case    T_ConstraintsSetStmt                                                       :  rs =  "T_ConstraintsSetStmt";          break;
        case    T_ReindexStmt                                                              :  rs =  "T_ReindexStmt";                 break;
        case    T_CheckPointStmt                                                           :  rs =  "T_CheckPointStmt";              break;
        case    T_CreateSchemaStmt                                                         :  rs =  "T_CreateSchemaStmt";            break;
        case    T_AlterDatabaseStmt                                                        :  rs =  "T_AlterDatabaseStmt";           break;
        case    T_AlterDatabaseSetStmt                                                     :  rs =  "T_AlterDatabaseSetStmt";        break;
        case    T_AlterRoleSetStmt                                                         :  rs =  "T_AlterRoleSetStmt";            break;
        case    T_CreateConversionStmt                                                     :  rs =  "T_CreateConversionStmt";        break;
        case    T_CreateCastStmt                                                           :  rs =  "T_CreateCastStmt";              break;
        case    T_CreateOpClassStmt                                                        :  rs =  "T_CreateOpClassStmt";           break;
        case    T_CreateOpFamilyStmt                                                       :  rs =  "T_CreateOpFamilyStmt";          break;
        case    T_AlterOpFamilyStmt                                                        :  rs =  "T_AlterOpFamilyStmt";           break;
        case    T_PrepareStmt                                                              :  rs =  "T_PrepareStmt";                 break;
        case    T_ExecuteStmt                                                              :  rs =  "T_ExecuteStmt";                 break;
        case    T_DeallocateStmt                                                           :  rs =  "T_DeallocateStmt";              break;
        case    T_DeclareCursorStmt                                                        :  rs =  "T_DeclareCursorStmt";           break;
        case    T_CreateTableSpaceStmt                                                     :  rs =  "T_CreateTableSpaceStmt";        break;
        case    T_DropTableSpaceStmt                                                       :  rs =  "T_DropTableSpaceStmt";          break;
        case    T_AlterObjectDependsStmt                                                   :  rs =  "T_AlterObjectDependsStmt";      break;
        case    T_AlterObjectSchemaStmt                                                    :  rs =  "T_AlterObjectSchemaStmt";       break;
        case    T_AlterOwnerStmt                                                           :  rs =  "T_AlterOwnerStmt";              break;
        case    T_AlterOperatorStmt                                                        :  rs =  "T_AlterOperatorStmt";           break;
        case    T_DropOwnedStmt                                                            :  rs =  "T_DropOwnedStmt";               break;
        case    T_ReassignOwnedStmt                                                        :  rs =  "T_ReassignOwnedStmt";           break;
        case    T_CompositeTypeStmt                                                        :  rs =  "T_CompositeTypeStmt";           break;
        case    T_CreateEnumStmt                                                           :  rs =  "T_CreateEnumStmt";              break;
        case    T_CreateRangeStmt                                                          :  rs =  "T_CreateRangeStmt";             break;
        case    T_AlterEnumStmt                                                            :  rs =  "T_AlterEnumStmt";               break;
        case    T_AlterTSDictionaryStmt                                                    :  rs =  "T_AlterTSDictionaryStmt";       break;
        case    T_AlterTSConfigurationStmt                                                 :  rs =  "T_AlterTSConfigurationStmt";    break;
        case    T_CreateFdwStmt                                                            :  rs =  "T_CreateFdwStmt";               break;
        case    T_AlterFdwStmt                                                             :  rs =  "T_AlterFdwStmt";                break;
        case    T_CreateForeignServerStmt                                                  :  rs =  "T_CreateForeignServerStmt";     break;
        case    T_AlterForeignServerStmt                                                   :  rs =  "T_AlterForeignServerStmt";      break;
        case    T_CreateUserMappingStmt                                                    :  rs =  "T_CreateUserMappingStmt";       break;
        case    T_AlterUserMappingStmt                                                     :  rs =  "T_AlterUserMappingStmt";        break;
        case    T_DropUserMappingStmt                                                      :  rs =  "T_DropUserMappingStmt";         break;
        case    T_AlterTableSpaceOptionsStmt                                               :  rs =  "T_AlterTableSpaceOptionsStmt";  break;
        case    T_AlterTableMoveAllStmt                                                    :  rs =  "T_AlterTableMoveAllStmt";       break;
        case    T_SecLabelStmt                                                             :  rs =  "T_SecLabelStmt";                break;
        case    T_CreateForeignTableStmt                                                   :  rs =  "T_CreateForeignTableStmt";      break;
        case    T_ImportForeignSchemaStmt                                                  :  rs =  "T_ImportForeignSchemaStmt";     break;
        case    T_CreateExtensionStmt                                                      :  rs =  "T_CreateExtensionStmt";         break;
        case    T_AlterExtensionStmt                                                       :  rs =  "T_AlterExtensionStmt";          break;
        case    T_AlterExtensionContentsStmt                                               :  rs =  "T_AlterExtensionContentsStmt";  break;
        case    T_CreateEventTrigStmt                                                      :  rs =  "T_CreateEventTrigStmt";         break;
        case    T_AlterEventTrigStmt                                                       :  rs =  "T_AlterEventTrigStmt";          break;
        case    T_RefreshMatViewStmt                                                       :  rs =  "T_RefreshMatViewStmt";          break;
        case    T_ReplicaIdentityStmt                                                      :  rs =  "T_ReplicaIdentityStmt";         break;
        case    T_AlterSystemStmt                                                          :  rs =  "T_AlterSystemStmt";             break;
        case    T_CreatePolicyStmt                                                         :  rs =  "T_CreatePolicyStmt";            break;
        case    T_AlterPolicyStmt                                                          :  rs =  "T_AlterPolicyStmt";             break;
        case    T_CreateTransformStmt                                                      :  rs =  "T_CreateTransformStmt";         break;
        case    T_CreateAmStmt                                                             :  rs =  "T_CreateAmStmt";                break;
        case    T_CreatePublicationStmt                                                    :  rs =  "T_CreatePublicationStmt";       break;
        case    T_AlterPublicationStmt                                                     :  rs =  "T_AlterPublicationStmt";        break;
        case    T_CreateSubscriptionStmt                                                   :  rs =  "T_CreateSubscriptionStmt";      break;
        case    T_AlterSubscriptionStmt                                                    :  rs =  "T_AlterSubscriptionStmt";       break;
        case    T_DropSubscriptionStmt                                                     :  rs =  "T_DropSubscriptionStmt";        break;
        case    T_CreateStatsStmt                                                          :  rs =  "T_CreateStatsStmt";             break;
        case    T_AlterCollationStmt                                                       :  rs =  "T_AlterCollationStmt";          break;

                /*                                         
                 * TAGS FOR PARSE TREE NODES (parsenodes.h)
                 */                                        
        case    T_A_Expr                                                                   :  rs =  "T_A_Expr";                      break;
        case    T_ColumnRef                                                                :  rs =  "T_ColumnRef";                   break;
        case    T_ParamRef                                                                 :  rs =  "T_ParamRef";                    break;
        case    T_A_Const                                                                  :  rs =  "T_A_Const";                     break;
        case    T_FuncCall                                                                 :  rs =  "T_FuncCall";                    break;
        case    T_A_Star                                                                   :  rs =  "T_A_Star";                      break;
        case    T_A_Indices                                                                :  rs =  "T_A_Indices";                   break;
        case    T_A_Indirection                                                            :  rs =  "T_A_Indirection";               break;
        case    T_A_ArrayExpr                                                              :  rs =  "T_A_ArrayExpr";                 break;
        case    T_ResTarget                                                                :  rs =  "T_ResTarget";                   break;
        case    T_MultiAssignRef                                                           :  rs =  "T_MultiAssignRef";              break;
        case    T_TypeCast                                                                 :  rs =  "T_TypeCast";                    break;
        case    T_CollateClause                                                            :  rs =  "T_CollateClause";               break;
        case    T_SortBy                                                                   :  rs =  "T_SortBy";                      break;
        case    T_WindowDef                                                                :  rs =  "T_WindowDef";                   break;
        case    T_RangeSubselect                                                           :  rs =  "T_RangeSubselect";              break;
        case    T_RangeFunction                                                            :  rs =  "T_RangeFunction";               break;
        case    T_RangeTableSample                                                         :  rs =  "T_RangeTableSample";            break;
        case    T_RangeTableFunc                                                           :  rs =  "T_RangeTableFunc";              break;
        case    T_RangeTableFuncCol                                                        :  rs =  "T_RangeTableFuncCol";           break;
        case    T_TypeName                                                                 :  rs =  "T_TypeName";                    break;
        case    T_ColumnDef                                                                :  rs =  "T_ColumnDef";                   break;
        case    T_IndexElem                                                                :  rs =  "T_IndexElem";                   break;
        case    T_Constraint                                                               :  rs =  "T_Constraint";                  break;
        case    T_DefElem                                                                  :  rs =  "T_DefElem";                     break;
        case    T_RangeTblEntry                                                            :  rs =  "T_RangeTblEntry";               break;
        case    T_RangeTblFunction                                                         :  rs =  "T_RangeTblFunction";            break;
        case    T_TableSampleClause                                                        :  rs =  "T_TableSampleClause";           break;
        case    T_WithCheckOption                                                          :  rs =  "T_WithCheckOption";             break;
        case    T_SortGroupClause                                                          :  rs =  "T_SortGroupClause";             break;
        case    T_GroupingSet                                                              :  rs =  "T_GroupingSet";                 break;
        case    T_WindowClause                                                             :  rs =  "T_WindowClause";                break;
        case    T_ObjectWithArgs                                                           :  rs =  "T_ObjectWithArgs";              break;
        case    T_AccessPriv                                                               :  rs =  "T_AccessPriv";                  break;
        case    T_CreateOpClassItem                                                        :  rs =  "T_CreateOpClassItem";           break;
        case    T_TableLikeClause                                                          :  rs =  "T_TableLikeClause";             break;
        case    T_FunctionParameter                                                        :  rs =  "T_FunctionParameter";           break;
        case    T_LockingClause                                                            :  rs =  "T_LockingClause";               break;
        case    T_RowMarkClause                                                            :  rs =  "T_RowMarkClause";               break;
        case    T_XmlSerialize                                                             :  rs =  "T_XmlSerialize";                break;
        case    T_WithClause                                                               :  rs =  "T_WithClause";                  break;
        case    T_InferClause                                                              :  rs =  "T_InferClause";                 break;
        case    T_OnConflictClause                                                         :  rs =  "T_OnConflictClause";            break;
        case    T_CommonTableExpr                                                          :  rs =  "T_CommonTableExpr";             break;
        case    T_RoleSpec                                                                 :  rs =  "T_RoleSpec";                    break;
        case    T_TriggerTransition                                                        :  rs =  "T_TriggerTransition";           break;
        case    T_PartitionElem                                                            :  rs =  "T_PartitionElem";               break;
        case    T_PartitionSpec                                                            :  rs =  "T_PartitionSpec";               break;
        case    T_PartitionBoundSpec                                                       :  rs =  "T_PartitionBoundSpec";          break;
        case    T_PartitionRangeDatum                                                      :  rs =  "T_PartitionRangeDatum";         break;
        case    T_PartitionCmd                                                             :  rs =  "T_PartitionCmd";                break;

                /*                                                       
                 * TAGS FOR REPLICATION GRAMMAR PARSE NODES (replnodes.h)
                 */                                                      
        case    T_IdentifySystemCmd                                                        :  rs =  "T_IdentifySystemCmd";           break;
        case    T_BaseBackupCmd                                                            :  rs =  "T_BaseBackupCmd";               break;
        case    T_CreateReplicationSlotCmd                                                 :  rs =  "T_CreateReplicationSlotCmd";    break;
        case    T_DropReplicationSlotCmd                                                   :  rs =  "T_DropReplicationSlotCmd";      break;
        case    T_StartReplicationCmd                                                      :  rs =  "T_StartReplicationCmd";         break;
        case    T_TimeLineHistoryCmd                                                       :  rs =  "T_TimeLineHistoryCmd";          break;
        case    T_SQLCmd                                                                   :  rs =  "T_SQLCmd";                      break;

                /*                                                                     
                 * TAGS FOR RANDOM OTHER STUFF                                         
                 *                                                                     
                 * These are objects that aren't part of parse/plan/execute node tree  
                 * structures but we give them NodeTags anyway for identification     
                 * purposes (usually because they are involved in APIs where we want to
                 * pass multiple object types through the same pointer).               
                 */                                                                    
        case    T_TriggerData              /* in commands/trigger.h */                     :  rs =  "T_TriggerData";                 break;
        case    T_EventTriggerData         /* in commands/event_trigger.h */               :  rs =  "T_EventTriggerData";            break;
        case    T_ReturnSetInfo            /* in nodes/execnodes.h */                      :  rs =  "T_ReturnSetInfo";               break;
        case    T_WindowObjectData         /* private in nodeWindowAgg.c */                :  rs =  "T_WindowObjectData";            break;
        case    T_TIDBitmap                /* in nodes/tidbitmap.h */                      :  rs =  "T_TIDBitmap";                   break;
        case    T_InlineCodeBlock          /* in nodes/parsenodes.h */                     :  rs =  "T_InlineCodeBlock";             break;
        case    T_FdwRoutine               /* in foreign/fdwapi.h */                       :  rs =  "T_FdwRoutine";                  break;
        case    T_IndexAmRoutine           /* in access/amapi.h */                         :  rs =  "T_IndexAmRoutine";              break;
        case    T_TsmRoutine               /* in access/tsmapi.h */                        :  rs =  "T_TsmRoutine";                  break;
        case    T_ForeignKeyCacheInfo       /* in utils/rel.h */                           :  rs =  "T_ForeignKeyCacheInfo";         break;

    }
    return rs;
}


std::string text_enum(Oid val)
{
    const char *rs = "NA";
    switch(val)
    {
        case BOOLOID                :  rs = "BOOLOID"              ; break;
        case BYTEAOID               :  rs = "BYTEAOID"             ; break;
        case CHAROID                :  rs = "CHAROID"              ; break;
        case NAMEOID                :  rs = "NAMEOID"              ; break;
        case INT8OID                :  rs = "INT8OID"              ; break;
        case INT2OID                :  rs = "INT2OID"              ; break;
        case INT2VECTOROID          :  rs = "INT2VECTOROID"        ; break;
        case INT4OID                :  rs = "INT4OID"              ; break;
        case REGPROCOID             :  rs = "REGPROCOID"           ; break;
        case TEXTOID                :  rs = "TEXTOID"              ; break;
        case OIDOID                 :  rs = "OIDOID"               ; break;
        case TIDOID                 :  rs = "TIDOID"               ; break;
        case XIDOID                 :  rs = "XIDOID"               ; break;
        case CIDOID                 :  rs = "CIDOID"               ; break;
        case OIDVECTOROID           :  rs = "OIDVECTOROID"         ; break;
        case JSONOID                :  rs = "JSONOID"              ; break;
        case XMLOID                 :  rs = "XMLOID"               ; break;
        case PGNODETREEOID          :  rs = "PGNODETREEOID"        ; break;
        case PGNDISTINCTOID         :  rs = "PGNDISTINCTOID"       ; break;
        case PGDEPENDENCIESOID      :  rs = "PGDEPENDENCIESOID"    ; break;
        case PGDDLCOMMANDOID        :  rs = "PGDDLCOMMANDOID"      ; break;
        case POINTOID               :  rs = "POINTOID"             ; break;
        case LSEGOID                :  rs = "LSEGOID"              ; break;
        case PATHOID                :  rs = "PATHOID"              ; break;
        case BOXOID                 :  rs = "BOXOID"               ; break;
        case POLYGONOID             :  rs = "POLYGONOID"           ; break;
        case LINEOID                :  rs = "LINEOID"              ; break;
        case FLOAT4OID              :  rs = "FLOAT4OID"            ; break;
        case FLOAT8OID              :  rs = "FLOAT8OID"            ; break;
        case ABSTIMEOID             :  rs = "ABSTIMEOID"           ; break;
        case RELTIMEOID             :  rs = "RELTIMEOID"           ; break;
        case TINTERVALOID           :  rs = "TINTERVALOID"         ; break;
        case UNKNOWNOID             :  rs = "UNKNOWNOID"           ; break;
        case CIRCLEOID              :  rs = "CIRCLEOID"            ; break;
        case CASHOID                :  rs = "CASHOID"              ; break;
        case MACADDROID             :  rs = "MACADDROID"           ; break;
        case INETOID                :  rs = "INETOID"              ; break;
        case CIDROID                :  rs = "CIDROID"              ; break;
        case MACADDR8OID            :  rs = "MACADDR8OID"          ; break;
        case INT2ARRAYOID           :  rs = "INT2ARRAYOID"         ; break;
        case INT4ARRAYOID           :  rs = "INT4ARRAYOID"         ; break;
        case TEXTARRAYOID           :  rs = "TEXTARRAYOID"         ; break;
        case OIDARRAYOID            :  rs = "OIDARRAYOID"          ; break;
        case FLOAT4ARRAYOID         :  rs = "FLOAT4ARRAYOID"       ; break;
        case ACLITEMOID             :  rs = "ACLITEMOID"           ; break;
        case CSTRINGARRAYOID        :  rs = "CSTRINGARRAYOID"      ; break;
        case BPCHAROID              :  rs = "BPCHAROID"            ; break;
        case VARCHAROID             :  rs = "VARCHAROID"           ; break;
        case DATEOID                :  rs = "DATEOID"              ; break;
        case TIMEOID                :  rs = "TIMEOID"              ; break;
        case TIMESTAMPOID           :  rs = "TIMESTAMPOID"         ; break;
        case TIMESTAMPTZOID         :  rs = "TIMESTAMPTZOID"       ; break;
        case INTERVALOID            :  rs = "INTERVALOID"          ; break;
        case TIMETZOID              :  rs = "TIMETZOID"            ; break;
        case BITOID                 :  rs = "BITOID"               ; break;
        case VARBITOID              :  rs = "VARBITOID"            ; break;
        case NUMERICOID             :  rs = "NUMERICOID"           ; break;
        case REFCURSOROID           :  rs = "REFCURSOROID"         ; break;
        case REGPROCEDUREOID        :  rs = "REGPROCEDUREOID"      ; break;
        case REGOPEROID             :  rs = "REGOPEROID"           ; break;
        case REGOPERATOROID         :  rs = "REGOPERATOROID"       ; break;
        case REGCLASSOID            :  rs = "REGCLASSOID"          ; break;
        case REGTYPEOID             :  rs = "REGTYPEOID"           ; break;
        case REGROLEOID             :  rs = "REGROLEOID"           ; break;
        case REGNAMESPACEOID        :  rs = "REGNAMESPACEOID"      ; break;
        case REGTYPEARRAYOID        :  rs = "REGTYPEARRAYOID"      ; break;
        case UUIDOID                :  rs = "UUIDOID"              ; break;
        case LSNOID                 :  rs = "LSNOID"               ; break;
        case TSVECTOROID            :  rs = "TSVECTOROID"          ; break;
        case GTSVECTOROID           :  rs = "GTSVECTOROID"         ; break;
        case TSQUERYOID             :  rs = "TSQUERYOID"           ; break;
        case REGCONFIGOID           :  rs = "REGCONFIGOID"         ; break;
        case REGDICTIONARYOID       :  rs = "REGDICTIONARYOID"     ; break;
        case JSONBOID               :  rs = "JSONBOID"             ; break;
        case INT4RANGEOID           :  rs = "INT4RANGEOID"         ; break;
        case RECORDOID              :  rs = "RECORDOID"            ; break;
        case RECORDARRAYOID         :  rs = "RECORDARRAYOID"       ; break;
        case CSTRINGOID             :  rs = "CSTRINGOID"           ; break;
        case ANYOID                 :  rs = "ANYOID"               ; break;
        case ANYARRAYOID            :  rs = "ANYARRAYOID"          ; break;
        case VOIDOID                :  rs = "VOIDOID"              ; break;
        case TRIGGEROID             :  rs = "TRIGGEROID"           ; break;
        case EVTTRIGGEROID          :  rs = "EVTTRIGGEROID"        ; break;
        case LANGUAGE_HANDLEROID    :  rs = "LANGUAGE_HANDLEROID"  ; break;
        case INTERNALOID            :  rs = "INTERNALOID"          ; break;
        case OPAQUEOID              :  rs = "OPAQUEOID"            ; break;
        case ANYELEMENTOID          :  rs = "ANYELEMENTOID"        ; break;
        case ANYNONARRAYOID         :  rs = "ANYNONARRAYOID"       ; break;
        case ANYENUMOID             :  rs = "ANYENUMOID"           ; break;
        case FDW_HANDLEROID         :  rs = "FDW_HANDLEROID"       ; break;
        case INDEX_AM_HANDLEROID    :  rs = "INDEX_AM_HANDLEROID"  ; break;
        case TSM_HANDLEROID         :  rs = "TSM_HANDLEROID"       ; break;
        case ANYRANGEOID            :  rs = "ANYRANGEOID"          ; break;
    }
    
    return std::move(std::string(rs));
}



std::string text_enum(TupleTableSlot *slot)
{
    std::stringstream rs;
    
    if (slot->tts_isempty)
    {
        rs << "Tuple is empty";
        return std::move(rs.str());
    }

    TupleDesc tupdesc = slot->tts_tupleDescriptor;

    //  Make sure the tuple is fully deconstructed
    slot_getallattrs(slot);

    for (int i = 0; i < tupdesc->natts; ++i)
    {
        rs << std::to_string(i) << ". ";
        
        Form_pg_attribute attr = tupdesc->attrs[i];
        if (slot->tts_isnull[i])
        {
            rs << "isnull" << std::endl;
            continue;
        }

        Datum value = slot->tts_values[i];

        // We can't call the regular type output functions here because we
        // might not have catalog access.  Instead, we must hard-wire
        // knowledge of the required types.
        
        rs << text_enum(attr->atttypid) << ": " << NameStr(attr->attname) << " = ";
        switch (attr->atttypid)
        {
            case TEXTOID:
            {
                text *t = DatumGetTextPP(value);
                rs << VARDATA_ANY(t);
            }
            break;

            case INT4OID:
            {
                int32 num = DatumGetInt32(value);
                rs << num;
            }
            break;

            case INT8OID:
            {
                int64 num = DatumGetInt64(value);
                rs << num;
            }
            break;

            default:
                rs << "novalue ???";
                elog(ERROR, "unsupported type OID: %u", attr->atttypid);
        }
        rs << std::endl;
    }
    
    return std::move(rs.str());
}

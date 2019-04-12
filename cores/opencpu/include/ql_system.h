/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Quectel Co., Ltd. 2013
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   ql_system.h 
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *  System related APIs
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 *----------------------------------------------------------------------------
 * 
 ****************************************************************************/
 

#ifndef __QL_SYSTEM_H__
#define __QL_SYSTEM_H__
#include "ql_common.h"
#include "ql_type.h"
#include "ql_memory.h"


/**************************************************************
 * Message Structure Definition
 **************************************************************/
typedef struct {
    u32  message;
    u32  param1;
    u32  param2;
    u32  srcTaskId;
} ST_MSG;

/**************************************************************
 * User Message ID Definition
 **************************************************************/
#define     MSG_ID_USER_START       0x1000
#define     MSG_ID_RIL_READY        (MSG_ID_USER_START + 1)
#define     MSG_ID_URC_INDICATION   (MSG_ID_USER_START + 2)


typedef enum {
	EVENT_FLAG0 = 0x00000001,
	EVENT_FLAG1 = 0x00000002,
	EVENT_FLAG2 = 0x00000004,
	EVENT_FLAG3 = 0x00000008,
	EVENT_FLAG4 = 0x00000010,
	EVENT_FLAG5 = 0x00000020,
	EVENT_FLAG6 = 0x00000040,
	EVENT_FLAG7 = 0x00000080,
	EVENT_FLAG8 = 0x00000100,
	EVENT_FLAG9 = 0x00000200,
	EVENT_FLAG_END
}Enum_EventFlag;

/**************************************************************
 * Error Code Definition
 **************************************************************/
typedef enum {
    OS_SUCCESS,
    OS_ERROR,
    OS_Q_FULL,
    OS_Q_EMPTY,
    OS_SEM_NOT_AVAILABLE,
    OS_WOULD_BLOCK,
    OS_MESSAGE_TOO_BIG,
    OS_INVALID_ID,
    OS_NOT_INITIALIZED,
    OS_INVALID_LENGHT,
    OS_NULL_ADDRESS,
    OS_NOT_RECEIVE,
    OS_NOT_SEND,
    OS_MEMORY_NOT_VALID,
    OS_NOT_PRESENT,
    OS_MEMORY_NOT_RELEASE
} Enum_OS_ErrCode;

typedef enum {
    SYS_STATE_START = 0,
    SYS_STATE_ATOK  = 1,
    SYS_STATE_PHBOK = 2,
    SYS_STATE_SMSOK = 3
}Enum_SysInitState;

typedef enum {
    CFUN_STATE_0 = 0,
    CFUN_STATE_1 = 1,
    CFUN_STATE_4 = 4
}Enum_CfunState;

typedef enum
{
    INVAID_EXCEPTION                =  0,
    UNDEF_EXCEPTION                 =  1,
    SWI_EXCEPTION                   =  2,
    PREFETCH_EXCEPTION              =  3,
    DATAABORT_EXCEPTION             =  4,
    ASSERT_FAIL_EXCEPTION           =  5,
    SYS_FATALERR_EXT_TASK_EXCEPTION =  6,
    SYS_FATALERR_EXT_BUF_EXCEPTION  =  7,
    SYS_LOCKUP                      =  8,
    ASSERT_DUMP_EXTENDED_RECORD     =  9,
    ASSERT_FAIL_NATIVE              = 10,
    STACKACCESS_EXCEPTION           = 11,
    NUM_EXCEPTION
} exception_type;

typedef enum
{
    Healthy = 0,
    InternalSRAMCorrupted = 1,
    SystemStackCorrupted = 2,
    TaskStackCorrupted = 3,
    HISRStackCorrupted = 4,
    VectorTableCorrupted = 5,
    AliceCorrupted = 6
} EX_DIAGNOSIS_T;


/*******************************************************************************
 * Data Structure Definition - Common
 *******************************************************************************/


#define EX_STACK_DUMP_LEN 10
#define EX_QUEUE_TRACK 20
#define EX_UNIT_NAME_LEN 8
#define EX_TIMING_CHECK_LEN 6

/* Exception header, used to track the exception type (4Bytes) */
typedef struct ex_exception_record_header_t
{
    exception_type ex_type;      /* offset: +0x0, length: 1 */
    u8 ex_nvram;          /* offset: +0x1, length: 1 */
    u16 ex_serial_num;    /* offset: +0x2, length: 2 */
} EX_HEADER_T;

/* Duplicate structure definition to meet different platform requirement */
typedef struct _ex_rtc_struct
{
    u8 rtc_sec;    /* offset: +0x11, length: 1 */  /* seconds after the minute */
    u8 rtc_min;    /* offset: +0x12, length: 1 */  /* minutes after the hour */
    u8 rtc_hour;   /* offset: +0x13, length: 1 */  /* hours after the midnight */
    u8 rtc_day;    /* offset: +0x14, length: 1 */  /* day of the month */
    u8 rtc_mon;    /* offset: +0x15, length: 1 */  /* months */
    u8 rtc_wday;   /* offset: +0x16, length: 1 */  /* days in a week */
    u8 rtc_year;   /* offset: +0x17, length: 1 */  /* years */
    u8 rtc_pad;    /* offset: +0x18, length: 1 */  /* Padding */
} ex_rtc_struct;

/* boot mode enumeration */
typedef enum
{
    /* factory mode, system boot up to communicate with factory tools */
    FACTORY_BOOT = 0,
    /* normal mode */
    NORMAL_BOOT = 1,
    /* USB mode, system boot up when USB charger connect */
    USBMS_BOOT =  2,
    /* Firmware Update Engine mode, system run inside FUE */
    FUE_BOOT =  3,
    /* number of known boot mode */
    NUM_OF_BOOT_MODE,
    /* unknown mode */
    UNKNOWN_BOOT_MODE = 0xff
} boot_mode_type;

/* Environment information */
typedef struct ex_environment_info_t
{
    boot_mode_type boot_mode;                      /* offset: +0x10, length: 1 */
    ex_rtc_struct rtc;                             /* offset: +0x11, length: 8 */
    u8 execution_unit[EX_UNIT_NAME_LEN];     /* offset: +0x19, length: 8 */
    u8 status;                              /* offset: +0x21, length: 1 */
    u8 pad[2];                              /* offset: +0x22, length: 2 */
    u32 stack_ptr;                          /* offset: +0x24, length: 4 */
    u32 stack_dump[EX_STACK_DUMP_LEN];      /* offset: +0x28, length: 40 */
    u16 ext_queue_pending_cnt;              /* offset: +0x50, length: 2 */
    u16 interrupt_mask3;                    /* offset: +0x52, length: 2 */
    u32 ext_queue_pending[EX_QUEUE_TRACK];  /* offset: +0x54, length: 80 */
    u32 interrupt_mask[2];                  /* offset: +0xA4, length: 8 */
    u32 processing_lisr;                    /* offset: +0xAC, length: 4 */
    u32 lr;                                 /* offset: +0xB0, length: 4 */
} EX_ENVINFO_T;

/* Diagnosis information  */
typedef struct ex_diagnosis_info_t
{
    EX_DIAGNOSIS_T diagnosis;                      /* offset: +0xB4, length: 1 */
    u8 owner[EX_UNIT_NAME_LEN];              /* offset: +0xB5, length: 8 */
    u8 pad[3];                              /* offset: +0xBD, length: 3 */
    u32 timing_check[EX_TIMING_CHECK_LEN];  /* offset: +0xC0, length: 24 */
} EX_DIAGNOSISINFO_T;


/*******************************************************************************
 * Constant Definition - Common
 *******************************************************************************/

#define EX_LOG_SIZE 512
#define EX_HEADER_SIZE sizeof(EX_HEADER_T)
#define EX_SWVER_LEN 12
#define EX_ENVINFO_SIZE sizeof(EX_ENVINFO_T)
#define EX_DIAGINFO_SIZE sizeof(EX_DIAGNOSISINFO_T)
#define EX_GUARD_LEN 4
#define EX_FORCEMEMORYDUMP 0x26409001
#define TOTAL_EXPTR_SIZE EX_LOG_SIZE
#define EX_NATIVE_ASSERT_ID 0x20110410


/*******************************************************************************
 * Data Structure Definition - Fatal Error in general
 *******************************************************************************/

#define EX_FATALERR_DESCRIPTION_PARAM_LEN 16
#define EX_FATALERR_ANALYSIS_PARAM_LEN 48
#define EX_FATALERR_GUIDELINE_PARAM_LEN 16

/* Special for Fatal Error only! (100Bytes) */
typedef struct ex_fatalerror_code_t
{
    u32 code1;
    u32 code2;
} EX_FATALERR_CODE_T;

typedef struct ex_description_t
{
    u32 trace;
    u8 param[EX_FATALERR_DESCRIPTION_PARAM_LEN];
} EX_DESCRIPTION_T;

typedef struct ex_analysis_t
{
    u32 trace;
    u8 param[EX_FATALERR_ANALYSIS_PARAM_LEN];
} EX_ANALYSIS_T;

typedef struct ex_guideline_t
{
    u32 trace;
    u8 param[EX_FATALERR_GUIDELINE_PARAM_LEN];
} EX_GUIDELINE_T;


/*******************************************************************************
 * Data Structure Definition - Fatal Error extended
 *******************************************************************************/

typedef struct
{
    u8 ex_his_owner[8];   /* control buffer owner */
    u8 ex_his_source[16]; /* source file */
    u32 ex_his_line; /* line number */
    u32 ex_his_count;    /* number of identical entries */
} EX_CTRLBUFF_HISTORY_T;

typedef struct
{
    u32 ex_buf_RTOS_header1;  /* NUCLEUS overhead 1, 0: allocated, else next pointer */
    u32 ex_buf_RTOS_header2;  /* NUCLEUS overhead 2, pointer to its control block */
    u32 ex_buf_KAL_header1; /* KAL overhead 1, header (0xF1F1F1F1) */
    u32 ex_buf_KAL_header2; /* KAL overhead 2, task ID */
    u32 ex_buf_KAL_header3; /* KAL overhead 3, pointer to its control block) */
    u32 ex_buf_poolID;      /* Buffer pointer */
    u32 ex_buf_KAL_footer1; /* KAL footer: 0xF2F2F2F2 */
    u32 ex_buf_KAL_footer2; /* KAL footer appended after size requested */
} EX_CTRLBUFF_COMMON_T;

typedef struct
{
    u8 ex_buf_source[12];  /* Source file name */
    u32 ex_buf_line;        /* line number */
} EX_CTRLBUFF_OWNER_T;

typedef union
{
    EX_CTRLBUFF_HISTORY_T history; /* length: 32 */
    EX_CTRLBUFF_COMMON_T common;   /* length: 32 */
} EX_CTRLBUFF_INFO_T;

typedef struct
{
    u32 ex_ctrlbuf_size;           /* offset: +0x13C, length: 4 */   /* control buffer size per entry */
    u32 ex_ctrlbuf_num;            /* offset: +0x140, length: 4 */   /* total number of entries */
    EX_CTRLBUFF_INFO_T ex_ctrlbuf_top;    /* offset: +0x144, length: 32 */   /* top occupation history node */
    EX_CTRLBUFF_INFO_T ex_ctrlbuf_second; /* offset: +0x164, length: 32 */   /* second occupation history node */
    EX_CTRLBUFF_INFO_T ex_ctrlbuf_third;  /* offset: +0x184, length: 32 */   /* third occupation history node */
    EX_CTRLBUFF_OWNER_T ex_monitor[3];    /* offset: +0x1A4, length: 48 */
    u32 ex_reserved[2];            /* offset: +0x1D4, length: 16 */   /* reserved */
} EX_CTRLBUFF_T;

typedef struct
{
    u8 ex_q_src_mod;        /* source module ID */
    u8 ex_q_count;          /* total number of identical message */
    u16 ex_q_msg_id;         /* message ID */
    u16 ex_q_cur_mes_no;     /* tatal number of messages left in queue */
    u16 ex_q_config_entry;   /* total number of entries */
} EX_QUEUE_T;

typedef struct
{
    u8 ex_task_name[8];       /* task name */
    u8 ex_task_stack_gp[8];   /* guard pattern:STACK_END */
    u32 ex_task_cur_status;    /* task current status, eg. RUNNING, READY etc */
    EX_QUEUE_T ex_task_external_q;    /* task external queue */
    EX_QUEUE_T ex_task_internal_q;    /* task internal queue */
    u32 ex_reserved;           /* reserved */
} EX_TASKINFO_T;

typedef struct
{
    u32 cpsr;     /* offset: +0x13C, length: 4 */
    u32 reg[16];  /* offset: +0x140, length: 64 */
} EX_CPU_REG_T;

/*******************************************************************************
 * Constant Definition and Exported Type - Fatal Error
 *******************************************************************************/

#define EX_MAX_TASK_DUMP 4

typedef struct ex_fatalerror_t
{
    EX_FATALERR_CODE_T error_code;                 /* offset: +0xD8, length: 8 */
    EX_DESCRIPTION_T description;                  /* offset: +0xE0, length: 20 */
    EX_ANALYSIS_T analysis;                        /* offset: +0xF4, length: 52 */
    EX_GUIDELINE_T guideline;                      /* offset: +0x128, length: 20 */
    union
    {
        EX_CTRLBUFF_T ctrl_buff;                   /* offset: +0x13C, length: 136 */
        EX_TASKINFO_T task_info[EX_MAX_TASK_DUMP]; /* offset: +0x13C, length: 160 */
        EX_CPU_REG_T cpu_reg;                      /* offset: +0x13C, length: 68 */
    } info;
} EX_FATALERR_T;


/*******************************************************************************
 * Constant Definition and Exported Type - Assert Failure
 *******************************************************************************/

#define EX_ASSERTFAIL_FILENAME_LEN 24
#define EX_ASSERTFAIL_SIZE EX_ASSERTFAIL_FILENAME_LEN + \
                           sizeof(u32) * 4 + \
                           EX_GUARD_LEN
#define EX_ASSERTFAIL_DUMP_LEN  EX_LOG_SIZE - (EX_HEADER_SIZE + EX_SWVER_LEN + \
                                EX_ENVINFO_SIZE + EX_DIAGINFO_SIZE + EX_ASSERTFAIL_SIZE)

typedef struct ex_assert_fail_t
{
    u8 filename[EX_ASSERTFAIL_FILENAME_LEN];  /* offset: +0xD8, length: 24 */
    u32 linenumber;                          /* offset: +0xF0, length: 4 */
    u32 parameters[3];                       /* offset: +0xF4, length: 12 */
    u8 dump[EX_ASSERTFAIL_DUMP_LEN];         /* offset: +0x100, length: 252 */
    u8 guard[EX_GUARD_LEN];                  /* offset: +0x1FC, length: 4 */
} EX_ASSERTFAIL_T;


/*******************************************************************************
 * Globally Exported Data Structure
 *******************************************************************************/

typedef union
{
    EX_FATALERR_T fatalerr;
    EX_ASSERTFAIL_T assert;
} EX_CONTENT_T;

/* Standard strutcure of an exception log */
/*==========================================*/
/* NOTE: The structure is frozen; offset of content should be FIXED. */
/*==========================================*/
typedef struct ex_exception_log_t
{
    EX_HEADER_T header;                  /* offset: +0x0, length: 4 */
    u8 sw_version[EX_SWVER_LEN];   /* offset: +0x4, length: 12 */
    EX_ENVINFO_T envinfo;                /* offset: +0x10, length: 164 */
    EX_DIAGNOSISINFO_T diaginfo;         /* offset: +0xB4, length: 36 */
    EX_CONTENT_T content;                /* offset: +0xD8, length: 292 */
} EX_LOG_T;

/*****************************************************************
* Function:     Ql_Sleep
*
* Description:
*               Suspends the execution of the current task
*               until the time-out interval elapses.
* Parameters:
*               msec:
*                   The time interval for which execution is to
*                   be suspended, in milliseconds.
* Return:
*               None
*****************************************************************/
void Ql_Sleep(u32 msec);

/*****************************************************************
* Function:     Ql_Reset
*
* Description:
*               This function resets the system.
*
* Parameters:
*               resetType:
*                   must be 0.
* Return:
*               None
*****************************************************************/
void Ql_Reset(u8 resetType);

/*****************************************************************
* Function:     Ql_GetUID
*
* Description:
*               Get the module UID.
*
* Parameters:
*               ptrUID:
*                  [out] Point to the buffer to store the uid.
*                           Need 20 bytes length of buffer.
*               len:
*                   [in] The length of buffer.
* Return:        
*               If the uid_buf is null, this function will return
*               QL_RET_ERR_INVALID_PARAMETER.
*               If this function read the uid successfully, the length
*               of UID will be returned.
*****************************************************************/
s32 Ql_GetUID(u8* ptrUID, u32 len);

/*****************************************************************
* Function:     Ql_GetMsSincePwrOn 
* 
* Description:
*               This function returns the number of milliseconds 
*               since the device booted.
*
* Parameters:
*               None
* Return:        
*               Number of milliseconds
*****************************************************************/
u64 Ql_GetMsSincePwrOn(void);

/*****************************************************************
* Function:     Ql_GetCoreVer
*
* Description:
*               Get the version ID of the core.
*
* Parameters:
*               ptrVer:
*                   [out] Point to a unsigned char buffer, which is
*                           the the version ID of the core.
*               len:
*                   [in] It must be equal or greater than the length of version ID.
*                         Otherwise error code will be returned.
* Return:
*               The length of version ID indicates success.
*               Negative indicates failure. please see Error Code Definition.
*****************************************************************/
s32  Ql_GetCoreVer(u8* ptrVer, u32 len);

/*****************************************************************
* Function:     Ql_GetSDKVer
*
* Description:
*               Get the version ID of the SDK.
*
* Parameters:
*               ptrVer:
*                   [out] Point  to a unsigned char buffer, which is
*                           the the version ID of the SDK.
*               len:
*                   [in] A number will be compare with the length of version ID.
*
* Return:
*               The smaller between len and the length of version ID.
*****************************************************************/
s32  Ql_GetSDKVer(u8* ptrVer, u32 len);

/*****************************************************************
* Function:     Ql_OS_GetMessage 
* 
* Description:
*               This function retrieves a message from the current task's 
*               message queue. When there is no message in the task's
*               message queue, the task is in the waiting state..
* Parameters:
*               msg: 
*                   A pointer to ST_MSG.
* Return:        
*               Always return QL_RET_OK.
*****************************************************************/
s32 Ql_OS_GetMessage(ST_MSG* msg);

/*****************************************************************
* Function:     Ql_OS_SendMessage 
* 
* Description:
*               Send message between tasks.

* Parameters:
*               destTaskId: 
*                   One value of Enum_TaskId.
*               msgId: 
*                   User message id, must bigger than 0xFF.
*               param1:
*               param2:
*                   Parameters to send to another task.
* Return:        
*               OS_SUCCESS
*               OS_INVALID_ID
*               OS_MEMORY_NOT_VALID
*               OS_Q_FULL
*****************************************************************/
s32 Ql_OS_SendMessage(s32 destTaskId, u32 msgId, u32 param1, u32 param2);

/*****************************************************************
* Function:     Ql_OS_CreateMutex 
* 
* Description:
*               Create a mutex with name.

* Parameters:
*               mutexName: Mutex Name
* Return:        
*               Mutex Id
*****************************************************************/
u32 Ql_OS_CreateMutex(char* mutexName);

/*****************************************************************
* Function:     Ql_OS_TakeMutex 
* 
* Description:
*               Obtain an instance of the specified MUTEX.

* Parameters:
*               mutexId: Mutex Id
* Return:        
*               None
*****************************************************************/
void Ql_OS_TakeMutex(u32 mutexId);

/*****************************************************************
* Function:     Ql_OS_GiveMutex 
* 
* Description:
*               Release the instance of the specified MUTEX.

* Parameters:
*               mutexId: Mutex Id
* Return:        
*               None
*****************************************************************/
void Ql_OS_GiveMutex(u32 mutexId);

/*****************************************************************
* Function:     Ql_OS_CreateSemaphore 
* 
* Description:
*               Creates a counting semaphore.
* Parameters:
*               semName: 
*                   Name of semaphore
*               maxCount: 
*                   Initial value of semaphore
* Return:        
*               Value of created semaphore
*****************************************************************/
u32 Ql_OS_CreateSemaphore(char* semName, u32 maxCount);

/*****************************************************************
* Function:     Ql_OS_TakeSemaphore 
* 
* Description:
*               Obtain an instance of the specified semaphore.

* Parameters:
*               semId: Name of semaphore
*               wait: [IN] wait mode, specify the behavior when the semaphore is
*                   not ready immediately, it can be one the the following values:
*                   TRUE  - wait until ownership can be satisfied.
*                   FALSE - don't wait for other task gives ownership to it.
* Return:        
*               OS_SUCCESS: the operation is done successfully
*               OS_SEM_NOT_AVAILABLE: the semaphore is unavailable immediately.
*****************************************************************/
u32 Ql_OS_TakeSemaphore(u32 semId, bool wait);

/*****************************************************************
* Function:     Ql_OS_GiveSemaphore 
* 
* Description:
*               Release the instance of the specified semaphore.

* Parameters:
*               semId: Name of semaphore
* Return:        
*               None
*****************************************************************/
void Ql_OS_GiveSemaphore(u32 semId);

/*****************************************************************
* Function:     Ql_OS_CreateEvent 
* 
* Description:
* 			This function creates an event-flag group with the specified name.
*			Each event-flag group contains 10 event flags. 
*
* Parameters:
*			evtName:
*				Event name.
*
* Return:        
*			An event Id that identify this event uniquely.
*****************************************************************/
u32 Ql_OS_CreateEvent(char* evtName);

/*****************************************************************
* Function:     Ql_OS_WaitEvent 
* 
* Description:
* 			This function waits until the specified type of event is in the signaled 
*                  state. Developers can specify different types of events for purposes.
*                  The event flags are defined in 'Enum_EventFlag'.
*
* Parameters:
*			evtId:
*				Event id that is returned by calling Ql_OS_CreateEvent().
*			evtFlag:
*				Event flag type. Please refer to Enum_EventFlag.
*
* Return:        
*			Zero indicates success, an nonzero means failure.
*****************************************************************/
s32 Ql_OS_WaitEvent(u32 evtId, u32 evtFlag);

/*****************************************************************
* Function:     Ql_OS_SetEvent 
* 
* Description:
*                  This function sets the specified event flag. Any task waiting on the
*                  event, whose event flag request is satisfied, is resumed.
*
* Parameters:
*			evtId:
*				Event id that is returned by calling Ql_OS_CreateEvent().
*			evtFlag:
*				Event flag type. Please refer to Enum_EventFlag.
*
* Return:        
*			Zero indicates success, an nonzero means failure.
*****************************************************************/
s32 Ql_OS_SetEvent(u32 evtId, u32 evtFlag);

/*****************************************************************
* Function:     Ql_SetLastErrorCode 
* 
* Description:
*               Set error code

* Parameters:
*               errCode: Error code
* Return:        
*              True indicates success or failure indicates failure. 
*****************************************************************/
s32 Ql_SetLastErrorCode(s32 errCode);

/*****************************************************************
* Function:     Ql_GetLastErrorCode 
* 
* Description:
*               Retrieves the calling task's last-error code value.

* Parameters:
*               None
* Return:        
*               The return value is the calling task's last-error code.
*****************************************************************/
s32 Ql_GetLastErrorCode(void);

/*****************************************************************
* Function:     Ql_OS_GetCurrentTaskPriority 
* 
* Description:
*               Get the priority of the current task

* Parameters:
*               None
* Return:        
*               Task priority, ranges from 200 to 255.
*****************************************************************/
u32 Ql_OS_GetCurrentTaskPriority(void);

/*****************************************************************
* Function:     Ql_OS_GetCurrenTaskLeftStackSize 
* 
* Description:
*               Get the left number of bytes in the current task stack
* Parameters:
*               None
* Return:        
*               Number of bytes, ranges from 1024 to 10*1024.
*****************************************************************/
u32 Ql_OS_GetCurrenTaskLeftStackSize(void);

/*****************************************************************
* Function:     Ql_OS_GetActiveTaskId 
* 
* Description:
*               This function returns the task ID of the current task.
* Parameters:
*               None.
* Return:        
*               The Id number of current task.
*****************************************************************/
s32 Ql_OS_GetActiveTaskId(void);

/*****************************************************************
* Function:     Ql_GetExceptionRecords
* 
* Description:
*               Retrieves the exception records in the NVRAM.

* Parameters:
*               exceptionBuf:
*                      pointer of exception records buffer 
*               validCnt:
*                      pointer of count of valid exception records 
*               rdCnt:
*                      get count of records
* Return:        
*               The return the exception records in the NVRAM.
*****************************************************************/
s32 Ql_GetExceptionRecords (EX_LOG_T *exceptionBuf,u8 *validCnt,u8 rdCnt);

#ifdef TASK_ENTRYFUNC_DEF
#undef TASK_ENTRYFUNC_DEF
#endif
#ifdef TASK_DEFINITION
#undef TASK_DEFINITION
#endif
#define TASK_ID_DEF
#include "ql_common.h"
TASK_DEFINITION_BEGIN
#include "custom_task_cfg.h"
TASK_DEFINITION_END
#undef TASK_ID_DEF

#endif  // End-of __QL_SYSTEM_H__

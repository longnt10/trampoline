/**
 * @file tpl_trace_posix.c
 *
 * @section desc File description
 *
 * Trampoline Trace Calls
 *
 * @section copyright Copyright
 *
 * Trampoline OS
 *
 * Trampoline is copyright (c) IRCCyN 2005+
 * Copyright ESEO for function and data structures documentation
 * Copyright IRISA - JF Deverge for libpcl port
 * Copyright Geensys for hcs12 port
 * Trampoline is protected by the French intellectual property law.
 *
 * This software is distributed under the Lesser GNU Public Licence
 *
 * @section infos File informations
 *
 * $Date: 2009-04-24 10:26:01 +0000 (ven. 24 avril 2009) $
 * $Rev: 735 $
 * $Author: ljunker $
 * $URL: http://trampoline.rts-software.org/svn/trunk/os/tpl_trace.c $
 */
#include "tpl_trace_posix.h"

#define OS_START_SEC_CODE
#include "tpl_memmap.h"

#include <string.h> /* strncpy() */
#include <stdlib.h> /* atol() */

/**
* DATE FUNCTION
*/

FUNC(void, OS_CODE)tpl_trace_get_date()
{
  struct timeval result;
    int sec;
    int usec;

  gettimeofday(&result,NULL);
  sec = result.tv_sec;
  usec = result.tv_usec;
  sprintf(DATE,"%d%06d",sec,usec);
}

/**
* RECORD FUNCTIONS
*/

/** Function which start the record, memorizing the begin date of the event 
* and the type of event traced 
*/

FUNC(void, OS_CODE)tpl_trace_event_begin(
    VAR(int, OS_VAR) trace_id)
{
 strncpy(trace.begin_date, DATE, sizeof(DATE));
 trace.trace_type = trace_id;
 trace.value[0] = -1;

}

/** Function adding a value to the record. Knowing the event type of the trace, 
* we can retrieve how much value this record must contain. 
*/

FUNC(void, OS_CODE)tpl_trace_value(
  VAR(int, OS_VAR) value)
{
  int i = 0;
  while ( trace.value[i] != -1 )
  {
    i++;
  }
  trace.value[i] = value;
  trace.value[i+1] = -1;
}

/* Function ending the record, by adding the end date of the event traced. */

FUNC(void, OS_CODE)tpl_trace_event_end(void)
{
  tpl_trace_get_date();
  strncpy(trace.end_date, DATE, sizeof(DATE));
}

/* Three functions of format, each one corresponding to a different output format. */

FUNC(void, OS_CODE)tpl_trace_format_txt(void)
{
  int i = 0;
  if(TRACE_FILE_PT == NULL)
  {
  TRACE_FILE_PT = fopen(TRACE_FILE,"a");
  }
  fprintf(TRACE_FILE_PT,"%s,%d",trace.begin_date,trace.trace_type);
  while ( trace.value[i] != -1 )
  {
    fprintf(TRACE_FILE_PT,",%d",trace.value[i]);
    i++;
  }
  fprintf(TRACE_FILE_PT,",%s\n",trace.end_date);
}


FUNC(void, OS_CODE)tpl_trace_format_xml(void)
{
  int i = 0;
  if(TRACE_FILE_PT == NULL)
  {
  TRACE_FILE_PT = fopen(TRACE_FILE,"a");
  fprintf(TRACE_FILE_PT,"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<!DOCTYPE trace\n[\n<!ELEMENT trace(record*)>\n<!ELEMENT record (begin_date,trace_type,value*,end_date)>\n<!ELEMENT begin_date>\n<!ELEMENT trace_type>\n<!ELEMENT value>\n<!ELEMENT end_value>\n]>\n<trace>\n");
  }
  fprintf(TRACE_FILE_PT,"<record>\n<begin_date>%s</begin_date>\n<trace_type>%d</trace_type>\n",trace.begin_date,trace.trace_type);
  while ( trace.value[i] != -1 )
  {
    fprintf(TRACE_FILE_PT,"<value>%d</value>\n",trace.value[i]);
    i++;
  }
  fprintf(TRACE_FILE_PT,"<end_date>%s</end_date>\n</record>\n",trace.end_date);
  if(trace.trace_type == 19)
  {
    fprintf(TRACE_FILE_PT,"</trace>");
  }
}


FUNC(void, OS_CODE)tpl_trace_format_bin(void)
{ 
  int i = 0;
  long date;
  if(TRACE_FILE_PT == NULL)
  {
  TRACE_FILE_PT = fopen(TRACE_FILE,"ab");
  }
  date = atol(trace.begin_date);
  fwrite(&date,1,sizeof(date),TRACE_FILE_PT);
  fwrite(&trace.trace_type,1,sizeof(trace.trace_type),TRACE_FILE_PT);
  while ( trace.value[i] != -1 )
  {
    fwrite(&trace.value[i],1,sizeof(trace.value[i]),TRACE_FILE_PT);
    i++;
  }
  date = atol(trace.end_date);
  fwrite(&date,1,sizeof(date),TRACE_FILE_PT);
}

#define OS_STOP_SEC_CODE
#include "tpl_memmap.h"

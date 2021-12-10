/*
*********************************************************************************************************
*                                                THREAD-OS
*                                          The Real-Time Kernel
*                                  THREAD-OS Configuration File for V1.x
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
*
*
* File    : os_config.h
* By      : tony(gulonglong)
* Version : V1.1
*

*   THREAD-OS is provided in source form for FREE evaluation, for educational use or for peaceful research.

*********************************************************************************************************
*/

#ifndef __OS_CONFIG_H__
#define __OS_CONFIG_H__

#define  THREAD_OS_VERSION          "V1.2"

#define  THREAD_OS_TICKS_PER_SEC    1000        /* Set the number of ticks in one second    */

                                               /* --------------------- TIME MANAGEMENT ---------------------- */
#define  THREAD_OS_TIME_EN          1u         /* Enable (1) or Disable (0) code generation for SYSTEM TIME    */

                                               /* --------------------- TIMER MANAGEMENT --------------------- */
#define  THREAD_OS_SOFT_TIMER_EN    1u         /* Enable (1) or Disable (0) code generation for SOFT TIMER     */

                                               /* --------------------- LOG MANAGEMENT ---------------------   */
#define  THREAD_OS_LOG              2u         /* Disable (0) ; UART (2)*/

                                               /* --------------------- SHELL MANAGEMENT ---------------------   */
#define  THREAD_OS_SHELL_EN         1u         /* Enable (1) or Disable (0) code generation for OS SHELL         */

#define  THREAD_OS_OSFIFO_EN        1u          /* --------------------- SHELL MANAGEMENT ---------------------   */
                                               /* Enable (1) or Disable (0) code generation for OSFIFO           */

#endif

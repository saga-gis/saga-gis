/**********************************************************
 * Version $Id: e00write.c 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/**********************************************************************
 * $Id: e00write.c 911 2011-02-14 16:38:15Z reklov_w $
 *
 * Name:     e00write.c
 * Project:  Compressed E00 Read/Write library
 * Language: ANSI C
 * Purpose:  Functions to write Compressed E00 files from a stream of 
 *           uncompressed lines. 
 * Author:   Daniel Morissette, danmo@videotron.ca
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2005/08/31 14:01:00  oconrad
 * no message
 *
 * Revision 1.1.1.1  2005/08/17 08:25:16  oconrad
 * no message
 *
 * Revision 1.1.1.1  2005/08/15 13:35:12  oconrad
 * no message
 *
 * Revision 1.1  2004/04/16 13:36:45  oconrad
 * no message
 *
 * Revision 1.5  1999/02/25 18:46:41  daniel
 * Now use CPL for Error handling, Memory allocation, and File access.
 *
 * Revision 1.4  1999/01/08 17:40:01  daniel
 * Added E00WriteCallbackOpen()
 *
 * Revision 1.3  1998/11/13 16:54:23  daniel
 * Check for '\r' and '\n' at end of input line while compressing, just
 * in case...
 *
 * Revision 1.2  1998/11/13 15:48:42  daniel
 * Simplified the generation of the compression codes for numbers
 * (use a logical rule instead of going case by case)
 *
 * Revision 1.1  1998/11/13 14:19:51  daniel
 * Initial revision
 *
 **********************************************************************
 * Copyright (c) 1998, 1999, Daniel Morissette
 *
 * All rights reserved.  This software may be copied or reproduced, in
 * all or in part, without the prior written consent of its author,
 * Daniel Morissette (danmo@videotron.ca).  However, any material copied
 * or reproduced must bear the original copyright notice (above), this 
 * original paragraph, and the original disclaimer (below).
 * 
 * The entire risk as to the results and performance of the software,
 * supporting text and other information contained in this file
 * (collectively called the "Software") is with the user.  Although 
 * considerable efforts have been used in preparing the Software, the 
 * author does not warrant the accuracy or completeness of the Software.
 * In no event will the author be liable for damages, including loss of
 * profits or consequential damages, arising out of the use of the 
 * Software.
 * 
 **********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>

#include "e00compr.h"

static int _CompressLine(E00WritePtr psInfo, const char *pszLine);
static int _WriteNextCompressedLine(E00WritePtr psInfo, int nMaxChars);
static int _PrintfNextLine(E00WritePtr psInfo, const char *pszFmt, ...);

/**********************************************************************
 *                          E00WriteOpen()
 *
 * Try to open output file, and alloc/initialize a new E00WritePtr
 * handle.
 *
 * nComprLevel must be one of:
 *       E00_COMPR_NONE, E00_COMPR_PARTIAL or E00_COMPR_FULL
 *
 * Returns the new handle, or NULL if the file could not be opened.
 * E00WriteClose() will eventually have to be called to release 
 * the resources used by the new handle.
 **********************************************************************/
E00WritePtr  E00WriteOpen(const char *pszFname, int nComprLevel)
{
    E00WritePtr  psInfo = NULL;
    FILE        *fp;

    CPLErrorReset();

    /* Open the file 
     */
    fp = VSIFOpen(pszFname, "wt");
    if (fp == NULL)
    {
        CPLError(CE_Failure, CPLE_OpenFailed,
                 "Failed to open %s: %s", pszFname, strerror(errno));
        return NULL;
    }

    /* Allocate and initialize a E00ReadPtr handle.
     */
    psInfo = (E00WritePtr)CPLCalloc(1, sizeof(struct _E00WriteInfo));

    psInfo->fp = fp;
    psInfo->nComprLevel = nComprLevel;

    return psInfo;
}

/**********************************************************************
 *                          E00WriteCallbackOpen()
 *
 * This is an alternative to E00WriteOpen() for cases where you want to
 * do all the file management yourself.  You open/close the file yourself
 * and provide a callback functions to write one line at a time to the
 * file.  pRefData is your handle on the physical file and can
 * be whatever you want... it is not used by the library, it will be
 * passed directly to your callback function when it is called.
 *
 * The callback function must have the following C prototype:
 *
 *   int    myWriteNextLine(void *pRefData, const char *pszLine);
 *
 *   Like printf() does, myWriteNextLine() should return a positive 
 *   value on success (the number of chars written) 
 *   or -1 if an error happened.
 *   The value passed by the library in pszLine will not be terminated
 *   by a '\n' character... it is assumed that the myWriteNextLine()
 *   implementation will take care of terminating the line with a
 *   '\n' if necessary.
 *
 * nComprLevel must be one of:
 *       E00_COMPR_NONE, E00_COMPR_PARTIAL or E00_COMPR_FULL
 *
 * E00WriteCallbackOpen() returns a new E00ReadWritePtr handle.
 * E00WriteClose() will eventually have to be called to release 
 * the resources used by the new handle.
 **********************************************************************/
E00WritePtr  E00WriteCallbackOpen(void *pRefData,
                                 int (*pfnWriteNextLine)(void *, const char *),
                                 int nComprLevel)
{
    E00WritePtr  psInfo = NULL;

    CPLErrorReset();

    /* Make sure we received a valid function pointer
     */
    if (pfnWriteNextLine == NULL)
    {
        CPLError(CE_Failure, CPLE_IllegalArg,
                 "Invalid function pointer!");
        return NULL;
    }

    /* Allocate and initialize a E00ReadPtr handle.
     */
    psInfo = (E00WritePtr)CPLCalloc(1, sizeof(struct _E00WriteInfo));

    psInfo->pRefData = pRefData;
    psInfo->pfnWriteNextLine = pfnWriteNextLine;
    psInfo->nComprLevel = nComprLevel;

    return psInfo;
}


/**********************************************************************
 *                          E00WriteClose()
 *
 * Close output file and release any memory used by the E00WritePtr.
 **********************************************************************/
void    E00WriteClose(E00WritePtr psInfo)
{
    CPLErrorReset();

    if (psInfo)
    {
        /* Flush output buffer before closing file.
         */
        if (psInfo->iOutBufPtr > 0)
            _WriteNextCompressedLine(psInfo, 1);

        if (psInfo->fp)
            fclose(psInfo->fp);

        CPLFree(psInfo);
    }
}


/**********************************************************************
 *                          E00WriteNextLine()
 *
 * Take a line of what should be headed to a uncompressed E00 file,
 * convert it to the requested compression level, and write the 
 * compressed result to the output file.
 *
 * Returns 0 if the line was processed successfully, or an error number
 * (> 0) if an error happened.
 **********************************************************************/
int     E00WriteNextLine(E00WritePtr psInfo, const char *pszLine)
{
    char    *pszPtr;
    int     nStatus = 0;

    CPLErrorReset();

    if (psInfo && (psInfo->fp || psInfo->pfnWriteNextLine))
    {
        psInfo->nSrcLineNo++;

        if (psInfo->nComprLevel == E00_COMPR_NONE)
        {
            /* Uncompressed file... write line directly. 
             */
            nStatus = _PrintfNextLine(psInfo, "%s", pszLine);
        }
        else if (psInfo->nSrcLineNo == 1)
        {
            /* Header line in a compressed file... write line 
             * after replacing "EXP  0" with "EXP  1".
             */
            if ((pszPtr = strstr(pszLine, " 0")) != NULL)
            {
                nStatus = _PrintfNextLine(psInfo, "EXP  1%s", pszPtr+2);
            }
            else
            {
                /* Write line directly... assume that it contains a valid
                 * EXP prefix!
                 */
                nStatus = _PrintfNextLine(psInfo, "%s", pszLine);
            }
        }
        else
        {
            /* FULL or PARTIAL compression... compress the current line,
             * (output goes in psInfo->szOutBuf)
             */
            nStatus = _CompressLine(psInfo, pszLine);

            while (nStatus == 0 && psInfo->iOutBufPtr >= 80)
            {
                /* Time to write the first 80 chars from the output buffer
                 */
                nStatus = _WriteNextCompressedLine(psInfo, 0);
            }
        }

    }
    else
    {
        /* This should never happen unless the lib is not properly used
         * or if an error happened in previous calls and was ignored by 
         * the caller
         */
        CPLError(CE_Failure, CPLE_IllegalArg,
                 "Invalid E00WritePtr handle!");
        nStatus = 203;
    }

    return nStatus;
}


/**********************************************************************
 *                          _CompressLine()
 *
 * Compress one line of input, and store the compressed copy at the
 * end of psInfo->pszOutBuf.
 *
 * Returns 0 if the line was compressed successfully, or an error number
 * (> 0) if an error happened.
 **********************************************************************/
static int _CompressLine(E00WritePtr psInfo, const char *pszLine)
{
    int     nStatus = 0;
    int     nDigits, nExpSign, nDotPosition, iCurPos;
    int     numTotalDigits, numExpDigits;
    char    n, *pszCodePos;

    int         nStartOutBufPtr;
    const char *pszStartSrcPtr;

    while(*pszLine != '\0' && *pszLine != '\n' && *pszLine != '\r')
    {
        /*-------------------------------------------------------------
         * By default, apply PARTIAL compression 
         * Note that PARTIAL is a subset of FULL compression.
         *------------------------------------------------------------*/
        if (*pszLine == '~')
        {
            /* The '~' char is encoded as "~~"
             */
            strcpy( psInfo->szOutBuf+psInfo->iOutBufPtr, "~~");
            psInfo->iOutBufPtr += 2;
        }
        else if (strncmp(pszLine, "   ", 3) == 0)
        {
            /* A stream of at least 3 spaces.
             * Count number of spaces and replace with a "~ n" code
             */
            n=1;
            while(*(pszLine+1) == ' ')
            {
                n++;
                pszLine++;
            }
            strcpy( psInfo->szOutBuf+psInfo->iOutBufPtr, "~ ");
            psInfo->iOutBufPtr += 2;
            psInfo->szOutBuf[psInfo->iOutBufPtr++] = ' ' + n;
        }

        /*-------------------------------------------------------------
         * The cases below are specific to FULL compression.
         *------------------------------------------------------------*/
        else if (psInfo->nComprLevel == E00_COMPR_FULL &&
                 isdigit(*pszLine))
        {
            /* Keep track of current input/output buffer positions in case
             * we would have to revert the encoding.  This could happen if
             * the numeric value is less than 4 characters.
             */
            nStartOutBufPtr = psInfo->iOutBufPtr;
            pszStartSrcPtr = pszLine;

            /* Reset flags used for parsing numeric format
             */
            nDigits = 0;
            nDotPosition = 0; /* 0 means no decimal point */
            nExpSign = 0;     /* 0 means no exponent      */
            numExpDigits = 0;
            numTotalDigits = 0;
            n = 0;

            /* Prepare the code sequence in the output buffer.
             * The code value will be set only after the number 
             * is finished parsing.
             */
            psInfo->szOutBuf[psInfo->iOutBufPtr++] = '~';
            pszCodePos = psInfo->szOutBuf+psInfo->iOutBufPtr++;

            /* Scan the numeric sequence, encoding the digits as we read,
             * and keeping track of decimal point position and exponent.
             */
            for(iCurPos=0; 
                *pszLine != '\0' && numExpDigits<2; 
                pszLine++, iCurPos++)
            {
                if (isdigit(*pszLine))
                {
                    /* Accumulate pairs of digits
                     */
                    numTotalDigits++;
                    if (numTotalDigits % 2 == 1)
                    {
                        n = (*pszLine - '0')*10;
                    }
                    else
                    {
                        n += (*pszLine - '0');
                        if (n >= 92)
                        {
                            /* Pairs of digits > 92 are encoded on 2 chars
                             */
                            psInfo->szOutBuf[psInfo->iOutBufPtr++] = 92 + '!';
                            n -= 92;
                        }
                        psInfo->szOutBuf[psInfo->iOutBufPtr++] = n + '!';
                    }

                    /* Count number of exponent digits to end the loop
                     * once we've read 2 exponent digits
                     */
                    if (nExpSign != 0)
                        numExpDigits++;
                }
                else if (*pszLine == '.' && nDotPosition == 0 && iCurPos < 15)
                {
                    /* Decimal point position... make sure that we have only
                     * one decimal point, and that it is not beyond the 14th
                     * position. If these conditions are not met then end the
                     * numeric sequence here. 
                     */
                    nDotPosition = iCurPos;
                }
                else if (*pszLine == 'E' && 
                         (*(pszLine+1) == '-' || *(pszLine+1)=='+') &&
                         isdigit(*(pszLine+2)) && isdigit(*(pszLine+3)) &&
                         !isdigit(*(pszLine+4)) )
                {
                    /* Exponent ... fetch sign, and read only 2 more digits
                     * A "E+" or "E-" MUST be followed by 2 and only 2 digits.
                     * If it's not the case, then the numeric sequence ends
                     * here.
                     */
                    pszLine++;
                    nExpSign = (*pszLine=='-') ? -1: 1;
                }
                else
                {
                    /* Numeric sequence finished ... stop parsing.
                     */
                    break;
                }
            }/*for ... parsing numeric value*/

            /* If the numeric value contains an even number of digits,
             * then pad the last pair of digits with a zero and encode it.
             */
            if (numTotalDigits % 2 == 1)
            {
                psInfo->szOutBuf[psInfo->iOutBufPtr++] = n + '!';
            }

            /* If the character that ends the number is NOT a '~', a ' '
             * or a 'end of line' then add a '~' to mark the end of the 
             * number.  This extra '~' will be ignored by the uncompress
             * algorithm.
             */
            if (*(pszLine) != '~' && *(pszLine) != ' ' && 
                *(pszLine) != '\0')
            {
                psInfo->szOutBuf[psInfo->iOutBufPtr++] = '~';
            }

            /* At the end of the loop, the read pointer is located on the char
             * that follows the numeric value.  Move it back 1 char so that
             * processing can continue later with the outer loop.
             */
            pszLine--;
             
            /* Check that the parsed value contains enough characters to 
             * justify encoding it. The encoded value should not be
             * bigger than the original.  If the encoded value is the same
             * size as the original then it is still encoded (a bit silly!).
             * All numbers < 4 chars are not encoded, and some, such as 
             * "1092" won't either because it would require 5 chars to 
             * encode them.
             *
             * If the value should not be encoded, then overwrite the sutff 
             * we started encoding with a direct copy of the numeric value.
             *
             * (At this point, iCurPos is equal to the number of chars in the
             * source value.)
             */
            if ( iCurPos < psInfo->iOutBufPtr - nStartOutBufPtr )
            {
                strncpy(psInfo->szOutBuf+ nStartOutBufPtr, 
                        pszStartSrcPtr, iCurPos);

                psInfo->iOutBufPtr = nStartOutBufPtr + iCurPos;
            }
            else
            {
                /* Now that we have parsed the numeric value, set the code 
                 * based on the characteristics we found.
                 */
                *pszCodePos = '!' + ((numTotalDigits % 2 == 1)? 45:0) +
                                    (nExpSign?((nExpSign>0)?15:30):0) +
                                    nDotPosition;
            }
        }
        else
        {
            /* This char cannot be compressed ... output directly.
             */
            psInfo->szOutBuf[psInfo->iOutBufPtr++] = *pszLine;
        }

        /* Check for buffer overflow... just in case!!!
         */
        if (psInfo->iOutBufPtr >= E00_WRITE_BUF_SIZE)
        {
            /* The buffer size has been set big enough to prevent this error
             * from ever happening.  So if it ever happens, then it's likely
             * that the input lines were longer than 80 chars, which is the
             * maximum length of a uncompressed line in a E00 file.
             */
            CPLError(CE_Failure, CPLE_FileIO,
                     "Output buffer overflow!!!.");
            nStatus = 205;
            break;
        }

        /* Get ready to proceed with next char from input string
         */
        pszLine++;
    }

    /* Terminate this line with a newline code 
     * This code applies to both PARTIAL and FULL compression.
     */
    strcpy( psInfo->szOutBuf+psInfo->iOutBufPtr, "~}");
    psInfo->iOutBufPtr += 2;

    return nStatus;
}

/**********************************************************************
 *                          _WriteNextCompressedLine()
 *
 * If bFlushWholeBuffer == 0, write the first 80 characters from 
 * psInfo->szOutBuf, and remove the written chars from szOutBuf.  
 * A '\n' (not counted in nMaxChars) will be written to terminate the 
 * output line.
 *
 * Pass bFlushBuffer == 1 to force writing the whole contents of szOutBuf
 * at once.
 *
 * Returns 0 if the line was written successfully, or an error number
 * (> 0) if an error happened.
 **********************************************************************/
static int _WriteNextCompressedLine(E00WritePtr psInfo, int bFlushWholeBuffer)
{
    int     nStatus = 0, nToWrite;
    char    *pszSrc, *pszDst;

    psInfo->szOutBuf[psInfo->iOutBufPtr] = '\0';

    if (!bFlushWholeBuffer && psInfo->iOutBufPtr > 80)
    {
        /* Write the first 80 chars
         * Note that a compressed line cannot end with spaces... spaces should
         * be reported on the next line.
         */
        nToWrite = 80;
        while(nToWrite > 1 && psInfo->szOutBuf[nToWrite-1] == ' ')
            nToWrite--;

        nStatus = _PrintfNextLine(psInfo, "%-.*s", nToWrite, psInfo->szOutBuf);

        /* Remove these chars from the output buffer
         */
        pszDst = psInfo->szOutBuf;
        pszSrc = psInfo->szOutBuf+nToWrite;
        while(*pszSrc != '\0')
        {
            *pszDst = *pszSrc;
            pszDst++;
            pszSrc++;
        }
        psInfo->iOutBufPtr -= nToWrite;
    }
    else
    {
        /* Just write the buffer contents directly
         */
        nStatus = _PrintfNextLine(psInfo, "%s", psInfo->szOutBuf);

        psInfo->iOutBufPtr = 0;
    }

    return nStatus;
}


/**********************************************************************
 *                          _PrintfNextLine()
 *
 * Cover function for fprintf() that will do error checking and
 * reporting, and either call fprintf() directly or call the callback
 * provided by the caller if E00WriteCallbackOpen() was used.
 *
 * A \n will be automatically appended to the string when it is
 * written, so no \n should be included at the end of pszFmt.
 * 
 * Returns 0 on success, or error 204 if it failed.
 **********************************************************************/
static int _PrintfNextLine(E00WritePtr psInfo, const char *pszFmt, ...)
{
    va_list args;
    static char szBuf[E00_WRITE_BUF_SIZE];
    int     nStatus = 0;

    /* Expand the string to print
     */
    va_start(args, pszFmt);
    vsprintf(szBuf, pszFmt, args);
    va_end(args);

    /* Write the line using the right method 
     */
    if (psInfo->pfnWriteNextLine == NULL)
    {
        /* Use fprintf() directly
         */
        if ( VSIFPrintf(psInfo->fp, "%s\n", szBuf) < 0)
        {
            CPLError(CE_Failure, CPLE_FileIO,
                     "Error writing to file: %s", 
                            strerror(errno));
            nStatus = 204;
        }
    }
    else
    {
        /* Use pfnWritenextLine() callback
         */
        if ( psInfo->pfnWriteNextLine(psInfo->pRefData, szBuf) < 0)
        {
            CPLError(CE_Failure, CPLE_FileIO,
                     "Error writing to file.");
            nStatus = 204;
        }
    }

    return nStatus;
}


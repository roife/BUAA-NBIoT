/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/

#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>


#define TY_FORMAT_FLAG_LEFT_JUSTIFY   (1u << 0)
#define TY_FORMAT_FLAG_PAD_ZERO       (1u << 1)
#define TY_FORMAT_FLAG_PRINT_SIGN     (1u << 2)
#define TY_FORMAT_FLAG_ALTERNATE      (1u << 3)


#define TY_PRINTF_BUFFER_SIZE (512)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef struct 
{
  char*     pBuffer;
  uint16_t  BufferSize;
  uint16_t  Cnt;

  int16_t   ReturnValue;
} ty_printf_desc_t;

typedef uint32_t (*ty_write_cb)(void *, uint32_t );

ty_write_cb ty_write;

/*********************************************************************
*
*       Function prototypes
*
**********************************************************************
*/
int ty_vprintf(const char * sFormat, va_list * pParamList);

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _StoreChar
*/
static void _StoreChar(ty_printf_desc_t *p, char c) 
{	
	if( (p->Cnt + 1) <= p->BufferSize )
	{
		p->pBuffer[p->Cnt] = c;
		p->Cnt++;
	  p->ReturnValue++;
	}
  
  /* Write part of string, when the buffer is full */
  
  if (p->Cnt == p->BufferSize) 
	{
    if (ty_write(p->pBuffer, p->Cnt) != p->Cnt) 
		{
      p->ReturnValue = -1;
    } 
		else 
		{
      p->Cnt = 0u;
    }
  }
}

/*********************************************************************
*
*       _PrintUnsigned
*/
static void _PrintUnsigned(ty_printf_desc_t * pBufferDesc, unsigned v, unsigned Base, unsigned NumDigits, unsigned FieldWidth, unsigned FormatFlags) 
{
  static const char _aV2C[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
  unsigned Div;
  unsigned Digit;
  unsigned Number;
  unsigned Width;
  char c;

  Number = v;
  Digit = 1u;
  
  /* Get actual field width */
	
  Width = 1u;
  while (Number >= Base)
	{
    Number = (Number / Base);
    Width++;
  }
	
  if (NumDigits > Width) 
	{
    Width = NumDigits;
  }
  
  /* Print leading chars if necessary */
  
  if ((FormatFlags & TY_FORMAT_FLAG_LEFT_JUSTIFY) == 0u) 
	{
    if (FieldWidth != 0u) 
		{
      if (((FormatFlags & TY_FORMAT_FLAG_PAD_ZERO) ==  TY_FORMAT_FLAG_PAD_ZERO) && (NumDigits == 0u)) 
			{
        c = '0';
      } 
			else 
			{
        c = ' ';
      }
			
      while ((FieldWidth != 0u) && (Width < FieldWidth)) 
			{
        FieldWidth--;
        _StoreChar(pBufferDesc, c);
				
        if (pBufferDesc->ReturnValue < 0)
          break;
      }
    }
  }
	
  if (pBufferDesc->ReturnValue >= 0) 
	{
    /*
     Compute Digit.
     Loop until Digit has the value of the highest digit required.
     Example: If the output is 345 (Base 10), loop 2 times until Digit is 100.
    */
    while (1) 
		{
      if (NumDigits > 1u)        // User specified a min number of digits to print? => Make sure we loop at least that often, before checking anything else (> 1 check avoids problems with NumDigits being signed / unsigned)
			{
				NumDigits--;
      } 
			else 
			{
        Div = v / Digit;
				
        if (Div < Base)          // Is our divider big enough to extract the highest digit from value? => Done
          break;
      }
      Digit *= Base;
    }
    
    /* Output digits  */
    
    do 
		{
      Div = v / Digit;
      v -= Div * Digit;
      _StoreChar(pBufferDesc, _aV2C[Div]);
      if (pBufferDesc->ReturnValue < 0) {
        break;
      }
      Digit /= Base;
    }while (Digit);
    
    /* Print trailing spaces if necessary */
    
    if ((FormatFlags & TY_FORMAT_FLAG_LEFT_JUSTIFY) == TY_FORMAT_FLAG_LEFT_JUSTIFY) 
		{
      if (FieldWidth != 0u) 
			{
        while ((FieldWidth != 0u) && (Width < FieldWidth)) 
				{
          FieldWidth--;
          _StoreChar(pBufferDesc, ' ');
					
          if (pBufferDesc->ReturnValue < 0)
            break;
        }
      }
    }
  }
}

/*********************************************************************
*
*       _PrintInt
*/
static void _PrintInt(ty_printf_desc_t * pBufferDesc, int v, unsigned Base, unsigned NumDigits, unsigned FieldWidth, unsigned FormatFlags) 
{
  unsigned Width;
  int Number;

  Number = (v < 0) ? -v : v;

  /* Get actual field width */

  Width = 1u;
  while (Number >= (int)Base) 
  {
    Number = (Number / (int)Base);
    Width++;
  }
	
  if (NumDigits > Width) 
	{
    Width = NumDigits;
  }
	
  if ((FieldWidth > 0u) && ((v < 0) || ((FormatFlags & TY_FORMAT_FLAG_PRINT_SIGN) == TY_FORMAT_FLAG_PRINT_SIGN)))
	{
    FieldWidth--;
  }
  
  /* Print leading spaces if necessary */
  
  if ( (((FormatFlags & TY_FORMAT_FLAG_PAD_ZERO) == 0u) || (NumDigits != 0u)) 
		   && ((FormatFlags & TY_FORMAT_FLAG_LEFT_JUSTIFY) == 0u) ) 
	{
    if (FieldWidth != 0u) 
		{
      while ((FieldWidth != 0u) && (Width < FieldWidth)) 
			{
        FieldWidth--;
        _StoreChar(pBufferDesc, ' ');
				
        if (pBufferDesc->ReturnValue < 0) 
          break;
      }
    }
  }
  
  /* Print sign if necessary */
  
  if (pBufferDesc->ReturnValue >= 0) 
	{
    if (v < 0)
		{
      v = -v;
      _StoreChar(pBufferDesc, '-');
    } 
		else if ((FormatFlags & TY_FORMAT_FLAG_PRINT_SIGN) == TY_FORMAT_FLAG_PRINT_SIGN) 
		{
      _StoreChar(pBufferDesc, '+');
    } 
    
		if (pBufferDesc->ReturnValue >= 0) 
		{
      /* Print leading zeros if necessary */
      if ( ((FormatFlags & TY_FORMAT_FLAG_PAD_ZERO) == TY_FORMAT_FLAG_PAD_ZERO) 
				   && ((FormatFlags & TY_FORMAT_FLAG_LEFT_JUSTIFY) == 0u) 
			     && (NumDigits == 0u) ) 
			{
        if (FieldWidth != 0u) 
				{
          while ((FieldWidth != 0u) && (Width < FieldWidth)) 
					{
            FieldWidth--;
            _StoreChar(pBufferDesc, '0');
						
            if (pBufferDesc->ReturnValue < 0)
              break;
          }
        }
      }
			
      if (pBufferDesc->ReturnValue >= 0) 
			{
        /* Print number without sign */
        _PrintUnsigned(pBufferDesc, (unsigned)v, Base, NumDigits, FieldWidth, FormatFlags);
      }
    }
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       SEGGER_RTT_vprintf
*
*  Function description
*    Stores a formatted string in SEGGER RTT control block.
*    This data is read by the host.
*
*  Parameters
*    BufferIndex  Index of "Up"-buffer to be used. (e.g. 0 for "Terminal")
*    sFormat      Pointer to format string
*    pParamList   Pointer to the list of arguments for the format string
*
*  Return values
*    >= 0:  Number of bytes which have been stored in the "Up"-buffer.
*     < 0:  Error
*/
int ty_vprintf(const char * sFormat, va_list * pParamList) 
{
  char c;
  int v;
  unsigned NumDigits;
  unsigned FormatFlags;
  unsigned FieldWidth;
	
	ty_printf_desc_t BufferDesc;
  char acBuffer[TY_PRINTF_BUFFER_SIZE];

  BufferDesc.pBuffer        = acBuffer;
  BufferDesc.BufferSize     = TY_PRINTF_BUFFER_SIZE;
  BufferDesc.Cnt            = 0u;
  BufferDesc.ReturnValue    = 0;

  do 
	{
    c = *sFormat;
    sFormat++;
		
		/*****  sFormat is NULL *******/
    if(c == 0u) 
		{
      break;
    }
    else if(c == '%') 
		{
      /* Filter out flags */

      FormatFlags = 0u;
      v = 1;
      do 
			{
        c = *sFormat;
        switch (c) 
				{
					case '-': FormatFlags |= TY_FORMAT_FLAG_LEFT_JUSTIFY; sFormat++; break;
					case '0': FormatFlags |= TY_FORMAT_FLAG_PAD_ZERO;     sFormat++; break;
					case '+': FormatFlags |= TY_FORMAT_FLAG_PRINT_SIGN;   sFormat++; break;
					case '#': FormatFlags |= TY_FORMAT_FLAG_ALTERNATE;    sFormat++; break;
					default:  v = 0; break;
        }
      }while (v);
      
      /* Filter out field with */
      
      FieldWidth = 0u;
      do 
			{
        c = *sFormat;
        if ((c < '0') || (c > '9'))
          break;

        sFormat++;
        FieldWidth = (FieldWidth * 10u) + ((unsigned)c - '0');
      } while (1);

      /* Filter out precision (number of digits to display) */
      
      NumDigits = 0u;
      c = *sFormat;
      if (c == '.') 
			{
        sFormat++;
        do
			  {
          c = *sFormat;
          if ((c < '0') || (c > '9')) {
            break;
          }
          sFormat++;
          NumDigits = NumDigits * 10u + ((unsigned)c - '0');
        } while (1);
      }
      
      /* Filter out length modifier */
      
      c = *sFormat;
      do 
			{
        if ((c == 'l') || (c == 'h')) 
				{
          c = *sFormat;
          sFormat++;
        } 
				else 
				{
          break;
        }
      } while (1);
      
      /* Handle specifiers */
      
      switch (c) 
			{
				case 'c': 
				{
					char c0;
					v = va_arg(*pParamList, int);
					c0 = (char)v;
					_StoreChar(&BufferDesc, c0);
					break;
				}
				
				case 'd':
					v = va_arg(*pParamList, int);
					_PrintInt(&BufferDesc, v, 10u, NumDigits, FieldWidth, FormatFlags);
					break;
				
				case 'u':
					v = va_arg(*pParamList, int);
					_PrintUnsigned(&BufferDesc, (unsigned)v, 10u, NumDigits, FieldWidth, FormatFlags);
					break;
				
				case 'x':
				case 'X':
					v = va_arg(*pParamList, int);
					_PrintUnsigned(&BufferDesc, (unsigned)v, 16u, NumDigits, FieldWidth, FormatFlags);
					break;
				
				case 's':
        {
          const char * s = va_arg(*pParamList, const char *);
          do 
					{
            c = *s;
            s++;
            if (c == '\0') {
              break;
            }
            _StoreChar(&BufferDesc, c);
          } while (BufferDesc.ReturnValue >= 0);
        }
        break;
				
				case 'p':
					v = va_arg(*pParamList, int);
					_PrintUnsigned(&BufferDesc, (unsigned)v, 16u, 8u, 8u, 0u);
					break;
				
				case '%':
					_StoreChar(&BufferDesc, '%');
					break;
				
				default:
					break;
      }
      sFormat++;
    } 
		else 
		{
      _StoreChar(&BufferDesc, c);
    }
  } while (BufferDesc.ReturnValue >= 0);

  if (BufferDesc.ReturnValue > 0) 
	{
    /* Write remaining data, if any */
    if (BufferDesc.Cnt != 0u) 
		{
      ty_write(acBuffer, BufferDesc.Cnt);
    }
		
    BufferDesc.ReturnValue += (int)BufferDesc.Cnt;
  }
	
  return BufferDesc.ReturnValue;
}

/*********************************************************************
*
*       ty_printf
*
*  Function description
*    Stores a formatted string in ty_write_cb.
*    This data is read by the host.
*
*  Parameters
*    sFormat      Pointer to format string, followed by the arguments for conversion
*
*  Return values
*    >= 0:  Number of bytes which have been send.
*     < 0:  Error
*
*  Notes
*    (1) Conversion specifications have following syntax:
*          %[flags][FieldWidth][.Precision]ConversionSpecifier
*    (2) Supported flags:
*          -: Left justify within the field width
*          +: Always print sign extension for signed conversions
*          0: Pad with 0 instead of spaces. Ignored when using '-'-flag or precision
*        Supported conversion specifiers:
*          c: Print the argument as one char
*          d: Print the argument as a signed integer
*          u: Print the argument as an unsigned integer
*          x: Print the argument as an hexadecimal integer
*          s: Print the string pointed to by the argument
*          p: Print the argument as an 8-digit hexadecimal integer. (Argument shall be a pointer to void.)
*/
uint32_t ty_printf(const char * sFormat, ...) 
{
  va_list ParamList;

  va_start(ParamList, sFormat);
  return ty_vprintf(sFormat, &ParamList);
}

void ty_printf_init(void *write_cb)
{
	ty_write = (ty_write_cb)write_cb;
}


/*************************** End of file ****************************/

XIncludeFile "defs.pbi"

Declare.s       util_ansi_to_unicode(*in)
DeclareCDLL.i   util_char_to_wchar(*in, *out, inlen.l, outlen.l)
DeclareCDLL.i   util_wchar_to_char(*in, *out, inlen.l, outlen.l)
DeclareCDLL.i   util_to_wchar(*in)
DeclareCDLL.i   util_to_char(*in)
DeclareCDLL.i   util_free_buffer(*buffer)
DeclareCDLL.i   util_rgbquad_to_24(*quad.RGBQUADA)
DeclareCDLL.i   util_rgbquad_to_32(*quad.RGBQUADA, alpha.a)
DeclareCDLL.i   util_translate_point(*bounds.RECT, *point.POINT, *result.POINT)
DeclareCDLL.i   util_translate_points(*bounds.RECT, *points.POINT, *results.POINT, count.l)
DeclareCDLL.i   util_is_white(color.l, requireFullAlpha.l = #False)

; Peek @*in as ascii string.
Procedure.s util_ansi_to_unicode(*in)
  ProcedureReturn PeekS(*in, -1, #PB_Ascii)
EndProcedure

; Convert a char string to a wchar string.
ProcedureCDLL.i util_char_to_wchar(*in, *out, inlen.l, outlen.l)
  If (inlen * 2 < outlen)
    ProcedureReturn 0 
  EndIf 
  
  ProcedureReturn PokeS(*out, PeekS(*in, inlen, #PB_Ascii))
EndProcedure

; Convert a wchar string to char string.
ProcedureCDLL.i util_wchar_to_char(*in, *out, inlen.l, outlen.l)
  If (inlen / 2 < outlen)
    ProcedureReturn 0
  EndIf 
  
  ProcedureReturn PokeS(*out, PeekS(*in, inlen, #PB_Unicode))
EndProcedure

; Allocate a new wchar string from an existing char string.
ProcedureCDLL.i util_to_wchar(*in)
  Protected length = MemoryStringLength(*in, #PB_Ascii)
  If (length = 0)
    ProcedureReturn AllocateMemory(2)
  EndIf 
  
  Protected *buffer = AllocateMemory(length * 2 + 2)
  PokeS(*buffer, PeekS(*in, -1, #PB_Ascii), -1, #PB_Unicode)
  ProcedureReturn *buffer 
EndProcedure

; Allocate a new char string from an existing wchar string.
ProcedureCDLL.i util_to_char(*in)
  Protected length = MemoryStringLength(*in, #PB_Unicode)
  If (length = 0)
    ProcedureReturn AllocateMemory(1)
  EndIf 
  
  Protected *buffer = AllocateMemory(length / 2 + 1)
  PokeS(*buffer, PeekS(*in, -1, #PB_Unicode), -1, #PB_Ascii)
  ProcedureReturn *buffer 
EndProcedure

; Free a buffer previously allocated by any of the library functions
ProcedureCDLL.i util_free_buffer(*buffer)
  If (*buffer)
    FreeMemory(*buffer)
  EndIf 
EndProcedure

; Convert a RGBQUAD value to a color value that can be used with PB
ProcedureCDLL.i util_rgbquad_to_24(*quad.RGBQUADA)
  ProcedureReturn RGB(*quad\Red, *quad\Green, *quad\Blue)
EndProcedure

; Convert a RGBQUAD value to a color value that can be used with PB
ProcedureCDLL.i util_rgbquad_to_32(*quad.RGBQUADA, alpha.a)
  ProcedureReturn RGBA(*quad\Red, *quad\Green, *quad\Blue, alpha)
EndProcedure

; Translate a point to an absolute coordinate system using a bounds rect
ProcedureCDLL.i util_translate_point(*bounds.RECT, *point.POINT, *result.POINT)
  *result\x = *point\x + *bounds\left
  *result\y = *point\y + *bounds\top 
EndProcedure

; Translate a list of points to an absolute coordinate system using a bounds rect 
ProcedureCDLL.i util_translate_points(*bounds.RECT, *points.POINT, *results.POINT, count.l)
  Protected *a.POINT
  Protected *b.POINT 
  
  For current = 0 To count - 1 
    *a = *points  + SizeOf(POINT) * current 
    *b = *results + SizeOf(POINT) * current 
    util_translate_point(*bounds, *a, *b)
  Next 
EndProcedure

; Determine if a color can be considered white 
ProcedureCDLL.i util_is_white(color.l, requireFullAlpha.l = #False)
  If (requireFullAlpha)
    ProcedureReturn Bool(Red(color) = 255 And Green(color) = 255 And Blue(color) = 255 And Alpha(color) = 255)
  Else 
    ProcedureReturn Bool(Red(color) = 255 And Green(color) = 255 And Blue(color) = 255)
  EndIf 
EndProcedure
; IDE Options = PureBasic 5.72 (Windows - x64)
; CursorPosition = 96
; FirstLine = 54
; Folding = --
; EnableXP
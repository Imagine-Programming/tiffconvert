XIncludeFile "defs.pbi"
XIncludeFile "utilities.pbi"

Declare.i       font_create_flags(bold.l, italic.l, underline.l, strikeout.l, antialias.l)
Declare.i       font_open(szFontName.s, height.l, bold.l, italic.l, underline.l, strikeout.l, antialias.l)
DeclareCDLL.i   font_open_a(*fontname, height.l, bold.l, italic.l, underline.l, strikeout.l, antialias.l)
DeclareCDLL.i   font_open_w(*fontname, height.l, bold.l, italic.l, underline.l, strikeout.l, antialias.l)
DeclareCDLL.i   font_close(*font.font_handle)

; create an integer representing the options specified by combining them as flags.
Procedure.i font_create_flags(bold.l, italic.l, underline.l, strikeout.l, antialias.l)
  Protected flags = 0
  
  If (bold)       : flags | #PB_Font_Bold        : EndIf 
  If (italic)     : flags | #PB_Font_Italic      : EndIf 
  If (underline)  : flags | #PB_Font_Underline   : EndIf 
  If (strikeout)  : flags | #PB_Font_StrikeOut   : EndIf 
  If (antialias)  : flags | #PB_Font_HighQuality : EndIf 
  
  ProcedureReturn flags 
EndProcedure

; open a font and return a new font_handle 
Procedure.i font_open(szFontName.s, height.l, bold.l, italic.l, underline.l, strikeout.l, antialias.l)
  Protected flags = font_create_flags(bold, italic, underline, strikeout, antialias)
  Protected hFont = LoadFont(#PB_Any, szFontName, height, flags)
  
  If (Not hFont) 
    ProcedureReturn #Null
  EndIf 
  
  Protected *font.font_handle = AllocateMemory(SizeOf(font_handle))
  If (Not *font)
    FreeFont(hFont)
    ProcedureReturn #Null 
  EndIf 
  
  With *font 
    \hFont = hFont 
  EndWith
  
  ProcedureReturn *font
EndProcedure

; open a font from ascii name
ProcedureCDLL.i font_open_a(*fontname, height.l, bold.l, italic.l, underline.l, strikeout.l, antialias.l)
  ProcedureReturn font_open(util_ansi_to_unicode(*fontname), height, bold, italic, underline, strikeout, antialias)
EndProcedure

; open a font from unicode name
ProcedureCDLL.i font_open_w(*fontname, height.l, bold.l, italic.l, underline.l, strikeout.l, antialias.l)
  ProcedureReturn font_open(PeekS(*fontname), height, bold, italic, underline, strikeout, antialias)
EndProcedure

; close a previously opened font handle
ProcedureCDLL.i font_close(*font.font_handle)
  If (IsFont(*font\hFont))
    FreeFont(*font\hFont)
  EndIf 
  
  FreeMemory(*font)
EndProcedure
; IDE Options = PureBasic 5.71 LTS (Windows - x64)
; CursorPosition = 54
; FirstLine = 12
; Folding = -
; EnableXP
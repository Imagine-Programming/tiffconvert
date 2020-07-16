; CatchTIFFPage functions by the great wilbert: https://www.purebasic.fr/english/viewtopic.php?f=13&t=58880#p441476
; I did not know the TIFF codec in PureBasic worked this way, and that loading an individual page was this easy. 
; Thanks Wilbert for showing that to the PB forum!

Procedure.w SwapBytesW(v.w)
  !movzx eax, word [p.v_v]
  !xchg al, ah
  ProcedureReturn
EndProcedure

Procedure.l SwapBytesL(v.l)
  !mov eax, [p.v_v]
  !bswap eax
  ProcedureReturn
EndProcedure

Procedure.i CatchTIFFPage(Image, *MemoryAddress.Word, Page = 0)
  Protected.i Result, Count, *Entries.Word
  Protected *IFD0.Long = *MemoryAddress + 4
  Protected *IFD.Long  = *IFD0
  
  While *IFD\l
    If Count = Page
      Swap *IFD0\l, *IFD\l
      Result = CatchImage(Image, *MemoryAddress)
      Swap *IFD0\l, *IFD\l
      
      Break
    EndIf
    
    Count + 1 
    
    If *MemoryAddress\w = $4D4D
      ; big endian
      *Entries = *MemoryAddress + SwapBytesL(*IFD\l)
      *IFD = *Entries + 2 + SwapBytesW(*Entries\w) * 12
    Else
      ; little endian
      *Entries = *MemoryAddress + *IFD\l
      *IFD = *Entries + 2 + *Entries\w * 12
    EndIf
  Wend
  
  If Page >= 0
    ProcedureReturn Result
  Else
    ProcedureReturn Count
  EndIf
EndProcedure

; Simple procedure for reading a file to a buffer in full
Procedure.i ReadFileToMemory(szPath.s)
  Protected hFile = ReadFile(#PB_Any, szPath)
  If (Not hFile)
    ProcedureReturn #Null
  EndIf 
  
  Protected *buffer = AllocateMemory(Lof(hFile))
  If (Not *buffer)
    CloseFile(hFile)
    ProcedureReturn #Null 
  EndIf 
  
  ReadData(hFile, *buffer, MemorySize(*buffer))
  CloseFile(hFile)
  ProcedureReturn *buffer
EndProcedure

;------------------------------------
; https://www.purebasic.fr/english/viewtopic.php?f=13&t=43570#p334079
;
;Bresenhams Line Drawing Algorithm

;The Bresenham line drawing algorithm uses 
; integer addition And subtraction ONLY
; NO multiplication, NO division, NO sines or cosines
;------------------------------------

Procedure thickLineXY (start_x,start_y,end_x,end_y,thickness,whatcolor)
  err_x = 0 : err_y = 0 : inc_x = 0 : inc_y = 0
  
  delta_x = end_x - start_x;
  delta_y = end_y - start_y;
  
  
  If(delta_x > 0) : inc_x = 1 : ElseIf (delta_x = 0) : inc_x = 0 : Else : inc_x = -1 : EndIf 
  If(delta_y > 0) : inc_y = 1 : ElseIf (delta_y = 0) : inc_y = 0 : Else : inc_y = -1 : EndIf         
  
  delta_x = Abs(delta_x);
  delta_y = Abs(delta_y);
  
  If(delta_x > delta_y)
    distance = delta_x;
  Else
    distance = delta_y;
  EndIf 
  
  For  xyz = 0 To  distance+1 Step 1
    ; modified to place a circle at the pixel location to get a thick line
    
    ;Plot(start_x,start_y,whatcolor)     
    Circle(start_x,start_y,thickness,whatcolor)     
    
    err_x = err_x + delta_x
    err_y = err_y + delta_y
    
    If (err_x > distance)
      err_x = err_x - distance
      start_x = start_x + inc_x
    EndIf
    
    If (err_y > distance)
      err_y = err_y - distance
      start_y = start_y +inc_y
    EndIf 
  Next
EndProcedure

; Michael Vogel - https://www.purebasic.fr/english/viewtopic.php?f=13&t=43570#p333664
Procedure Liner(x1,y1,x2,y2,r=1,color=#Black)
    Protected n.f
    Protected c.f
    Protected x
    Protected y

    LineXY(x1,y1,x2,y2,color)
    If r>1 And r<180
        c=360/r/#PI/2/6
        n=0
        While n<360
            x=Sin(n/180*#PI)*r
            y=Cos(n/180*#PI)*r
            LineXY(x1+x,y1+y,x2+x,y2+y,color)
            n+c
        Wend
    EndIf
EndProcedure
; IDE Options = PureBasic 5.72 (Windows - x64)
; CursorPosition = 136
; FirstLine = 88
; Folding = --
; EnableXP
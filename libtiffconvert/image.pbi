XIncludeFile "defs.pbi"
XIncludeFile "RotateImage.pbi"

DeclareCDLL.i image_open_p(*imagedata, size.l)
DeclareCDLL.i image_free(*handle.image_handle)
DeclareCDLL.i image_clone(*handle.image_handle)

DeclareCDLL.i image_rotate_fixed(*handle.image_handle, mode.l)
DeclareCDLL.i image_rotate_free(*handle.image_handle, degrees.f, antiAlias.l, fillColor.l)
DeclareCDLL.i image_mirror(*handle.image_handle, axis.l)

; open an image from encoded image data and return an image handle
ProcedureCDLL.i image_open_p(*imagedata, size.l)
  Protected hImage = CatchImage(#PB_Any, *imagedata, size)
  If (Not hImage)
    ProcedureReturn #Null 
  EndIf 
  
  Protected *handle.image_handle = AllocateMemory(SizeOf(image_handle))
  If (Not *handle)
    FreeImage(hImage)
    ProcedureReturn #Null 
  EndIf 
  
  *handle\hImage = hImage
  ProcedureReturn *handle 
EndProcedure

; free a previously loaded image
ProcedureCDLL.i image_free(*handle.image_handle)
  If (IsImage(*handle\hImage))
    FreeImage(*handle\hImage)
  EndIf 
  
  FreeMemory(*handle)
EndProcedure

; clone an image, effectively creating a new instance
ProcedureCDLL.i image_clone(*handle.image_handle)
  If (Not IsImage(*handle\hImage))
    ProcedureReturn #Null 
  EndIf 
  
  Protected hImageCopy = CopyImage(*handle\hImage, #PB_Any)
  If (Not hImageCopy)
    ProcedureReturn #Null 
  EndIf 
  
  Protected *copy.image_handle = AllocateMemory(SizeOf(image_handle))
  If (Not *copy)
    FreeImage(hImageCopy)
    ProcedureReturn #Null 
  EndIf 
  
  *copy\hImage = hImageCopy
  ProcedureReturn *copy 
EndProcedure

; clone an image and rotate it by a fixed option
ProcedureCDLL.i image_rotate_fixed(*handle.image_handle, mode.l)
  If (Not IsImage(*handle\hImage))
    ProcedureReturn 0 
  EndIf 
  
  Protected degrees = 0
  Select mode 
    Case #ROTATE_90
      degrees = 90
    Case #ROTATE_180 
      degrees = 180
    Case #ROTATE_270
      degrees = 270
    Default 
      ProcedureReturn #Null 
  EndSelect
  
  Protected hRotated = RotateImage(*handle\hImage, degrees)
  If (Not hRotated)
    ProcedureReturn #Null 
  EndIf 
  
  Protected *copy.image_handle = AllocateMemory(SizeOf(image_handle))
  If (Not *copy)
    FreeImage(hRotated)
    ProcedureReturn #Null 
  EndIf 
  
  *copy\hImage = hRotated
  
  ProcedureReturn *copy 
EndProcedure

; clone an image and rotate it by any amount
ProcedureCDLL.i image_rotate_free(*handle.image_handle, degrees.f, antiAlias.l, fillColor.l)
  If (degrees = 0.0) ; no need to check for 90, 180 and 270. RotateImageFree does that for us.
    ProcedureReturn image_clone(*handle)
  EndIf 
  
  If (Not IsImage(*handle\hImage))
    ProcedureReturn 0 
  EndIf 
  
  Protected hRotated = RotateImageFree(*handle\hImage, degrees, antiAlias, fillColor)
  If (Not hRotated)
    ProcedureReturn #Null 
  EndIf 
  
  Protected *copy.image_handle = AllocateMemory(SizeOf(image_handle))
  If (Not *copy)
    FreeImage(hRotated)
    ProcedureReturn #Null 
  EndIf 
  
  *copy\hImage = hRotated
  
  ProcedureReturn *copy
EndProcedure

; clone an image and mirror it
ProcedureCDLL.i image_mirror(*handle.image_handle, axis.l)
  If (Not IsImage(*handle\hImage))
    ProcedureReturn #Null 
  EndIf 
  
  If (axis <> #MIRROR_HORIZONTAL And axis <> #MIRROR_VERTICAL)
    ProcedureReturn #Null 
  EndIf 
  
  Protected hMirrored
  If (axis = #MIRROR_HORIZONTAL)
    hMirrored = FlipImage(*handle\hImage)
  Else 
    hMirrored = MirrorImage(*handle\hImage)
  EndIf 
  
  If (Not hMirrored)
    ProcedureReturn #Null 
  EndIf 
  
  Protected *copy.image_handle = AllocateMemory(SizeOf(image_handle))
  If (Not *copy)
    FreeImage(hMirrored)
    ProcedureReturn #Null 
  EndIf 
  
  *copy\hImage = hMirrored
  
  ProcedureReturn *copy
EndProcedure




; IDE Options = PureBasic 5.71 LTS (Windows - x64)
; CursorPosition = 118
; FirstLine = 104
; Folding = --
; EnableXP
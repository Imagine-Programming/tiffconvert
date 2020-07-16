; the tiff file header
Structure tiff_header
  ByteOrder.u   ; byte order mark ("II" or "MM")
  Magic.u       ; Magic number (42)
  IFD0.l        ; Offset to the first image file directory
EndStructure

; represents a loaded tiff image
Structure tiff_image Align #PB_Structure_AlignC
  Filepath.s              ; path to file 
  *RawData.tiff_header    ; the buffer holding the full tiff image
  PageCount.l             ; the number of IFDs 
  ReleaseRaw.l            ; whether or not to free the raw data
  Array PageHandles.i(0)  ; an array of image handles for each IFD
EndStructure

; represents a loaded font
Structure font_handle Align #PB_Structure_AlignC
  hFont.i
EndStructure

; represents a decoded image
Structure image_handle Align #PB_Structure_AlignC
  hImage.i
EndStructure

; RGBQUADA where A indicates ascii (uint8_t), see https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-rgbquad
Structure RGBQUADA
  Blue.a
  Green.a
  Red.a
  Reserved.a
EndStructure

; LOGFONTA, see https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-logfonta
Structure LOGFONTA Align #PB_Structure_AlignC
  lfHeight.l
  lfWidth.l
  lfEscapement.l
  lfOrientation.l
  lfWeight.l
  lfItalic.a
  lfUnderline.a
  lfStrikeOut.a
  lfCharSet.a
  lfOutPrecision.a
  lfClipPrecision.a
  lfQuality.a
  lfPitchAndFamily.a
  lfFaceName.a[#LF_FACESIZE]
EndStructure

; possible codecs to use for exporting a tiff page
Enumeration tiff_export_format
  #TIFF_EXPORT_PNG
  #TIFF_EXPORT_JPEG
  #TIFF_EXPORT_JPEG2000
  #TIFF_EXPORT_BITMAP
EndEnumeration

; possible modes for the fixed image rotation
Enumeration image_rotation
  #ROTATE_90
  #ROTATE_180
  #ROTATE_270
EndEnumeration

; possible modes for image mirroring
Enumeration image_mirror
  #MIRROR_HORIZONTAL
  #MIRROR_VERTICAL
EndEnumeration
; IDE Options = PureBasic 5.71 LTS (Windows - x64)
; CursorPosition = 67
; FirstLine = 22
; EnableXP
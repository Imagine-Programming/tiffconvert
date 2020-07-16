#pragma once
#include "pch.h"

#ifndef libtifwang_h
#define libtifwang_h

#include <filesystem>
#include <functional>
#include <fstream>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#ifdef LIBTIFWANG_EXPORT
# define __EXPORTED_API __declspec(dllexport)
#else 
# define __EXPORTED_API __declspec(dllimport)
#endif 

/// <summary>
/// An enum describing tag IDs in Tiff IFD entries.
/// </summary>
enum class TiffTagId : uint16_t {
	TIFF_WANG_TAG			   = 0x80a4,
	TIFF_IMAGE_WIDTH_TAG	   = 0x0100,
	TIFF_IMAGE_LENGTH_TAG	   = 0x0101,
	TIFF_IMAGE_XRESOLUTION	   = 0x011A,
	TIFF_IMAGE_YRESOLUTION	   = 0x011B,
	TIFF_IMAGE_RESOLUTION_UNIT = 0x0128,
	TIFF_PAGE_NUMBER		   = 0x0129,
	TIFF_IMAGE_SOFTWARE        = 0x0131,
	TIFF_IMAGE_DATETIME        = 0x0132,
	TIFF_IMAGE_ARTIST          = 0x013B,
};

/// <summary>
/// An enum describing the type of data in a Tiff IFD tag.
/// </summary>
enum class TiffTagType : uint16_t {
	BYTE = 1,		// BYTE 8-bit unsigned integer.
	ASCII,			// ASCII 8-bit byte that contains a 7-bit ASCII code; the last byte must be NUL (binary zero).
	SHORT,			// SHORT 16-bit (2-byte) unsigned integer.
	LONG,			// LONG 32-bit (4-byte) unsigned integer.
	RATIONAL		// RATIONAL Two LONGs: the first represents the numerator of a fraction; the second, the denominator.
};

/// <summary>
/// An enum describing the resolution units in Tiff IFDs.
/// </summary>
enum class TiffResolutionUnit : uint16_t {
	NoAbsoluteMeasurement = 1,
	Inch,
	Centimeter
};

_Check_return_ float _cdecl _byteswap_float(_In_ float _Number);
_Check_return_ double _cdecl _byteswap_double(_In_ double _Number);

#if REG_DWORD == REG_DWORD_LITTLE_ENDIAN
# define le_to_host_ulong(VAL) VAL
# define be_to_host_ulong(VAL) _byteswap_ulong(VAL)
# define le_to_host_ushort(VAL) VAL
# define be_to_host_ushort(VAL) _byteswap_ushort(VAL)
# define le_to_host_uint64(VAL) VAL
# define be_to_host_uint64(VAL) _byteswap_uint64(VAL)
# define le_to_host_float(VAL) VAL
# define be_to_host_float(VAL) _byteswap_float(VAL)
# define le_to_host_double(VAL) VAL
# define be_to_host_double(VAL) _byteswap_double(VAL)
#else
# define le_to_host_ulong(VAL) _byteswap_ulong(VAL)
# define be_to_host_ulong(VAL) VAL
# define le_to_host_ushort(VAL) _byteswap_ushort(VAL)
# define be_to_host_ushort(VAL) VAL
# define le_to_host_uint64(VAL) _byteswap_uint64(VAL)
# define be_to_host_uint64(VAL) VAL
# define le_to_host_float(VAL) _byteswap_float(VAL)
# define be_to_host_float(VAL) VAL
# define le_to_host_double(VAL) _byteswap_double(VAL)
# define be_to_host_double(VAL) VAL
#endif

/// <summary>
/// An enum describing the eiStream/Wang integer encoding.
/// </summary>
enum class TiffWangIntegerMode : uint32_t {
    Intel16Bit,
    Intel32Bit
};

/// <summary>
/// An enum describing the data type in an eiStream/Wang tag.
/// </summary>
enum class TiffWangDataType : uint32_t {
    GLOBAL_NAMED_BLOCK      = 2,
    ATTRIBUTE_DATA          = 5,
    LOCAL_NAMED_BLOCK       = 6
};

/// <summary>
/// A TiffWangEntry contains the data type and size.
/// </summary>
struct TiffWangEntry {
    TiffWangDataType DataType;
    uint32_t         DataSize;
};

/// <summary>
/// A TiffWangNamedBlock contains the name and size of a local or global named block.
/// </summary>
struct TiffWangNamedBlock {
    char        Name[8] = { 0 };
    uint32_t    Size = 0;
    // ... data
};

/// <summary>
/// The annotation mark types supported by eiStream/Wang and this library.
/// </summary>
enum class OAIN_MARK_TYPE : UINT {
    ImageEmbedded = 1,
    ImageReference,
    StraightLine,
    FreehandLine,
    HollowRectangle,
    FilledRectangle,
    TypedText,
    TextFromFile,
    TextStamp,
    AttachANote,
    Form = 12,
    OCRRegion
};

/// <summary>
/// The type of rotation in an AN_NEW_ROTATE_STRUCT struct.
/// </summary>
enum class AN_ROTATE_TYPE : int {
    Original = 1,
    RotateRight,
    Flip,
    RotateLeft,
    VerticalMirror,
    VerticalMirrorRotateRight,
    VerticalMirrorFlip,
    VerticalMirrorRotateLeft
};

/// <summary>
/// The OIAN_MARK_ATTRIBUTES struct describes an annotation mark.
/// </summary>
struct OIAN_MARK_ATTRIBUTES {
    OAIN_MARK_TYPE uType;    // The type of the mark.
                             // 1 = Image embedded
                             // 2 = Image reference
                             // 3 = Straight line
                             // 4 = Freehand line
                             // 5 = Hollow rectangle
                             // 6 = Filled rectangle
                             // 7 = Typed text
                             // 8 = Text from file
                             // 9 = Text stamp
                             //10 = Attach-a-Note
                             //12 = Form
                             //13 = OCR region
    RECT lrBounds;           // Rectangle in FULLSIZE units; equivalent to type RECT.
                             // Can be a rectangle or two points.
    RGBQUAD rgbColor1;       // The main color; for example, the color of all lines,
                             // all rectangles, and standalone text.
    RGBQUAD rgbColor2;       // The secondary color; for example, the color of the text of
                             // an Attach-a-Note.
    BOOL bHighlighting;      // TRUE ¾ The mark is drawn highlighted. Highlighting
                             // performs the same function as a highlighting marker on a
                             // piece of paper. Valid only for lines, rectangles, and
                             // freehand.
    BOOL bTransparent;       // TRUE ¾ The mark is drawn transparent. A transparent
                             // mark does not draw white pixels. That is, transparent
                             // replaces white pixels with whatever is behind those pixels.
                             // Available only for images.
    UINT uLineSize;          // The width of the line in pixels.
    UINT uReserved1;         // Reserved; must be set to 0.
    UINT uReserved2;         // Reserved; must be set to 0.
    LOGFONTA lfFont;         // The font information for the text, consisting of standard
                             // font attributes of font size, name, style, effects, and
                             // background color.
    DWORD bReserved3;        // Reserved; must be set to 0.
    uint32_t Time;           // The time that the mark was first saved, in seconds, from 
                             // 00:00:00 1-1-1970 GMT. Every annotation mark has
                             // time as one of its attributes. If you do not set the time before
                             // the file is saved, the time is set to the date and time that the
                             // save was initiated. This time is in the form returned by the
                             // "time" C call, which is the number of seconds since
                             // midnight 00:00:00 on 1-1-1970 GMT. If necessary, refer
                             // to your C documentation for a more detailed description. (was time_t)
    BOOL bVisible;           // TRUE ¾ The mark is currently set to be visible. Annotation marks can be visible or hidden.
    DWORD dwReserved4;       // Reserved; must be set to 0x0FF83F.
    long lReserved[10];      // Must be set to 0.
};

/// <summary>
/// AN_POINTS describes a collection of points, in the stream the nPoints*sizeof(POINT) will follow.
/// </summary>
struct AN_POINTS {
    int nMaxPoints;          // The maximum number of points; must
                             // be equal to the value of nPoints.
    int nPoints;             // The current number of points.
};

/// <summary>
/// AN_NEW_ROTATE_STRUCT describes the rotation of a mask or image.
/// </summary>
struct AN_NEW_ROTATE_STRUCT {

    AN_ROTATE_TYPE rotation; // 1=Original
                             // 2=Rotate right (90 degrees clockwise)
                             // 3=Flip (180 degrees clockwise)
                             // 4=Rotate left (270 degrees clockwise)
                             // 5=Vertical mirror (reflected around a
                             // vertical line)
                             // 6=Vertical mirror + Rotate right
                             // 7=Vertical mirror + Flip
                             // 8=Vertical mirror + Rotate left
    int scale;               // Set to 1000.
    int nHRes;               // Set to value of nOrigHRes.
    int nVRes;               // Set to value of nOrigVRes.
    int nOrigHRes;           // Resolution of image mark in DPI.
    int nOrigVRes;           // Resolution of image mark in DPI.
    BOOL bReserved1;         // Set to 0.
    BOOL bReserved2;         // Set to 0.
    int nReserved[6];
};

/// <summary>
/// OIAN_TEXTPRIVDATA describes the scale of a font, length of text and orientation of text.
/// </summary>
struct OIAN_TEXTPRIVDATA {
    int nCurrentOrientation; // Angle of text baseline to image
                             // in tenths of a degree; valid
                             // values are 0, 900, 1800, 2700.
    UINT uReserved1;         // Always 1000 when writing;
                             // ignore when reading.
    UINT uCreationScale;     // Always 72000 divided by
                             // the vertical resolution of the
                             // base image when writing;
                             // Used to modify the
                             // Attributes.lfFont.lfHeight
                             // variable for display.
    UINT uAnoTextLength;     // 64K byte limit (32K for multi-
                             // byte data) for Attach-a-Note,
                             // typed text, text from file;
                             // 255 byte limit for text stamp.
    // char szAnoText[1];       // Text string for text mark types.
};

/// <summary>
/// HYPERLINK_NB describes the hyperlink of a certain mark. Not supported by this library (yet?)
/// Burning the annotation marks onto the image makes hyperlinks impossible, mybe in a future PDF 
/// rendering solution we can implement this as well.
/// </summary>
struct HYPERLINK_NB {
    int             nVersion;
    int             nLinkSize;
    std::wstring    szLinkString;
    int             nLocationSize;
    std::wstring    szLocationString;
    int             nWorkDirSize;
    std::wstring    szWorkDirString;
    int             nFlags;
};

/// <summary>
/// A convenience macro to define a flag in an enum, by setting a specific bit.
/// </summary>
#define __flag(name, bit) name = (1 << (bit))

/// <summary>
/// Flags to use to describe which properties are set in the annotation mark state.
/// </summary>
enum class TiffWangMarkSet : uint64_t {
    NoFlagsSet = 0,

    __flag(GlobalGroupSet,        0),
    __flag(LocalGroupSet,         1),
    __flag(GlobalFilenameSet,     2),
    __flag(LocalFilenameSet,      3),
    __flag(GlobalDibInfoSet,      4),
    __flag(LocalDibInfoSet,       5),
    __flag(GlobalAsciiTextSet,    6),
    __flag(LocalAsciiTextSet,     7),
    __flag(GlobalUnicodeTextSet,  8),
    __flag(LocalUnicodeTextSet,   9),
    __flag(AttributesSet,         10),
    __flag(GlobalPointsSet,       11),
    __flag(LocalPointsSet,        12),
    __flag(GlobalRotationSet,     13),
    __flag(LocalRotationSet,      14),
    __flag(GlobalHyperlinkSet,    15),
    __flag(LocalHyperlinkSet,     16),
    __flag(GlobalIndexSet,        17),
    __flag(LocalIndexSet,         18)
};

/// <summary>
/// TiffWangMarkProperties describes a collection of local or global properties for a 
/// eiStream/Wang annotation mark. The purpose is to use only the local properties when
/// emitting a mark, but to use global properties to initialize the local properties on 
/// each new mark.
/// </summary>
struct TiffWangMarkProperties {
    std::string             Group;          /* OiGroup */
    std::string             Index;          /* OiIndex */
    std::string             FileName;       /* OiFilNam */
    std::vector<uint8_t>    DibInfo;        /* OiDib */
    std::string             AsciiText;      /* when OIAN_TEXTPRIVDATA is about Ascii-text */
    std::wstring            UnicodeText;    /* when OIAN_TEXTPRIVDATA is about Unicode-text */

    OIAN_MARK_ATTRIBUTES    Attributes;     /* mark attributes */
    AN_POINTS               Points;         /* for lines */
    std::vector<POINT>      PointList;      /* size determined by Points */
    AN_NEW_ROTATE_STRUCT    Rotation;       /* for images */
    OIAN_TEXTPRIVDATA       Text;           /* for text annotations */
    HYPERLINK_NB            Hyperlink;      /* read manually, for hyperlinks */
};

/// <summary>
/// TiffWangMark describes the state of the current mark that is to be emitted. This mark consists 
/// of global and local properties. The global properties always stay the same, unless overwritten.
/// The local properties get reset after each emit of a mark, and the global properties are then used
/// to initialize a new local property state.
/// </summary>
class TiffWangMark {
    private:
        TiffWangMarkProperties      m_Local;        // the local properties
        TiffWangMarkProperties      m_Global;       // the global properties
        OIAN_MARK_ATTRIBUTES        m_Attributes;   // the annotation mark, know as mark attributes
        uint64_t                    m_SetProperties = static_cast<uint64_t>(TiffWangMarkSet::NoFlagsSet); // track which properties are set.

    public:
        /// <summary>
        /// Get a reference to the attributes instance, to read from or write to it.
        /// </summary>
        /// <returns>a reference to the attributes instance.</returns>
        OIAN_MARK_ATTRIBUTES& Attributes();

        /// <summary>
        /// Determine if the mark attributes have been set at least once.
        /// </summary>
        /// <returns>true when a new annotation mark has been encountered at least once.</returns>
        bool HasAttributes() const;

        /// <summary>
        /// Declare that mark attributes have been encountered at least once.
        /// </summary>
        void SetAttributes();

        /// <summary>
        /// Reset the local property flags and assign the global properties that are set to the local properties.
        /// </summary>
        void AssignGlobalToLocal();

        /// <summary>
        /// Determine if a certain property is set.
        /// </summary>
        /// <param name="p">The property to check for.</param>
        /// <returns>true when the property p is set.</returns>
        bool IsSet(TiffWangMarkSet p) const;

        /// <summary>
        /// Determine if any of the properties in flags is set, similar to <see cref="IsSet"/> but for multiple properties.
        /// </summary>
        /// <param name="flags">An initializer list of properties to check for.</param>
        /// <returns>true when any of the properties is set.</returns>
        bool AnySet(std::initializer_list<TiffWangMarkSet> flags) const;

        /// <summary>
        /// Mark property p as set.
        /// </summary>
        /// <param name="p">The property to mark as set.</param>
        void Set(TiffWangMarkSet p);

        /// <summary>
        /// Get either the local or global group, depending on the set flags.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
        const std::string& Group() const;

        /// <summary>
        /// Get either the local or global index, depending on the set flags.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
        const std::string& Index() const;

        /// <summary>
        /// Get either the local or global filename, depending on the set flags.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
        const std::string& FileName() const;

        /// <summary>
        /// Get either the local or global DIB info, depending on the set flags.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
        const std::vector<uint8_t>& DibInfo() const;

        /// <summary>
        /// Get either the local or global ascii text, depending on the set flags.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
        const std::string& AsciiText() const;

        /// <summary>
        /// Get either the local or global unicode text, depending on the set flags.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
        const std::wstring& UnicodeText() const;

        /// <summary>
        /// Get either the local or global points struct, depending on the set flags.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
        const AN_POINTS& Points() const;

        /// <summary>
        /// Get either the local or global point list, depending on the set flags.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
        const std::vector<POINT>& PointList() const;

        /// <summary>
        /// Get either the local or global rotation struct, depending on the set flags.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
        const AN_NEW_ROTATE_STRUCT& Rotation() const;

        /// <summary>
        /// Get either the local or global text struct, depending on the global state.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        const OIAN_TEXTPRIVDATA& Text(bool global) const;

        /// <summary>
        /// Get either the local or global hyperlink struct, depending on the set flags.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
        const HYPERLINK_NB& HyperLink() const;

        /// <summary>
        /// Get the local group.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
        const std::string& LocalGroup() const;

        /// <summary>
        /// Get the local index.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
        const std::string& LocalIndex() const;

        /// <summary>
        /// Get the local filename.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
        const std::string& LocalFileName() const;

        /// <summary>
        /// Get the local DIB info.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
        const std::vector<uint8_t>& LocalDibInfo() const;

        /// <summary>
        /// Get the local ascii text.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
        const std::string& LocalAsciiText() const;

        /// <summary>
        /// Get the local unicode text.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
        const std::wstring& LocalUnicodeText() const;

        /// <summary>
        /// Get the local points struct.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
        const AN_POINTS& LocalPoints() const;

        /// <summary>
        /// Get the local points list.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
        const std::vector<POINT>& LocalPointList() const;

        /// <summary>
        /// Get the local rotation struct.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
        const AN_NEW_ROTATE_STRUCT& LocalRotation() const;

        /// <summary>
        /// Get the local text struct.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
        const OIAN_TEXTPRIVDATA& LocalText() const;

        /// <summary>
        /// Get the local hyperlink.
        /// </summary>
        /// <returns>A const reference to the property value.</returns>
        /// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
        const HYPERLINK_NB& LocalHyperLink() const;

        /// <summary>
        /// Get the non-const reference to either the local or global group.
        /// </summary>
        /// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
        /// <returns>A non-const reference to the property value.</returns>
        std::string& GroupSetter(bool global);

        /// <summary>
        /// Get the non-const reference to either the local or global index.
        /// </summary>
        /// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
        /// <returns>A non-const reference to the property value.</returns>
        std::string& IndexSetter(bool global);

        /// <summary>
        /// Get the non-const reference to either the local or global filename.
        /// </summary>
        /// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
        /// <returns>A non-const reference to the property value.</returns>
        std::string& FileNameSetter(bool global);

        /// <summary>
        /// Get the non-const reference to either the local or global DIB info.
        /// </summary>
        /// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
        /// <returns>A non-const reference to the property value.</returns>
        std::vector<uint8_t>& DibInfoSetter(bool global);

        /// <summary>
        /// Get the non-const reference to either the local or global ascii text.
        /// </summary>
        /// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
        /// <returns>A non-const reference to the property value.</returns>
        std::string& AsciiTextSetter(bool global);

        /// <summary>
        /// Get the non-const reference to either the local or global unicode text.
        /// </summary>
        /// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
        /// <returns>A non-const reference to the property value.</returns>
        std::wstring& UnicodeTextSetter(bool global);

        /// <summary>
        /// Get the non-const reference to either the local or global points struct.
        /// </summary>
        /// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
        /// <returns>A non-const reference to the property value.</returns>
        AN_POINTS& PointsSetter(bool global);

        /// <summary>
        /// Get the non-const reference to either the local or global point list.
        /// </summary>
        /// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
        /// <returns>A non-const reference to the property value.</returns>
        std::vector<POINT>& PointListSetter(bool global);

        /// <summary>
        /// Get the non-const reference to either the local or global rotate struct.
        /// </summary>
        /// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
        /// <returns>A non-const reference to the property value.</returns>
        AN_NEW_ROTATE_STRUCT& RotationSetter(bool global);

        /// <summary>
        /// Get the non-const reference to either the local or global text struct.
        /// </summary>
        /// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
        /// <returns>A non-const reference to the property value.</returns>
        OIAN_TEXTPRIVDATA& TextSetter(bool global);

        /// <summary>
        /// Get the non-const reference to either the local or global hyperlink struct.
        /// </summary>
        /// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
        /// <returns>A non-const reference to the property value.</returns>
        HYPERLINK_NB& HyperLinkSetter(bool global);
};

#endif 
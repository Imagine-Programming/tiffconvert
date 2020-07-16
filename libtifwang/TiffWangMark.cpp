#include "pch.h"
#include "libtiffwang.h"

/// <summary>
/// Get a reference to the attributes instance, to read from or write to it.
/// </summary>
/// <returns>a reference to the attributes instance.</returns>
OIAN_MARK_ATTRIBUTES& TiffWangMark::Attributes() {
	return m_Attributes;
}

/// <summary>
/// Determine if the mark attributes have been set at least once.
/// </summary>
/// <returns>true when a new annotation mark has been encountered at least once.</returns>
bool TiffWangMark::HasAttributes() const {
	return IsSet(TiffWangMarkSet::AttributesSet);
}

/// <summary>
/// Declare that mark attributes have been encountered at least once.
/// </summary>
void TiffWangMark::SetAttributes() {
	Set(TiffWangMarkSet::AttributesSet);
}

#define __setlocal_ifset(fname, pname)\
	if (IsSet(TiffWangMarkSet::Global ## fname ## Set)) { \
		m_Local.pname = m_Global.pname; \
		Set(TiffWangMarkSet::Local ## fname ## Set); \
	}

#define __either_set(fname)\
	(IsSet(TiffWangMarkSet::Global ## fname ## Set) || IsSet(TiffWangMarkSet::Local ## fname ## Set))

#define __assert_either_set(fname)\
	if (!__either_set(fname)) throw std::runtime_error("mark property '" # fname "' not set")

#define __return_either(fname, pname)\
	__assert_either_set(fname); \
	return (IsSet(TiffWangMarkSet::Local ## fname ## Set) ? m_Local.pname : m_Global.pname);

#define __assert_local_set(fname)\
	if (!IsSet(TiffWangMarkSet::Local ## fname ## Set)) throw std::runtime_error("mark property '" # fname "' not set");

#define __return_local(fname, pname)\
	__assert_local_set(fname); \
	return m_Local.pname;

#define __return_setter_either(fname, pname, glob)\
	if (glob) { \
		Set(TiffWangMarkSet::Global ## fname ## Set);\
		return m_Global.pname;\
	} else { \
		Set(TiffWangMarkSet::Local ## fname ## Set);\
		return m_Local.pname;\
	}

/// <summary>
/// Reset the local property flags and assign the global properties that are set to the local properties.
/// </summary>
void TiffWangMark::AssignGlobalToLocal() {
	static std::vector<TiffWangMarkSet> locals = { 
		TiffWangMarkSet::LocalAsciiTextSet,
		TiffWangMarkSet::LocalDibInfoSet,
		TiffWangMarkSet::LocalFilenameSet,
		TiffWangMarkSet::LocalGroupSet,
		TiffWangMarkSet::LocalHyperlinkSet,
		TiffWangMarkSet::LocalIndexSet,
		TiffWangMarkSet::LocalPointsSet,
		TiffWangMarkSet::LocalRotationSet,
		TiffWangMarkSet::LocalUnicodeTextSet
	};

	// unset locals
	for (const auto& flag : locals) 
		m_SetProperties &= ~(uint64_t)flag;

	__setlocal_ifset(Group, Group);
	__setlocal_ifset(Index, Index);
	__setlocal_ifset(Filename, FileName);
	__setlocal_ifset(DibInfo, DibInfo);
	__setlocal_ifset(AsciiText, AsciiText);
	__setlocal_ifset(UnicodeText, UnicodeText);
	__setlocal_ifset(Rotation, Rotation);
	__setlocal_ifset(Hyperlink, Hyperlink);

	if (IsSet(TiffWangMarkSet::GlobalPointsSet)) {
		m_Local.Points	  = m_Global.Points;
		m_Local.PointList = m_Global.PointList;
		Set(TiffWangMarkSet::LocalPointsSet);
	}
}

/// <summary>
/// Determine if a certain property is set.
/// </summary>
/// <param name="p">The property to check for.</param>
/// <returns>true when the property p is set.</returns>
bool TiffWangMark::IsSet(TiffWangMarkSet p) const {
	return (m_SetProperties & (uint64_t)p) != 0;
}

/// <summary>
/// Determine if any of the properties in flags is set, similar to <see cref="IsSet"/> but for multiple properties.
/// </summary>
/// <param name="flags">An initializer list of properties to check for.</param>
/// <returns>true when any of the properties is set.</returns>
bool TiffWangMark::AnySet(std::initializer_list<TiffWangMarkSet> flags) const {
	for (const auto& p : flags)
		if (IsSet(p)) return true;
	return false;
}

/// <summary>
/// Mark property p as set.
/// </summary>
/// <param name="p">The property to mark as set.</param>
void TiffWangMark::Set(TiffWangMarkSet p) {
	m_SetProperties |= (uint64_t)p;
}

/// <summary>
/// Get either the local or global group, depending on the set flags.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
const std::string& TiffWangMark::Group() const {
	__return_either(Group, Group);
}

/// <summary>
/// Get either the local or global index, depending on the set flags.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
const std::string& TiffWangMark::Index() const {
	__return_either(Index, Index);
}

/// <summary>
/// Get either the local or global filename, depending on the set flags.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
const std::string& TiffWangMark::FileName() const {
	__return_either(Filename, FileName);
}

/// <summary>
/// Get either the local or global DIB info, depending on the set flags.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
const std::vector<uint8_t>& TiffWangMark::DibInfo() const {
	__return_either(DibInfo, DibInfo);
}

/// <summary>
/// Get either the local or global ascii text, depending on the set flags.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
const std::string& TiffWangMark::AsciiText() const {
	__return_either(AsciiText, AsciiText);
}

/// <summary>
/// Get either the local or global unicode text, depending on the set flags.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
const std::wstring& TiffWangMark::UnicodeText() const {
	__return_either(UnicodeText, UnicodeText);
}

/// <summary>
/// Get either the local or global points struct, depending on the set flags.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
const AN_POINTS& TiffWangMark::Points() const {
	__return_either(Points, Points);
}

/// <summary>
/// Get either the local or global point list, depending on the set flags.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
const std::vector<POINT>& TiffWangMark::PointList() const {
	__return_either(Points, PointList);
}

/// <summary>
/// Get either the local or global rotation struct, depending on the set flags.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
const AN_NEW_ROTATE_STRUCT& TiffWangMark::Rotation() const {
	__return_either(Rotation, Rotation);
}

/// <summary>
/// Get either the local or global text struct, depending on the global state.
/// </summary>
/// <returns>A const reference to the property value.</returns>
const OIAN_TEXTPRIVDATA& TiffWangMark::Text(bool global) const {
	if (global)
		return m_Global.Text;
	return m_Local.Text;
}

/// <summary>
/// Get either the local or global hyperlink struct, depending on the set flags.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the property is not set at all.</exception>
const HYPERLINK_NB& TiffWangMark::HyperLink() const {
	__return_either(Hyperlink, Hyperlink);
}

/// <summary>
/// Get the local group.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>/
const std::string& TiffWangMark::LocalGroup() const {
	__return_local(Group, Group);
}

/// <summary>
/// Get the local index.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
const std::string& TiffWangMark::LocalIndex() const {
	__return_local(Index, Index);
}

/// <summary>
/// Get the local filename.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
const std::string& TiffWangMark::LocalFileName() const {
	__return_local(Filename, FileName);
}

/// <summary>
/// Get the local DIB info.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
const std::vector<uint8_t>& TiffWangMark::LocalDibInfo() const {
	__return_local(DibInfo, DibInfo);
}

/// <summary>
/// Get the local ascii text.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
const std::string& TiffWangMark::LocalAsciiText() const {
	__return_local(AsciiText, AsciiText);
}

/// <summary>
/// Get the local unicode text.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
const std::wstring& TiffWangMark::LocalUnicodeText() const {
	__return_local(UnicodeText, UnicodeText);
}

/// <summary>
/// Get the local points struct.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
const AN_POINTS& TiffWangMark::LocalPoints() const {
	__return_local(Points, Points);
}

/// <summary>
/// Get the local points list.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
const std::vector<POINT>& TiffWangMark::LocalPointList() const {
	__return_local(Points, PointList);
}

/// <summary>
/// Get the local rotation struct.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
const AN_NEW_ROTATE_STRUCT& TiffWangMark::LocalRotation() const {
	__return_local(Rotation, Rotation);
}

/// <summary>
/// Get the local text struct.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
const OIAN_TEXTPRIVDATA& TiffWangMark::LocalText() const {
	if (IsSet(TiffWangMarkSet::LocalAsciiTextSet) || IsSet(TiffWangMarkSet::LocalUnicodeTextSet))
		return m_Local.Text;
	throw std::runtime_error("mark property 'Text' not set");
}

/// <summary>
/// Get the local hyperlink.
/// </summary>
/// <returns>A const reference to the property value.</returns>
/// <exception cref="std::runtime_error">thrown when the local property is not set.</exception>
const HYPERLINK_NB& TiffWangMark::LocalHyperLink() const {
	__return_local(Hyperlink, Hyperlink);
}

/// <summary>
/// Get the non-const reference to either the local or global group.
/// </summary>
/// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
/// <returns>A non-const reference to the property value.</returns>
std::string& TiffWangMark::GroupSetter(bool global) {
	__return_setter_either(Group, Group, global);
}

/// <summary>
/// Get the non-const reference to either the local or global index.
/// </summary>
/// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
/// <returns>A non-const reference to the property value.</returns>
std::string& TiffWangMark::IndexSetter(bool global) {
	__return_setter_either(Index, Index, global);
}

/// <summary>
/// Get the non-const reference to either the local or global filename.
/// </summary>
/// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
/// <returns>A non-const reference to the property value.</returns>
std::string& TiffWangMark::FileNameSetter(bool global) {
	__return_setter_either(Filename, FileName, global);
}

/// <summary>
/// Get the non-const reference to either the local or global DIB info.
/// </summary>
/// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
/// <returns>A non-const reference to the property value.</returns>
std::vector<uint8_t>& TiffWangMark::DibInfoSetter(bool global) {
	__return_setter_either(DibInfo, DibInfo, global);
}

/// <summary>
/// Get the non-const reference to either the local or global ascii text.
/// </summary>
/// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
/// <returns>A non-const reference to the property value.</returns>
std::string& TiffWangMark::AsciiTextSetter(bool global) {
	__return_setter_either(AsciiText, AsciiText, global);
}

/// <summary>
/// Get the non-const reference to either the local or global unicode text.
/// </summary>
/// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
/// <returns>A non-const reference to the property value.</returns>
std::wstring& TiffWangMark::UnicodeTextSetter(bool global) {
	__return_setter_either(UnicodeText, UnicodeText, global);
}

/// <summary>
/// Get the non-const reference to either the local or global points struct.
/// </summary>
/// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
/// <returns>A non-const reference to the property value.</returns>
AN_POINTS& TiffWangMark::PointsSetter(bool global) {
	__return_setter_either(Points, Points, global);
}

/// <summary>
/// Get the non-const reference to either the local or global point list.
/// </summary>
/// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
/// <returns>A non-const reference to the property value.</returns>
std::vector<POINT>& TiffWangMark::PointListSetter(bool global) {
	__return_setter_either(Points, PointList, global);
}

/// <summary>
/// Get the non-const reference to either the local or global rotate struct.
/// </summary>
/// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
/// <returns>A non-const reference to the property value.</returns>
AN_NEW_ROTATE_STRUCT& TiffWangMark::RotationSetter(bool global) {
	__return_setter_either(Rotation, Rotation, global);
}

/// <summary>
/// Get the non-const reference to either the local or global text struct.
/// </summary>
/// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
/// <returns>A non-const reference to the property value.</returns>
OIAN_TEXTPRIVDATA& TiffWangMark::TextSetter(bool global) {
	if (global)
		return m_Global.Text;
	return m_Local.Text;
}

/// <summary>
/// Get the non-const reference to either the local or global hyperlink struct.
/// </summary>
/// <param name="global">whether to fetch a non-const reference to the local or global property.</param>
/// <returns>A non-const reference to the property value.</returns>
HYPERLINK_NB& TiffWangMark::HyperLinkSetter(bool global) {
	__return_setter_either(Hyperlink, Hyperlink, global);
}

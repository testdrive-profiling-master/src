#include "stdafx.h"
#include "Localization.h"
#include "RegistryCtrl.h"

/*
LOCALE_DESC		__LocaleDesc[] =
{
	{ LANG_ENGLISH,				SUBLANG_ENGLISH_US,				_T("English"),						_T("english.xml") },
	{ LANG_CHINESE_TRADITIONAL,	SUBLANG_CHINESE_TRADITIONAL,	_T("中文繁體"),						_T("chinese.xml") },
	{ LANG_CHINESE_SIMPLIFIED,	SUBLANG_CHINESE_SIMPLIFIED,		_T("中文简体"),						_T("chineseSimplified.xml") },
	{ LANG_KOREAN,				SUBLANG_KOREAN,					_T("한국어"),							_T("korean.xml") },
	{ LANG_JAPANESE,			SUBLANG_JAPANESE_JAPAN,			_T("日本語"),							_T("japanese.xml") },
	{ LANG_FRENCH,				SUBLANG_FRENCH,					_T("Français"),						_T("french.xml") },
	{ LANG_GERMAN,				SUBLANG_GERMAN,					_T("Deutsch"),						_T("german.xml") },
	{ LANG_SPANISH,				SUBLANG_SPANISH,				_T("Español"),						_T("spanish.xml") },
	{ LANG_ITALIAN,				SUBLANG_ITALIAN,				_T("Italiano"),						_T("italian.xml") },
	{ LANG_PORTUGUESE,			SUBLANG_PORTUGUESE,				_T("Português"),						_T("portuguese.xml") },
	{ LANG_PORTUGUESE,			SUBLANG_PORTUGUESE_BRAZILIAN,	_T("Português brasileiro"),			_T("brazilian_portuguese.xml") },
	{ LANG_DUTCH,				SUBLANG_DUTCH,					_T("Nederlands"),						_T("dutch.xml") },
	{ LANG_RUSSIAN,				SUBLANG_RUSSIAN_RUSSIA,			_T("Русский"),					_T("russian.xml") },
	{ LANG_POLISH,				SUBLANG_POLISH_POLAND,			_T("Polski"),							_T("polish.xml") },
	{ LANG_CATALAN,				SUBLANG_CATALAN_CATALAN,		_T("Català"),							_T("catalan.xml") },
	{ LANG_CZECH,				SUBLANG_CZECH_CZECH_REPUBLIC,	_T("Česky"),							_T("czech.xml") },
	{ LANG_HUNGARIAN,			SUBLANG_HUNGARIAN_HUNGARY,		_T("Magyar"),							_T("hungarian.xml") },
	{ LANG_ROMANIAN,			SUBLANG_ROMANIAN_ROMANIA,		_T("Română"),							_T("romanian.xml") },
	{ LANG_TURKISH,				SUBLANG_TURKISH_TURKEY,			_T("Türkçe"),							_T("turkish.xml") },
	{ LANG_PERSIAN,				SUBLANG_PERSIAN_IRAN,			_T("فارسی"),							_T("farsi.xml") },
	{ LANG_UKRAINIAN,			SUBLANG_UKRAINIAN_UKRAINE,		_T("Українська"),			_T("ukrainian.xml") },
	{ LANG_HEBREW,				SUBLANG_HEBREW_ISRAEL,			_T("עברית"),							_T("hebrew.xml") },
	{ LANG_NORWEGIAN,			SUBLANG_NORWEGIAN_NYNORSK,		_T("Nynorsk"),						_T("nynorsk.xml") },
	{ LANG_NORWEGIAN,			SUBLANG_NORWEGIAN_BOKMAL,		_T("Norsk"),							_T("norwegian.xml") },
	{ LANG_OCCITAN,				SUBLANG_OCCITAN_FRANCE,			_T("Occitan"),						_T("occitan.xml") },
	{ LANG_THAI,				SUBLANG_THAI_THAILAND,			_T("ไทย"),								_T("thai.xml") },
	{ LANG_NEUTRAL,				0,								_T("Furlan"),							_T("friulian.xml") },
	{ LANG_ARABIC,				SUBLANG_ARABIC_SAUDI_ARABIA,	_T("الْعَرَبيّة"),						_T("arabic.xml") },
	{ LANG_FINNISH,				SUBLANG_FINNISH_FINLAND,		_T("Suomi"),							_T("finnish.xml") },
	{ LANG_LITHUANIAN,			_T("Lietuvių"),						_T("lithuanian.xml") },
	{ LANG_GREEK,				_T("Ελληνικά"),				_T("greek.xml") },
	{ LANG_SWEDISH,				_T("Svenska"),						_T("swedish.xml") },
	{ LANG_GALICIAN,			_T("Galego"),							_T("galician.xml") },
	{ LANG_SLOVENIAN,			_T("Slovenščina"),					_T("slovenian.xml") },
	{ LANG_SLOVAK,				_T("Slovenčina"),						_T("slovak.xml") },
	{ LANG_DANISH,				 _T("Dansk"),							_T("danish.xml") },
	{ _T("Estremeñu"),						_T("extremaduran.xml") },
	{ _T("Žemaitiu ruoda"),					_T("samogitian.xml") },
	{ LANG_BULGARIAN,			_T("Български"),				_T("bulgarian.xml") },
	{ LANG_INDONESIAN,			_T("Bahasa Indonesia"),				_T("indonesian.xml") },
	{ LANG_ALBANIAN,			_T("Gjuha shqipe"),					_T("albanian.xml") },
	{ LANG_CROATIAN,			_T("Hrvatski jezik"),					_T("croatian.xml") },
	{ _T("ქართული ენა"),					_T("georgian.xml") },
	{ LANG_BASQUE,				_T("Euskara"),						_T("basque.xml") },
	{ LANG_SPANISH,				SUBLANG_SPANISH_ARGENTINA,	_T("Español argentina"),				_T("spanish_ar.xml") },
	{ LANG_BELARUSIAN,			_T("Беларуская мова"),	_T("belarusian.xml") },
	{ LANG_SERBIAN,				_T("Srpski"),							_T("serbian.xml") },
	{ LANG_SERBIAN,				SUBLANG_SERBIAN_CYRILLIC,			_T("Cрпски"),					_T("serbianCyrillic.xml") },
	{ LANG_MALAY,	_T("Bahasa Melayu"),					_T("malay.xml") },
	{ _T("Lëtzebuergesch"),					_T("luxembourgish.xml") },
	{ _T("Tagalog"),						_T("tagalog.xml") },
	{ _T("Afrikaans"),						_T("afrikaans.xml") },
	{ _T("Қазақша"),					_T("kazakh.xml") },
	{ _T("O‘zbekcha"),						_T("uzbek.xml") },
	{ _T("Ўзбекча"),					_T("uzbekCyrillic.xml") },
	{ _T("Кыргыз тили"),			_T("kyrgyz.xml") },
	{ _T("Македонски јазик"),	_T("macedonian.xml") },
	{ _T("latviešu valoda"),				_T("latvian.xml") },
	{ _T("தமிழ்"),							_T("tamil.xml") },
	{ _T("Azərbaycan dili"),				_T("azerbaijani.xml") },
	{ _T("Bosanski"),						_T("bosnian.xml") },
	{ _T("Esperanto"),						_T("esperanto.xml") },
	{ _T("Zeneize"),						_T("ligurian.xml") },
	{ _T("हिन्दी"),							_T("hindi.xml") },
	{ _T("Sardu"),							_T("sardinian.xml") },
	{ _T("ئۇيغۇرچە"),						_T("uyghur.xml") },
	{ _T("తెలుగు"),							_T("telugu.xml") },
	{ _T("aragonés"),						_T("aragonese.xml") },
	{ _T("বাংলা"),							_T("bengali.xml") },
	{ _T("සිංහල"),							_T("sinhala.xml") },
	{ _T("Taqbaylit"),						_T("kabyle.xml") },
	{ _T("मराठी"),							_T("marathi.xml") },
	{ _T("tiếng Việt"),						_T("vietnamese.xml") },
	{ _T("Aranés"),							_T("aranese.xml") },
	{ _T("ગુજરાતી"),							_T("gujarati.xml") },
	{ _T("Монгол хэл"),			_T("mongolian.xml") },
	{ _T("اُردُو‎"),							_T("urdu.xml") },
	{ _T("ಕನ್ನಡ‎"),							_T("kannada.xml") },
	{ _T("Cymraeg"),						_T("welsh.xml") },
	{ _T("eesti keel"),						_T("estonian.xml") },
	{ _T("Тоҷик"),						_T("tajikCyrillic.xml") },
	{ _T("татарча"),					_T("tatar.xml") },
	{ _T("ਪੰਜਾਬੀ"),							_T("punjabi.xml") },
	{ _T("Corsu"),							_T("corsican.xml") },
	{ NULL, NULL }
};
*/

static const TCHAR*		__KEY_LOCALE_ID			= _T("LOCALE_ID");
static const TCHAR*		__KEY_LOCALE_SUB_ID		= _T("LOCALE_SUB_ID");

CLocalization	g_Localization;

CLocalization::CLocalization(void)
{
	m_dwTotalLocaleCount	= 0;
}

CLocalization::~CLocalization()
{
	Release();
}

BOOL CLocalization::Initialize(LPCTSTR sPath) {
	Release();
	if (sPath) {
		// read locale list
		pugi::xml_document doc;
		m_sRootPath.Format(_T("%slocalization\\"), sPath);
		{
			CString	sListFilePath(m_sRootPath);
			sListFilePath += _T("__list__.xml");
			if (!doc.load_file(sListFilePath)) return FALSE;
		}
		pugi::xml_node node = doc.child(_T("Localization")).child(_T("language"));
		while (node) {
			AddLocaleList(node.attribute(_T("name")).as_string(), node.attribute(_T("lang_id")).as_uint(), node.attribute(_T("sublang_id")).as_uint(), node.attribute(_T("file")).as_string());
			node = node.next_sibling();
		}
	}else return FALSE;

	// set locale ID from registery
	if (!SetLocale(g_RegistryCtrl.GetProfileInt(__KEY_LOCALE_ID, 0), g_RegistryCtrl.GetProfileInt(__KEY_LOCALE_SUB_ID, 0))) {
		// set locale ID from OS selection
		if (!SetLocale(PRIMARYLANGID(GetSystemDefaultLangID()), SUBLANGID(GetSystemDefaultLangID()))) 
		{
			// set default locale(english)
			if (!SetLocale(LANG_ENGLISH)) {
				return FALSE;
			}
		}
	}

RETRY_LOAD_LOCALE:
	if (!LoadLocale(CurrentLocale()->sFileName)) {
		if (CurrentLocale()->dwLangID != LANG_ENGLISH) {
			{
				CString msg;
				msg.Format(_T("Localization file for '%s'(%s) is not found.\n"), CurrentLocale()->sName, CurrentLocale()->sFileName);
				MessageBox(NULL, msg, _T("Warning"), MB_OK|MB_ICONEXCLAMATION);
			}
			SetLocale(LANG_ENGLISH);
			goto RETRY_LOAD_LOCALE;
		}
		return FALSE;
	}

	return TRUE;
}

void CLocalization::Release(void) {
	m_dwTotalLocaleCount	= 0;
	m_LocaleList.clear();
	m_StringCache.clear();
}

BOOL CLocalization::LoadLocale(LPCTSTR sFileName) {
	pugi::xml_document doc;
	CString	sListFilePath(m_sRootPath);
	sListFilePath += sFileName;
	if (!doc.load_file(sListFilePath)) return FALSE;
	pugi::xml_node node = doc.child(_T("string")).child(_T("Item"));
	while (node) {
		m_StringCache[node.attribute(_T("id")).as_uint()] = node.attribute(_T("expr")).as_string();
		node = node.next_sibling();
	}

	return TRUE;
}

void CLocalization::AddLocaleList(LPCTSTR sName, DWORD dwLangID, DWORD dwSubLangID, LPCTSTR sFIleName) {
	LOCALE_DESC desc = {sName, dwLangID, dwSubLangID, sFIleName};
	m_LocaleList[m_dwTotalLocaleCount] = desc;
	m_dwTotalLocaleCount++;
}

LOCALE_DESC* CLocalization::LocaleFromList(DWORD dwLocaleID) {
	if (dwLocaleID < m_dwTotalLocaleCount) {
		return &(m_LocaleList[dwLocaleID]);
	}
	return NULL;
}

BOOL CLocalization::SetLocale(DWORD dwLocaleID, DWORD dwLocaleSubID, BOOL bInvalidate) {
	if ((m_pCurrentLocale = FindLocale(dwLocaleID, dwLocaleSubID)) != NULL) {
		_tsetlocale(LC_ALL, _T(""));
		if (bInvalidate) {
			g_RegistryCtrl.SetProfileInt(__KEY_LOCALE_ID, dwLocaleID);
			g_RegistryCtrl.SetProfileInt(__KEY_LOCALE_SUB_ID, dwLocaleSubID);
		}
	}
	return m_pCurrentLocale != NULL;
}

LOCALE_DESC* CLocalization::FindLocale(DWORD dwLocaleID, DWORD dwLocaleSubID) {
	LOCALE_DESC*	pLocale = NULL;
	for (DWORD id = 0; id < m_dwTotalLocaleCount; id++) {
		if (m_LocaleList[id].dwLangID == dwLocaleID && (!dwLocaleSubID || (m_LocaleList[id].dwSubLangID == dwLocaleSubID))) {
			return &m_LocaleList[id];
		}
	}
	return NULL;
}

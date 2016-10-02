/*
	�����E�����񑀍샆�[�e�B���e�B
	Copyright (C)2000-2002 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_HMC_STRING__
#define __INC_HMC_STRING__

#include "util.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
	�O���[�o��
*****************************************************************/
// mbs<->mbc�ϊ�
UINT mbs2mbc(LPCSTR szChar);
void mbc2mbs(UINT uChar, LPSTR szChar);

// ������ɑS�p�������܂܂�Ă��邩��������(��������Ԃ�)
DWORD HaveMBChar(LPCSTR lpText);

// ���p<->�S�p������ϊ�
DWORD han2zen(LPCSTR lpText, LPSTR lpBuf);	// lpBuf�̍����o�b�t�@��2+1�o�C�g�ȏ�K�v
DWORD zen2han(LPCSTR lpText, LPSTR lpBuf);	// lpBuf�̍����o�b�t�@��2+1�o�C�g�ȏ�K�v
DWORD han2zen(LPCSTR lpText, LPSTR lpBuf, DWORD cchBuf);
DWORD zen2han(LPCSTR lpText, LPSTR lpBuf, DWORD cchBuf);

// �J�^�J�i<->�Ђ炪�ȕ�����ϊ�
DWORD kata2hira(LPCSTR lpText, LPSTR lpBuf);	// lpBuf�̍����o�b�t�@��4+1�o�C�g�ȏ�K�v(��->���J���l�����邽��)
DWORD hira2kata(LPCSTR lpText, LPSTR lpBuf);	// lpBuf�̍����o�b�t�@��2+2�o�C�g�ȏ�K�v
DWORD kata2hira(LPCSTR lpText, LPSTR lpBuf, DWORD cchBuf);
DWORD hira2kata(LPCSTR lpText, LPSTR lpBuf, DWORD cchBuf);

// ���[�}���J�^�J�i(�S�p)�ϊ�
DWORD roma2kana(LPCSTR lpText, LPSTR lpBuf, LPCSTR lpHint, DWORD dwFlags);	// lpBuf�̍����o�b�t�@��6+1�o�C�g�ȏ�K�v
DWORD kana2roma(LPCSTR lpText, LPSTR lpBuf, LPCSTR lpHint, DWORD dwFlags);	// lpBuf�̍����o�b�t�@��4+1�o�C�g�ȏ�K�v
DWORD roma2kana(LPCSTR lpText, LPSTR lpBuf, DWORD cchBuf, LPCSTR lpHint, DWORD dwFlags);	// ���w�{���A���{�A�P�ߎ��ǂ���ʂ�(�͂�)
DWORD kana2roma(LPCSTR lpText, LPSTR lpBuf, DWORD cchBuf, LPCSTR lpHint, DWORD dwFlags);	// ����{�I�ɌP�ߎ��L���ɏ���(�̂͂�)
enum {
	ROMA2KANA_KANA2HINT_MATCH			= 0x80000000,
	ROMA2KANA_KANA2HINT_PARTIALMATCH	= 0x40000000,
	ROMA2KANA_HINT2KANA_MATCH			= 0x20000000,
	ROMA2KANA_HINT2KANA_PARTIALMATCH	= 0x10000000,
	ROMA2KANA_KANA2HINT = ROMA2KANA_KANA2HINT_MATCH|ROMA2KANA_KANA2HINT_PARTIALMATCH,
	ROMA2KANA_HINT2KANA = ROMA2KANA_HINT2KANA_MATCH|ROMA2KANA_HINT2KANA_PARTIALMATCH,
	ROMA2KANA_HINT = ROMA2KANA_KANA2HINT|ROMA2KANA_HINT2KANA,
	ROMA2KANA_REMOVE_ENDOFNN			= 0x08000000,
};
enum {
	KANA2ROMA_ROMA2HINT_MATCH			= 0x80000000,
	KANA2ROMA_ROMA2HINT_PARTIALMATCH	= 0x40000000,
	KANA2ROMA_HINT2ROMA_MATCH			= 0x20000000,
	KANA2ROMA_HINT2ROMA_PARTIALMATCH	= 0x10000000,
	KANA2ROMA_ROMA2HINT = KANA2ROMA_ROMA2HINT_MATCH|KANA2ROMA_ROMA2HINT_PARTIALMATCH,
	KANA2ROMA_HINT2ROMA = KANA2ROMA_HINT2ROMA_MATCH|KANA2ROMA_HINT2ROMA_PARTIALMATCH,
	KANA2ROMA_HINT = KANA2ROMA_ROMA2HINT|KANA2ROMA_HINT2ROMA,
	KANA2ROMA_REMOVE_ENDOFNN			= 0x08000000,
};

// ����������擾(csv�̂悤�ȕ�����؂�e�L�X�g�ŁA����C���f�b�N�X�̕�����������擾�A�ݒ肷��)
BOOL GetCSVIndexedString(LPCTSTR lpText, TCHAR cDelimiter, DWORD nIndex, LPTSTR lpBuf, DWORD cchBuf, LPDWORD lpdwCopied=NULL);
BOOL GetCSVIndexedString(LPCTSTR lpText, LPCTSTR lpDelimiter, DWORD nIndex, LPTSTR lpBuf, DWORD cchBuf, LPDWORD lpdwCopied=NULL);
BOOL SetCSVIndexedString(LPTSTR lpText, DWORD cchText, TCHAR cDelimiter, DWORD nIndex, LPCTSTR lpString);
BOOL SetCSVIndexedString(LPTSTR lpText, DWORD cchText, LPCTSTR lpDelimiter, DWORD nIndex, LPCTSTR lpString);
DWORD CountCSVIndexedString(LPCTSTR lpText, TCHAR cDelimiter);
DWORD CountCSVIndexedString(LPCTSTR lpText, LPCTSTR lpDelimiter);
DWORD FindCSVIndexedString(LPTSTR lpText, TCHAR cDelimiter, LPCTSTR lpString);
DWORD FindCSVIndexedString(LPTSTR lpText, LPCTSTR lpDelimiter, LPCTSTR lpString);
BOOL RemoveCSVIndexedString(LPTSTR lpText, TCHAR cDelimiter, DWORD nIndex);
BOOL RemoveCSVIndexedString(LPTSTR lpText, LPCTSTR lpDelimiter, DWORD nIndex);
BOOL InsertCSVIndexedString(LPTSTR lpText, DWORD cchText, TCHAR cDelimiter, DWORD nIndex, LPCTSTR lpString);
BOOL InsertCSVIndexedString(LPTSTR lpText, DWORD cchText, LPCTSTR lpDelimiter, DWORD nIndex, LPCTSTR lpString);

/*****************************************************************
	������^�e���v���[�g�̃x�[�X�N���X
*****************************************************************/
template <typename tchar_t, size_t array_len> class TStringBaseObject
{
protected:
	TStringBaseObject() { m_rep[0]=0; }
	tchar_t *object() const { return const_cast<tchar_t *>(&m_rep[0]); }
	tchar_t m_rep[max(2,array_len)];	// �K���Q�������ȏ�K�v�I
};
template <typename tchar_t, size_t array_len> class CStringBaseObject
{
protected:
	CStringBaseObject() { m_rep=NULL; }
	tchar_t *object() const { return const_cast<tchar_t *>(m_rep); }
	tchar_t *m_rep;
};
template <typename TYPE, typename tchar_t, size_t array_len, bool fcheck_bounds, size_t **grow_list> class TStringBase
{
public:
	DECLARE_SYNONYM(TYPE,TYPE);
	typedef char achar_t;
	typedef unsigned char mchar_t;
protected:
	TStringBase() { init(); let(_safestring((tchar_t *)NULL)); }
	TStringBase(const TYPE &t) { init(); if(&t!=this) let(t.object()); }
	TStringBase(size_t n, const tchar_t *form, ...) { init(); va_list args; va_start(args,form); format(n,form,args); }
	TStringBase(const tchar_t *p, size_t c) { init(); let(p,c); }
	TStringBase(const achar_t *p) { init(); let(p); }
	TStringBase(const wchar_t *p) { init(); let(p); }
	//virtual ~TStringBase() { destroy(); }	// ���Œ�T�C�Y������̏ꍇvtbl������ƍ��邵�f�X�g���N�^�����������ǂ��̂�
private:
	void init() {
		if(grow_list) {
			m_prep = NULL;
		} else {
			m_rep[0] = 0;
		}
	}
public:
	void destroy() {
		if(grow_list) {
			free_buf();
		} else {
			*_safestring((tchar_t *)NULL);
		}
	}
public:
	enum {
		init_buf_size	= 512,
		bound_buf_size	= 512,
	};
	// ���������蓖��(�����\�ȏꍇ�̂�)
	tchar_t *alloc_buf(size_t n) {
		if(grow_list) {
			if(m_prep) {
				m_prep = (tchar_t *)::realloc(m_prep,n*sizeof(tchar_t));
			} else {
				m_prep = (tchar_t *)::malloc(n*sizeof(tchar_t));
				*m_prep = *_safestring((tchar_t *)NULL);
			}
		} else {
			ASSUME(0);
		}
		return object();
	}
	void free_buf() {
		if(grow_list) {
			if(m_prep) {
				::free(m_prep);
				m_prep = NULL;
			}
		} else {
			ASSUME(0);
		}
	}
public:
	// �������
	void let(const achar_t *p) {
		if(grow_list) {
			size_t len = _length(p)+1;
			if(chars()<len) {
				alloc_buf(len);
			}
		}
		_copy(object(),_safestring(p));
	}
	void let(const wchar_t *p) {
		if(grow_list) {
			size_t len = _length(p)+1;
			if(chars()<len) {
				alloc_buf(len);
			}
		}
		_copy(object(),_safestring(p));
	}
	void let(const tchar_t *p, size_t c) {
		if(grow_list) {
			size_t len = _length(p)+1;
			len = min(c,min(len,chars()-1));
			if(chars()<len+1) {
				alloc_buf(len+1);
			}
		}
		_copy(object(),_safestring(p),c);
	}
	void concat(const tchar_t *p) {
		if(grow_list) {
			size_t len = _length(p)+length()+1;
			if(chars()<len) {
				alloc_buf(len);
			}
		}
		_concat(object(),_safestring(p));
	}
	void setEmpty() { let(_safestring((tchar_t *)NULL)); }
public:
	// ����������̎擾
	TYPE &shorten(size_t len) { object()[len]=*_safestring((tchar_t *)NULL); return *((LPTYPE)this); }
	TYPE &substring(size_t n) const { return *((LPTYPE)&(object()[n])); }	// ��fbound||fgrown�̏ꍇ�͖���
	TYPE substring(size_t n, size_t len) const {
		size_t m = length();
		TYPE s(&(object()[min(n,m)]),len);
		return s;
	}
public:
	// ��I�u�W�F�N�g�ւ̃R�s�[
	tchar_t *copy_to(tchar_t *p) const { return _copy(p,object()); }
	tchar_t *copy_to(tchar_t *p, size_t c) const { return _copy(p,object(),c); }
public:
	// ��r����
	bool isEqual(const tchar_t *p) const { return compare(_safestring(p))==0; }
	bool isEqual(const tchar_t *p, size_t n) const { return compare(_safestring(p),n)==0; }
	bool isEqualSubstring(const tchar_t *p) const { return compareSubstring(_safestring(p))==0; }
	bool isEqualSubstring(size_t n, const tchar_t *p) const { return substring(n).compareSubstring(_safestring(p))==0; }
	int compare(const tchar_t *p) const { return _compare(object(),_safestring(p)); }
	int compare(const tchar_t *p, size_t n) const { return _compare(object(),_safestring(p),n); }
	int compareSubstring(const tchar_t *p) const { return compare(p,max(1,_length(_safestring(p)))); }
	int compareSubstring(size_t n, const tchar_t *p) const { return substring(n).compare(_safestring(p),max(1,_length(_safestring(p)))); }
	bool isEqualIgnoreCase(const tchar_t *p) const { return compareIgnoreCase(_safestring(p))==0; }
	bool isEqualIgnoreCase(const tchar_t *p, size_t n) const { return compareIgnoreCase(_safestring(p),n)==0; }
	int compareIgnoreCase(const tchar_t *p) const { return _icompare(object(),_safestring(p)); }
	int compareIgnoreCase(const tchar_t *p, size_t n) const { return _icompare(object(),_safestring(p),n); }
	int compareSubstringIgnoreCase(const tchar_t *p) const { return compareIgnoreCase(_safestring(p),max(1,_length(_safestring(p)))); }
	int compareSubstringIgnoreCase(size_t n, const tchar_t *p) const { return substring(n).compareIgnoreCase(_safestring(p),max(1,_length(_safestring(p)))); }
	bool isEqualSubstringIgnoreCase(const tchar_t *p) const { return compareSubstringIgnoreCase(_safestring(p))==0; }
	bool isEqualSubstringIgnoreCase(size_t n, const tchar_t *p) const { return compareSubstringIgnoreCase(n,_safestring(p))==0; }
public:
	// �����񑀍�(�ϊ��A�폜�A�}���A�t�H�[�}�b�g)
	void upcase() { _upcase(object()); }
	void downcase() { _downcase(pbject()); }
	void reverse() { _reverse(object()); }
public:
	// �u��
	int replace(tchar_t c1, tchar_t c2) {
		tchar_t *p = object();
		int i = 0;
		for(; p&&*p;) {
			if(p=_find(p,c1)) {
				*p=c2; i++;
			}
		}
		return i;
	}
	int replace(tchar_t c1, tchar_t c2, size_t n) {
		tchar_t *p = object();
		int i = 0;
		for(; p&&*p;) {
			if(p=_find(p,c1,n)) {
				*p=c2; i++;
			}
		}
		return i;
	}
	int replace(const tchar_t *s1, const tchar_t *s2) {
		tchar_t *p = object();
		size_t len1 = _length(s1);
		size_t len2 = _length(s2);
		int i = 0;
		for(; p&&*p;) {
			if(p=_find(p,s1)) {
				if(len1>len2) {
					removeAt((size_t)(p-object())+len2,len1-len2);
				} else if(len1<len2) {
					insertAt((size_t)(p-object())+len1,len2-len1);
				}
				memcpy(p,s2,len2*sizeof(tchar_t));
				p+=len2;
				i++;
			}
		}
		return i;
	}
	int replace(const tchar_t *s1, const tchar_t *s2, size_t n) {
		tchar_t *p = object();
		size_t len1 = _length(s1);
		size_t len2 = _length(s2);
		int i = 0;
		for(; p&&*p;) {
			if(p=_find(p,s1,n)) {
				if(len1>len2) {
					removeAt((size_t)(p-object())+len2,len1-len2);
				} else if(len1<len2) {
					insertAt((size_t)(p-object())+len1,len2-len1);
				}
				memcpy(p,s2,len2*sizeof(tchar_t));
				p+=len2;
				i++;
			}
		}
		return i;
	}
	int replaceAt(size_t n, const tchar_t *s1, const tchar_t *s2) {
		if(substring(n).isEqualSubstring(s1)) {
			size_t len1 = _length(s1);
			size_t len2 = _length(s2);
			if(len1>len2) {
				removeAt(n,len1-len2);
			} else {
				insertAt(n,len2-len1);
			}
			memcpy(&object()[n],s2,len2*sizeof(tchar_t));
			return 1;
		} else {
			return 0;
		}
	}
	int replaceAt(size_t n, size_t len, const tchar_t *s) {
		size_t len2 = _length(s);
		if(len>len2) {
			removeAt(n,len-len2);
		} else {
			insertAt(n,len2-len);
		}
		memcpy(&object()[n],s,len2*sizeof(tchar_t));
		return 1;
	}
public:
	// �폜
	int remove(tchar_t c) {
		tchar_t *p = object();
		int i = 0;
		for(; p&&*p;) {
			if(p=_find(p,c)) {
				removeAt((size_t)(p-object()),1);
				i++;
			}
		}
		return i;
	}
	int remove(const tchar_t *s) {
		tchar_t *p = object();
		size_t len = _length(s);
		int i = 0;
		for(; p&&*p;) {
			if(p=_find(p,s)) {
				removeAt((size_t)(p-object()),len);
				i++;
			}
		}
		return i;
	}
	int removeAt(size_t n, size_t c) {
		n = min(n,length());
		c = min(c,length()-n);
		size_t l = length()+1-(n+c);
		memmove(&object()[n],&object()[n+c],l*sizeof(tchar_t));
		return c;
	}
public:
	// �}��
	int insert(size_t n, tchar_t c) {
		int i = insertAt(n,1);
		object()[n] = c;
		return i;
	}
	int insert(size_t n, const tchar_t *s) {
		int i = insertAt(n,_length(s));
		memcpy(&object()[n],s,i*sizeof(tchar_t));
		return i;
	}
	int insertAt(size_t n, size_t c) {
		n = min(n,length());
		if(grow_list) {
			size_t len = length()+c+1;
			if(chars()<len) {
				alloc_buf(len);
			}
		}
		size_t l = _length(&object()[n])+1;
		memmove(&object()[n+c],&object()[n],l*sizeof(tchar_t));
		return c;
	}
public:
	// �t�H�[�}�b�g
	int format(const tchar_t *form, ...) {
		va_list args;
		va_start(args,form);
		if(grow_list) {
			if(chars()==0) {
				alloc_buf(init_buf_size);
			}
			int i = -1;
			while(1) {
				i = _nformat(&object()[0],chars()-1,form,args);
				if(i==-1 || i==chars()-1) {
					alloc_buf(chars()+bound_buf_size);
				} else {
					break;
				}
			}
			return i;
		} else {
			return _format(&object()[0],form,args);
		}
	}
	int format(size_t n, const tchar_t *form, ...) {
		va_list args;
		va_start(args,form);
		if(grow_list) {
			if(chars()==0) {
				alloc_buf(init_buf_size);
			}
			int i = -1;
			while(1) {
				i = _nformat(&object()[n],chars()-n-1,form,args);
				if(i==-1 || i==chars()-n-1) {
					alloc_buf(chars()+bound_buf_size);
				} else {
					break;
				}
			}
			return i;
		} else {
			return _format(&object()[n],form,args);
		}
	}
	int format(const tchar_t *form, va_list args) {
		if(grow_list) {
			if(chars()==0) {
				alloc_buf(init_buf_size);
			}
			int i = -1;
			while(1) {
				i = _nformat(&object()[0],chars()-1,form,args);
				if(i==-1 || i==chars()-1) {
					alloc_buf(chars()+bound_buf_size);
				} else {
					break;
				}
			}
			return i;
		} else {
			return _format(&object()[0],form,args);
		}
	}
	int format(size_t n, const tchar_t *form, va_list args) {
		if(grow_list) {
			if(chars()==0) {
				alloc_buf(init_buf_size);
			}
			int i = -1;
			while(1) {
				i = _nformat(&object()[n],chars()-n-1,form,args);
				if(i==-1 || i==chars()-n-1) {
					alloc_buf(chars()+bound_buf_size);
				} else {
					break;
				}
			}
			return i;
		} else {
			return _format(&object()[n],form,args);
		}
	}
public:
	// ����
	tchar_t *find(tchar_t c) const { return _find(object(),c); }
	tchar_t *find(tchar_t c, size_t n) const { return _find(object(),c,n); }
	tchar_t *find(const tchar_t *s) const { return _find(object(),s); }
	tchar_t *find(const tchar_t *s, size_t n) const { return _find(object(),s,n); }
public:
#if 0
	// ������̔���
	int bType(int c=0) const { return _mbsbtype((mchar_t *)object(),c); }
	bool isLead(int c=0) const { return !!_ismbslead((mchar_t *)object(),(mchar_t *)(&(object()[c]))); }
	bool isTrail(int c=0) const { return !!_ismbstrail((mchar_t *)object(),(mchar_t *)(&(object()[c]))); }
	bool isAlnum(int c=0) const { return !!_ismbcalnum(UINT(isLead(c) ? *((TWORD*)&(object()[c])) : object()[c])); }
	bool isAlpha(int c=0) const { return !!_ismbcalpha(UINT(isLead(c) ? *((TWORD*)&(object()[c])) : object()[c])); }
	bool isDigit(int c=0) const { return !!_ismbcdigit(UINT(isLead(c) ? *((TWORD*)&(object()[c])) : object()[c])); }
	bool isGraph(int c=0) const { return !!_ismbcgraph(UINT(isLead(c) ? *((TWORD*)&(object()[c])) : object()[c])); }
	bool isLegal(int c=0) const { return !!_ismbclegal(UINT(isLead(c) ? *((TWORD*)&(object()[c])) : object()[c])); }
	bool isLower(int c=0) const { return !!_ismbclower(UINT(isLead(c) ? *((TWORD*)&(object()[c])) : object()[c])); }
	bool isUpper(int c=0) const { return !!_ismbcupper(UINT(isLead(c) ? *((TWORD*)&(object()[c])) : object()[c])); }
	bool isPrint(int c=0) const { return !!_ismbcprint(UINT(isLead(c) ? *((TWORD*)&(object()[c])) : object()[c])); }
	bool isSpace(int c=0) const { return !!_ismbcspace(UINT(isLead(c) ? *((TWORD*)&(object()[c])) : object()[c])); }
#endif
public:
	// �N���X���̎擾/�ݒ�
	bool isEmpty() const { return *object()==*_safestring((tchar_t *)NULL);; }
	size_t length() const { return _length(object()); }
	// �T�C�Y�A�������̎擾
	size_t size() const {
		if(grow_list) {
			return m_prep?_msize(m_prep):0;
		} else {
			return sizeof(*((LPTYPE)this));
		}
	}
	size_t chars () const { return size()/sizeof(tchar_t); }
	size_t achars() const { return size()/sizeof(achar_t); }
	size_t wchars() const { return size()/sizeof(wchar_t); }
	// �|�C���^�擾
	tchar_t *object () const { return (tchar_t *)(grow_list ? m_prep : &m_rep[0]); }
protected:
	// NULL��NULL�����ɕϊ�(�����I�ɉE�l�ɂ̂ݎg�p���邱��)
	static const achar_t *_safestring(const achar_t *p) { return p ? p :  ""; }
	static const wchar_t *_safestring(const wchar_t *p) { return p ? p : L""; }
	// �P���R�s�[
	//static achar_t *_copy(achar_t *p1, const achar_t *p2) { return strcpy(p1,p2); }
	static achar_t *_copy(achar_t *p1, const achar_t *p2) { return (achar_t *)_mbscpy((mchar_t *)p1,(mchar_t *)p2); }
	static wchar_t *_copy(wchar_t *p1, const wchar_t *p2) { return wcscpy(p1,p2); }
	// ����������R�s�[([n]�`[n+c]���R�s�[�A�K��NULL������t������)
	static achar_t *_copy(achar_t *p1, const achar_t *p2, size_t n, size_t c) {
		size_t len = _length(p2);
		n = min(n,len);
		c = min(n+c,len) - n;
		memcpy(p1,&p2[n],c*sizeof(achar_t));
		p1[len] = *_safestring((achar_t *)NULL);
		return p1;
	}
	static achar_t *_copy(wchar_t *p1, const wchar_t *p2, size_t n, size_t c) {
		size_t len = _length(p2);
		n = min(n,len);
		c = min(n+c,len) - n;
		memcpy(p1,&p2[n],c*sizeof(wchar_t));
		p1[len] = *_safestring((wchar_t *)NULL);
		return p1;
	}
	// ����������R�s�[([0]�`[c]���R�s�[�A�K��NULL������t������)
	static achar_t *_copy(achar_t *p1, const achar_t *p2, size_t c) {
		size_t len = _length(p2);
		len = min(c,len);
		strncpy(p1,p2,len);
		p1[len] = *_safestring((achar_t *)NULL);
		return p1;
	}
	//static achar_t *_copy(achar_t *p1, const achar_t *p2, size_t c) {
	//	size_t len = _length(p2);
	//	len = min(c,len);
	//	_mbsncpy((mchar_t *)p1,(mchar_t *)p2,len);
	//	p1[len] = *_safestring((achar_t *)NULL);
	//	return p1;
	//}
	static wchar_t *_copy(wchar_t *p1, const wchar_t *p2, size_t c) {
		size_t len = _length(p2);
		len = min(c,len);
		wcsncpy(p1,p2,len);
		p1[len] = *_safestring((wchar_t *)NULL);
		return p1;
	}
	// ��r(�啶������������ʂ���)
	static int _compare(const achar_t *p1, const achar_t *p2) { return _mbscmp((mchar_t *)p1,(mchar_t *)p2); }
	static int _compare(const wchar_t *p1, const wchar_t *p2) { return wcscmp(p1,p2); }
	// ��r(�啶������������ʂ���E����������)
	//static int _compare(const achar_t *p1, const achar_t *p2, size_t c) { return _mbsncmp((mchar_t *)p1,(mchar_t *)p2,c); }
	static int _compare(const achar_t *p1, const achar_t *p2, size_t c) { return strncmp(p1,p2,c); }
	static int _compare(const wchar_t *p1, const wchar_t *p2, size_t c) { return wcsncmp(p1,p2,c); }
	// ��r(�啶������������ʂ��Ȃ�)
	static int _icompare(const achar_t *p1, const achar_t *p2) { return _mbsicmp((mchar_t *)p1,(mchar_t *)p2); }
	static int _icompare(const wchar_t *p1, const wchar_t *p2) { return wcsicmp(p1,p2); }
	// ��r(�啶������������ʂ��Ȃ��E����������)
	//static int _icompare(const achar_t *p1, const achar_t *p2, size_t c) { return _mbsnicmp((mchar_t *)p1,(mchar_t *)p2,c); }
	static int _icompare(const achar_t *p1, const achar_t *p2, size_t c) { return strnicmp(p1,p2,c); }
	static int _icompare(const wchar_t *p1, const wchar_t *p2, size_t c) { return wcsnicmp(p1,p2,c); }
	// MBS<->UNICODE�ϊ��R�s�[
	static achar_t *_copy(achar_t *p1, const wchar_t *p2) {
		if(grow_list) {
		} else {
			int n = ::WideCharToMultiByte(CP_ACP,WC_SEPCHARS,p2,-1,p1,(int)array_len,NULL,NULL);
			CHECKRESULT_API(n);
		}
		return p1;
	}
	static wchar_t *_copy(wchar_t *p1, const achar_t *p2) {
		if(grow_list) {
		} else {
			int n = ::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,p2,-1,p1,(int)array_len);
			CHECKRESULT_API(n);
		}
		return p1;
	}
	// �e��ϊ�(�啶����)
	//static void _upcase(achar_t *p) { _strupr(p); }
	static void _upcase(achar_t *p) { _mbsupr((mchar_t *)p); }
	static void _upcase(wchar_t *p) { _wcsupr(p); }
	// �e��ϊ�(��������)
	//static void _downcase(achar_t *p) { _strlwr(p); }
	static void _downcase(achar_t *p) { _mbslwr((mchar_t *)p); }
	static void _downcase(wchar_t *p) { _wcslwr(p); }
	// �e��ϊ�(���])
	//static void _reverse(achar_t *p) { _strrev(p); }
	static void _reverse(achar_t *p) { _mbsrev((mchar_t *)p); }
	static void _reverse(wchar_t *p) { _wcsrev(p); }
	// ������̐ڑ�
	//static achar_t *_concat(achar_t *p1, const achar_t *p2) { return strcat(p2); }
	static achar_t *_concat(achar_t *p1, const achar_t *p2) { return (achar_t *)_mbscat((mchar_t *)p1,(mchar_t *)(p2)); }
	static wchar_t *_concat(wchar_t *p1, const wchar_t *p2) { return wcscat(p1,p2); }
	// �t�H�[�}�b�g
	static int _format(achar_t *buf, const achar_t *form, va_list args) { return vsprintf(buf,form,args); }
	static int _format(wchar_t *buf, const wchar_t *form, va_list args) { return vswprintf(buf,form,args); }
	static int _nformat(achar_t *buf, size_t c, const achar_t *form, va_list args) { return _vsnprintf(buf,c,form,args); }
	static int _nformat(wchar_t *buf, size_t c, const wchar_t *form, va_list args) { return _vsnwprintf(buf,c,form,args); }
	// �����E������̌���
	static achar_t *_find(const achar_t *p, achar_t c) { return (achar_t *)_mbschr((mchar_t *)p,(mchar_t)c); }
	static wchar_t *_find(const wchar_t *p, wchar_t c) { return wcschr(p,c); }
	static achar_t *_find(const achar_t *p, const achar_t *s) { return (achar_t *)_mbsstr((mchar_t *)p,(mchar_t *)s); }
	static wchar_t *_find(const wchar_t *p, const wchar_t *s) { return wcsstr(p,s); }
	// �����E������̌���(�͈͎w���)
	static achar_t *_find(const achar_t *p, achar_t c, size_t n) {
		achar_t *r = (achar_t *)_mbschr((mchar_t *)p,(mchar_t)c);
		return r<&(p[n])?r:NULL;
	}
	static wchar_t *_find(const wchar_t *p, wchar_t c, size_t n) {
		wchar_t *r = wcschr(p,c);
		return r<&(p[n])?r:NULL;
	}
	static achar_t *_find(const achar_t *p, const achar_t *s, size_t n) {
		achar_t *r = (achar_t *)_mbsstr((mchar_t *)p,(mchar_t *)s);
		return (r-_length(s))<&(p[n])?r:NULL;
	}
	static wchar_t *_find(const wchar_t *p, const wchar_t *s, size_t n) {
		wchar_t *r = wcsstr(p,s);
		return (r-_length(s))<&(p[n])?r:NULL;
	}
	// �����񒷂̎擾
	static size_t _length(const achar_t *p) { return strlen(p); }
	//static size_t _length(const achar_t *p) { return _mbslen((mchar_t *)p); }
	static size_t _length(const wchar_t *p) { return wcslen(p); }
private:
	union {
		tchar_t m_rep[max(2,array_len)];	// �K���Q�������ȏ�I
		tchar_t *m_prep;
	};
};
/*****************************************************************
	�Œ�T�C�Y������^�N���X
*****************************************************************/
template <size_t array_len> class TString : public TStringBase<TString<array_len>,TCHAR,array_len,false,NULL>
{
public:
	DECLARE_SYNONYM(TYPE,TYPE);
	typedef TCHAR tchar_t;
	typedef char achar_t;
	typedef unsigned char mchar_t;
	operator tchar_t *() const { return object(); }
public:
	TString() { let(_safestring((tchar_t *)NULL)); }
	TString(const TYPE &t) { if(&t!=this) let(t.object()); }
	TString(size_t n, const tchar_t *form, ...) { va_list args; va_start(args,form); format(n,form,args); }
	TString(const tchar_t *p, size_t c) { let(p,c); }
	TString(const achar_t *p) { let(p); }
	TString(const wchar_t *p) { let(p); }
	// �e��I�y���[�^(�񍀉��Z�q�ō��l�̈Öٕϊ����󂯓���Ȃ����Ƃɒ���)
	TYPE &operator =(const tchar_t *p) { let(p); return *((LPTYPE)this); }
	TYPE &operator+=(const tchar_t *p) { concat(p); return *((LPTYPE)this); }
	bool operator==(const tchar_t *p) const { return isEqual(p); }
	bool operator!=(const tchar_t *p) const { return !isEqual(p); }
	bool operator< (const tchar_t *p) const { return compare(p)<0; }
	bool operator> (const tchar_t *p) const { return compare(p)>0; }
	TYPE operator+ (const tchar_t *p) const { TYPE t=*((LPTYPE)this); t.concat(p); return t; }
	// �e��I�y���[�^(TString����LPTSTR�ւ̈Öٕϊ����󂯓���邽�߂̐錾)
	TYPE &operator =(tchar_t *p) { let(p); return *((LPTYPE)this); }
	TYPE &operator+=(tchar_t *p) { concat(p); return *((LPTYPE)this); }
	bool operator==(tchar_t *p) const { return isEqual(p); }
	bool operator!=(tchar_t *p) const { return !isEqual(p); }
	bool operator< (tchar_t *p) const { return compare(p)<0; }
	bool operator> (tchar_t *p) const { return compare(p)>0; }
	TYPE operator+ (tchar_t *p) const { TYPE t=*((LPTYPE)this); t.concat(p); return t; }
};
template <size_t array_len> class TStringA : public TStringBase<TStringA<array_len>,char,array_len,false,NULL>
{
public:
	DECLARE_SYNONYM(TStringA,TYPE);
	typedef char tchar_t;
	typedef char achar_t;
	typedef unsigned char mchar_t;
	operator tchar_t *() const { return object(); }
public:
	TStringA() { let(_safestring((tchar_t *)NULL)); }
	TStringA(const TYPE &t) { if(&t!=this) let(t.object()); }
	TStringA(size_t n, const tchar_t *form, ...) { va_list args; va_start(args,form); format(n,form,args); }
	TStringA(const tchar_t *p, size_t c) { let(p,c); }
	TStringA(const achar_t *p) { let(p); }
	TStringA(const wchar_t *p) { let(p); }
	// �e��I�y���[�^(�񍀉��Z�q�ō��l�̈Öٕϊ����󂯓���Ȃ����Ƃɒ���)
	TYPE &operator =(const tchar_t *p) { let(p); return *((LPTYPE)this); }
	TYPE &operator+=(const tchar_t *p) { concat(p); return *((LPTYPE)this); }
	bool operator==(const tchar_t *p) const { return isEqual(p); }
	bool operator!=(const tchar_t *p) const { return !isEqual(p); }
	bool operator< (const tchar_t *p) const { return compare(p)<0; }
	bool operator> (const tchar_t *p) const { return compare(p)>0; }
	TYPE operator+ (const tchar_t *p) const { TYPE t=*((LPTYPE)this); t.concat(p); return t; }
	// �e��I�y���[�^(TString����LPTSTR�ւ̈Öٕϊ����󂯓���邽�߂̐錾)
	TYPE &operator =(tchar_t *p) { let(p); return *((LPTYPE)this); }
	TYPE &operator+=(tchar_t *p) { concat(p); return *((LPTYPE)this); }
	bool operator==(tchar_t *p) const { return isEqual(p); }
	bool operator!=(tchar_t *p) const { return !isEqual(p); }
	bool operator< (tchar_t *p) const { return compare(p)<0; }
	bool operator> (tchar_t *p) const { return compare(p)>0; }
	TYPE operator+ (tchar_t *p) const { TYPE t=*((LPTYPE)this); t.concat(p); return t; }
};
template <size_t array_len> class TStringW : public TStringBase<TStringW<array_len>,wchar_t,array_len,false,NULL>
{
public:
	DECLARE_SYNONYM(TStringW,TYPE);
	typedef wchar_t tchar_t;
	typedef char achar_t;
	typedef unsigned char mchar_t;
	operator tchar_t *() const { return object(); }
public:
	TStringW() { let(_safestring((tchar_t *)NULL)); }
	TStringW(const TYPE &t) { if(&t!=this) let(t.object()); }
	TStringW(size_t n, const tchar_t *form, ...) { va_list args; va_start(args,form); format(n,form,args); }
	TStringW(const tchar_t *p, size_t c) { let(p,c); }
	TStringW(const achar_t *p) { let(p); }
	TStringW(const wchar_t *p) { let(p); }
	// �e��I�y���[�^(�񍀉��Z�q�ō��l�̈Öٕϊ����󂯓���Ȃ����Ƃɒ���)
	TYPE &operator =(const tchar_t *p) { let(p); return *((LPTYPE)this); }
	TYPE &operator+=(const tchar_t *p) { concat(p); return *((LPTYPE)this); }
	bool operator==(const tchar_t *p) const { return isEqual(p); }
	bool operator!=(const tchar_t *p) const { return !isEqual(p); }
	bool operator< (const tchar_t *p) const { return compare(p)<0; }
	bool operator> (const tchar_t *p) const { return compare(p)>0; }
	TYPE operator+ (const tchar_t *p) const { TYPE t=*((LPTYPE)this); t.concat(p); return t; }
	// �e��I�y���[�^(TString����LPTSTR�ւ̈Öٕϊ����󂯓���邽�߂̐錾)
	TYPE &operator =(tchar_t *p) { let(p); return *((LPTYPE)this); }
	TYPE &operator+=(tchar_t *p) { concat(p); return *((LPTYPE)this); }
	bool operator==(tchar_t *p) const { return isEqual(p); }
	bool operator!=(tchar_t *p) const { return !isEqual(p); }
	bool operator< (tchar_t *p) const { return compare(p)<0; }
	bool operator> (tchar_t *p) const { return compare(p)>0; }
	TYPE operator+ (tchar_t *p) const { TYPE t=*((LPTYPE)this); t.concat(p); return t; }
};

/*****************************************************************
	������^�e���v���[�g�̉��p
*****************************************************************/
template <size_t len> class TIntAtom : public TString<len>
{
public:
	DECLARE_SYNONYM(TIntAtom<len>,TYPE);
	DECLARE_SYNONYM(TString<len>,BASE);
public:
	TIntAtom() { ASSERT(1<chars()); let(WORD(0)); }
	TIntAtom(size_t n, LPCTSTR form, ...) { ASSERT(1<chars()); va_list args; va_start(args,form); format(n,form,args); }
	TIntAtom(LPCTSTR p) { ASSERT(1<chars()); let(p); }
	explicit TIntAtom(LPCWSTR p) { ASSERT(1<chars()); let(p); }
public:
	// �e�푀��
	void let(LPCTSTR p) { if(HIWORD(p)) { BASE::let(p); } else { let(LOWORD(p)); } }
	void let(LPCWSTR p) { if(HIWORD(p)) { BASE::let(p); } else { let(LOWORD(p)); } }
	void let(WORD w) { setEmpty(); ((WORD&)(*this))=w; }
	bool isEqual(LPCTSTR p) const { return object()==p; }
public:
	// �N���X���̎擾/�ݒ�
	LPTSTR object() const { return isEmpty() ? LPTSTR((WORD&)(*this)) : BASE::object(); }
	operator LPTSTR() const { return object(); }
	operator WORD() const { return *((LPWORD)(&((LPTYPE)this)->BASE::object()[1])); }
public:
	// �e��I�y���[�^
	TYPE &operator=(LPCTSTR p) { let(p); return *this; }
	bool operator==(LPCTSTR p) { return isEqual(p); }
	bool operator!=(LPCTSTR p) { return !isEqual(p); }
};

// ��p������(MAX_PATH�����\���ɑ傫���l�ł��邱��)
const size_t TSTRING_IMPLICIT_LENGTH = 512;
typedef TString <TSTRING_IMPLICIT_LENGTH> TSTRING;
typedef TIntAtom <TSTRING_IMPLICIT_LENGTH> TINTATOM;
typedef TIntAtom <TSTRING_IMPLICIT_LENGTH> TINTRES;

//template class TString<TSTRING_IMPLICIT_LENGTH>;
//template class TIntAtom<TSTRING_IMPLICIT_LENGTH>;

/*****************************************************************
	�����\�ȕ�����^�N���X
*****************************************************************/
const size_t CStringGrowList[2][10] = {
	{ 512, 1024, 1024, 0 },
	{ 1024, 0 },
};
class CString : public TStringBase <CString,TCHAR,0,true,(size_t **)1>
{
public:
	DECLARE_SYNONYM(CString,TYPE);
	typedef TCHAR tchar_t;
	typedef char achar_t;
	typedef unsigned char mchar_t;
	operator tchar_t *() const { return object(); }
public:
	CString() { let(_safestring((tchar_t *)NULL)); }
	CString(const TYPE &t) { if(&t!=this) let(t.object()); }
	CString(size_t n, const tchar_t *form, ...) { va_list args; va_start(args,form); format(n,form,args); }
	CString(const tchar_t *p, size_t c) { let(p,c); }
	CString(const achar_t *p) { let(p); }
	CString(const wchar_t *p) { let(p); }
	virtual ~CString() { destroy(); }
public:
	// �e��I�y���[�^(�񍀉��Z�q�ō��l�̈Öٕϊ����󂯓���Ȃ����Ƃɒ���)
	TYPE &operator =(const tchar_t *p) { let(p); return *((LPTYPE)this); }
	TYPE &operator+=(const tchar_t *p) { concat(p); return *((LPTYPE)this); }
	bool operator==(const tchar_t *p) const { return isEqual(p); }
	bool operator!=(const tchar_t *p) const { return !isEqual(p); }
	bool operator< (const tchar_t *p) const { return compare(p)<0; }
	bool operator> (const tchar_t *p) const { return compare(p)>0; }
	TYPE operator+ (const tchar_t *p) const { TYPE t=*((LPTYPE)this); t.concat(p); return t; }
	// �e��I�y���[�^(TString����LPTSTR�ւ̈Öٕϊ����󂯓���邽�߂̐錾)
	TYPE &operator =(tchar_t *p) { let(p); return *((LPTYPE)this); }
	TYPE &operator+=(tchar_t *p) { concat(p); return *((LPTYPE)this); }
	bool operator==(tchar_t *p) const { return isEqual(p); }
	bool operator!=(tchar_t *p) const { return !isEqual(p); }
	bool operator< (tchar_t *p) const { return compare(p)<0; }
	bool operator> (tchar_t *p) const { return compare(p)>0; }
	TYPE operator+ (tchar_t *p) const { TYPE t=*((LPTYPE)this); t.concat(p); return t; }
};
class CStringA : public TStringBase <CStringA,char,0,true,(size_t **)1>
{
public:
	DECLARE_SYNONYM(CStringA,TYPE);
	typedef char tchar_t;
	typedef char achar_t;
	typedef unsigned char mchar_t;
	operator tchar_t *() const { return object(); }
public:
	CStringA() { let(_safestring((tchar_t *)NULL)); }
	CStringA(const TYPE &t) { if(&t!=this) let(t.object()); }
	CStringA(size_t n, const tchar_t *form, ...) { va_list args; va_start(args,form); format(n,form,args); }
	CStringA(const tchar_t *p, size_t c) { let(p,c); }
	CStringA(const achar_t *p) { let(p); }
	CStringA(const wchar_t *p) { let(p); }
	virtual ~CStringA() { destroy(); }
public:
	// �e��I�y���[�^(�񍀉��Z�q�ō��l�̈Öٕϊ����󂯓���Ȃ����Ƃɒ���)
	TYPE &operator =(const tchar_t *p) { let(p); return *((LPTYPE)this); }
	TYPE &operator+=(const tchar_t *p) { concat(p); return *((LPTYPE)this); }
	bool operator==(const tchar_t *p) const { return isEqual(p); }
	bool operator!=(const tchar_t *p) const { return !isEqual(p); }
	bool operator< (const tchar_t *p) const { return compare(p)<0; }
	bool operator> (const tchar_t *p) const { return compare(p)>0; }
	TYPE operator+ (const tchar_t *p) const { TYPE t=*((LPTYPE)this); t.concat(p); return t; }
	// �e��I�y���[�^(TString����LPTSTR�ւ̈Öٕϊ����󂯓���邽�߂̐錾)
	TYPE &operator =(tchar_t *p) { let(p); return *((LPTYPE)this); }
	TYPE &operator+=(tchar_t *p) { concat(p); return *((LPTYPE)this); }
	bool operator==(tchar_t *p) const { return isEqual(p); }
	bool operator!=(tchar_t *p) const { return !isEqual(p); }
	bool operator< (tchar_t *p) const { return compare(p)<0; }
	bool operator> (tchar_t *p) const { return compare(p)>0; }
	TYPE operator+ (tchar_t *p) const { TYPE t=*((LPTYPE)this); t.concat(p); return t; }
};
class CStringW : public TStringBase <CStringW,wchar_t,0,true,(size_t **)1>
{
public:
	DECLARE_SYNONYM(CStringW,TYPE);
	typedef wchar_t tchar_t;
	typedef char achar_t;
	typedef unsigned char mchar_t;
	operator tchar_t *() const { return object(); }
public:
	CStringW() { let(_safestring((tchar_t *)NULL)); }
	CStringW(const TYPE &t) { if(&t!=this) let(t.object()); }
	CStringW(size_t n, const tchar_t *form, ...) { va_list args; va_start(args,form); format(n,form,args); }
	CStringW(const tchar_t *p, size_t c) { let(p,c); }
	CStringW(const achar_t *p) { let(p); }
	CStringW(const wchar_t *p) { let(p); }
	virtual ~CStringW() { destroy(); }
public:
	// �e��I�y���[�^(�񍀉��Z�q�ō��l�̈Öٕϊ����󂯓���Ȃ����Ƃɒ���)
	TYPE &operator =(const tchar_t *p) { let(p); return *((LPTYPE)this); }
	TYPE &operator+=(const tchar_t *p) { concat(p); return *((LPTYPE)this); }
	bool operator==(const tchar_t *p) const { return isEqual(p); }
	bool operator!=(const tchar_t *p) const { return !isEqual(p); }
	bool operator< (const tchar_t *p) const { return compare(p)<0; }
	bool operator> (const tchar_t *p) const { return compare(p)>0; }
	TYPE operator+ (const tchar_t *p) const { TYPE t=*((LPTYPE)this); t.concat(p); return t; }
	// �e��I�y���[�^(TString����LPTSTR�ւ̈Öٕϊ����󂯓���邽�߂̐錾)
	TYPE &operator =(tchar_t *p) { let(p); return *((LPTYPE)this); }
	TYPE &operator+=(tchar_t *p) { concat(p); return *((LPTYPE)this); }
	bool operator==(tchar_t *p) const { return isEqual(p); }
	bool operator!=(tchar_t *p) const { return !isEqual(p); }
	bool operator< (tchar_t *p) const { return compare(p)<0; }
	bool operator> (tchar_t *p) const { return compare(p)>0; }
	TYPE operator+ (tchar_t *p) const { TYPE t=*((LPTYPE)this); t.concat(p); return t; }
};

/*****************************************************************
	Pascal������(��Ƀ}�b�v�p)
*****************************************************************/
// Pascal������̌�����A�����񒷂͍ő�255�����Ɍ��肳���
// C������ɃR�s�[����ꍇ256�o�C�g�ȏ��z�肷��K�v�͂Ȃ�
class PString
{
public:
	size_t length() const { return byLength; }
	size_t size() const { return sizeof(*this) + length(); }
	LPBYTE skip() const { return LPBYTE(this) + size(); }	// ���A���C�����g�����Ȃ��Ƃɒ���
	PString *next() const { return (PString *)skip(); }
	LPBYTE object() const { return LPBYTE(this) + sizeof(*this); }
private:
	BYTE byLength;
};

/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

#endif	/* __INC_HMC_STRING__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/

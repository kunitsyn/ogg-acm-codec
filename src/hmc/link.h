/*
	�����N���X�g�T�|�[�g
	Copyright (C)1998-2001 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_HMC_LINK__
#define __INC_HMC_LINK__

#include "util.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
class BLinkList;
template <class> class CLinkList;
template <class> class TLinkList;
template <class> class TLinkTree;

/*****************************************************************
	�o���������N���X�g���N���X(���ڎg��Ȃ�)
*****************************************************************/
class BLinkList
{
public:
	DECLARE_SYNONYM(BLinkList,TYPE);
	operator LPTYPE() { return this; }
	friend TYPE;
public:
	BLinkList() { _prev(NULL); _next(NULL); }
	~BLinkList() { unlink(); }
protected:
	// �����N���ݒ�
	LPTYPE _prev(LPTYPE p) { return m_prev=p; }
	LPTYPE _next(LPTYPE p) { return m_next=p; }
public:
	// �����N�̐ؒf
	void unlink() {
		if(prev()) { prev()->_next(next()); }
		if(next()) { next()->_prev(prev()); }
		_prev(NULL);
		_next(NULL);
	}
	void purge() {
		LPTYPE p = next();
		unlink();
		if(p) p->purge();
	}
	// �����N��̕ύX(�����̒����p)
	void next(LPTYPE p) {
		if(next()) {
			next()->_prev(NULL);
		}
		_next(p);
		if(p) {
			if(p->prev()) {
				p->prev()->_next(NULL);
			}
			p->_prev(this);
		}
	}
	// �����N��̕ύX(�����̒��O��p)
	void prev(LPTYPE p) {
		if(prev()) {
			prev()->_next(NULL);
		}
		_prev(p);
		if(p) {
			if(p->next()) {
				p->next()->_prev(NULL);
			}
			p->_next(this);
		}
	}
	// �����N�̑}��(�����̒����p)
	void after(LPTYPE p) {
		ASSERT(p);
		p->unlink();
		p->_prev(this);
		p->_next(next());
		if(next()) next()->_prev(p);
		_next(p);
	}
	// �����N�̑}��(�����̒��O��p)
	void before(LPTYPE p) {
		ASSERT(p);
		p->unlink();
		p->_next(this);
		p->_prev(prev());
		if(prev()) prev()->_next(p);
		_prev(p);
	}
	// �����N�ʒu�̌���
	void exchange(LPTYPE p) {
		ASSERT(p);
		if(next()==p) {
			before(p);
		} else if(prev()==p) {
			after(p);
		} else {
			LPTYPE pPrev1 = prev();
			LPTYPE pNext1 = next();
			LPTYPE pPrev2 = p->prev();
			LPTYPE pNext2 = p->next();
			if(pPrev1) {
				pPrev1->after(p);
			} else if(pNext1) {
				pNext1->before(p);
			} else {
				p->unlink();
			}
			if(pPrev2) {
				pPrev2->after(this);
			} else if(pNext2) {
				pNext2->before(this);
			} else {
				unlink();
			}
		}
	}
	// �擪/�I�[�֑}��
	void top(LPTYPE p) { top()->before(p); }
	void end(LPTYPE p) { end()->after (p); }
	// �D�揇�ʈʒu�̈ړ�
	void forward()  { if(next()) next()->after (this); }
	void backward() { if(prev()) prev()->before(this); }
public:
	// �����N���擾
	LPTYPE prev() const { return m_prev; }
	LPTYPE next() const { return m_next; }
	LPTYPE top() const { return prev() ? prev()->top() : (LPTYPE)this; }
	LPTYPE end() const { return next() ? next()->end() : (LPTYPE)this; }
	// �����N�ԋ����̑���
	INT distance(LPTYPE obj) const {
		ASSERT(obj);
		INT i = 0;
		LPTYPE p;
		for(p=(LPTYPE)this; p!=NULL; p=p->next()) {
			if(p==obj) return i;
			i--;
		}
		i = 0;
		for(p=(LPTYPE)this; p!=NULL; p=p->prev()) {
			if(p==obj) return i;
			i--;
		}
		return 0;
	}
	// ��������̗v�f����Ԃ�(�������܂�)
	size_t count() const {
		size_t n=1;
		for(LPTYPE p=next(); p; p=p->next()) { n++; }
		return n;
	}
	// ���������n�Ԗڂ̗v�f��Ԃ�(�������܂�)
	LPTYPE nth(size_t n) const {
		LPTYPE p=(LPTYPE)this;
		for(; p&&n; p=p->next()) { n--; }
		return n ? NULL : p;
	}
	LPTYPE operator[](size_t n) const { return nth(n); }
	LPTYPE operator()(size_t n) const { return nth(n); }
	// obj���܂܂�邩����(�����N������)
	bool isContain(LPTYPE obj) const {
		ASSERT(obj);
		bool b=false;
		for(LPTYPE p=top(); p!=NULL; p=p->next()) {
			if(p==obj) { b=true; break; }
		}
		return b;
	}
private:
	LPTYPE m_prev;	// ���O�ւ̃����N
	LPTYPE m_next;	// ����ւ̃����N
};

/*****************************************************************
	�o���������N���X�g(�Ԃ�l��OBJ�^��BLinkList)
*****************************************************************/
template <class OBJ> class CLinkList : public BLinkList
{
public:
	DECLARE_SYNONYM(CLinkList,TYPE);
	DECLARE_SYNONYM(BLinkList,BASE);
	DECLARE_SYNONYM(OBJ,OBJ);
	operator LPTYPE() { return this; }
	operator LPOBJ() { return (LPOBJ)this; }
public:
	// �����N���ݒ�
	void unlink() { BASE::unlink(); }
	void next(LPTYPE p) { BASE::next(p); }
	void prev(LPTYPE p) { BASE::prev(p); }
	void after(LPTYPE p) { BASE::after(p); }
	void before(LPTYPE p) { BASE::before(p); }
	void exchange(LPTYPE p) { BASE::exchange(p); }
	void top(LPTYPE p) { BASE::top(p); }
	void end(LPTYPE p) { BASE::end(p); }
	void forward()  { BASE::forward(); }
	void backward() { BASE::backward(); }
	// �����N���擾
	LPOBJ prev() const { return (LPOBJ)BASE::prev(); }
	LPOBJ next() const { return (LPOBJ)BASE::next(); }
	LPOBJ top() const { return (LPOBJ)BASE::top(); }
	LPOBJ end() const { return (LPOBJ)BASE::end(); }
	INT distance(LPTYPE p) const { return BASE::distance(p); }
	size_t count() const { return BASE::count(); }
	LPOBJ nth(size_t n) const { return (LPOBJ)BASE::nth(n); }
	LPOBJ operator[](size_t n) const { return (LPOBJ)BASE::nth(n); }
	LPOBJ operator()(size_t n) const { return (LPOBJ)BASE::nth(n); }
	bool isContain(LPTYPE p) const { return BASE::isContain(p); }
};

/*****************************************************************
	�o���������N���X�g(�����o�Ƃ��Ė��ߍ���Ŏg�p����)
*****************************************************************/
template <class OBJ> class TLinkList : private BLinkList
{
public:
	DECLARE_SYNONYM(TLinkList,TYPE);
	DECLARE_SYNONYM(BLinkList,BASE);
	DECLARE_SYNONYM(OBJ,OBJ);
	operator LPTYPE() { return this; }
	operator LPOBJ() { return object(); }
public:
	TLinkList() { object(NULL); }
	TLinkList(LPOBJ p) { object(p); }
	~TLinkList() {}
public:
	// �I�u�W�F�N�g�|�C���^�̐ݒ�/�擾
	void object(LPOBJ p) { m_obj=p; }
	LPOBJ object() const { return m_obj; }
public:
	// �����N���ݒ�
	void unlink() { BASE::unlink(); }
	void next(LPTYPE p) { BASE::next(p); }
	void prev(LPTYPE p) { BASE::prev(p); }
	void after(LPTYPE p) { BASE::after(p); }
	void before(LPTYPE p) { BASE::before(p); }
	void exchange(LPTYPE p) { BASE::exchange(p); }
	void top(LPTYPE p) { BASE::top(p); }
	void end(LPTYPE p) { BASE::end(p); }
	void forward()  { BASE::forward(); }
	void backward() { BASE::backward(); }
	// �����N���擾
	LPTYPE prev() const { return (LPTYPE)BASE::prev(); }
	LPTYPE next() const { return (LPTYPE)BASE::next(); }
	LPTYPE top() const { return (LPTYPE)BASE::top(); }
	LPTYPE end() const { return (LPTYPE)BASE::end(); }
	INT distance(LPTYPE p) const { return BASE::distance(p); }
	size_t count() const { return BASE::count(); }
	LPTYPE nth(size_t n) const { return (LPTYPE)BASE::nth(n); }
	LPTYPE operator[](size_t n) const { return (LPTYPE)BASE::nth(n); }
	LPTYPE operator()(size_t n) const { return (LPTYPE)BASE::nth(n); }
	bool isContain(LPTYPE p) const { return BASE::isContain(p); }
private:
	LPOBJ m_obj;	// OBJ�|�C���^
};

/*****************************************************************
	�o���������N���X�g�𗘗p���������N�c���[
*****************************************************************/
#pragma warning(push)
#pragma warning(disable: 4355)
template <class OBJ> class TLinkTree
{
public:
	DECLARE_SYNONYM(TLinkTree,TYPE);
	DECLARE_SYNONYM(TLinkList<TYPE>,LINK);
	DECLARE_SYNONYM(TLinkList<TYPE>,PROP);
	DECLARE_SYNONYM(OBJ,OBJ);
	operator LPTYPE() { return this; }
	operator LPOBJ() { return object(); }
	friend TYPE;
public:
	TLinkTree() : m_link(this), m_prop(this) { object(NULL); }
	TLinkTree(LPOBJ p) : m_link(this), m_prop(this) { object(p); }
	~TLinkTree() { purge(); }
public:
	// �I�u�W�F�N�g�|�C���^�̐ݒ�/�擾
	void object(LPOBJ p) { m_obj=p; }
	LPOBJ object() const { return m_obj; }
public:
	// �����N���̐ݒ�
	void next(LPTYPE p) { m_link.next(&p->m_link); }
	void prev(LPTYPE p) { m_link.prev(&p->m_link); }
	void after(LPTYPE p) { m_link.after(&p->m_link); }
	void before(LPTYPE p) { m_link.before(&p->m_link); }
	void exchange(LPTYPE p) {
		LPPROP pPrev1 = m_prop.prev();
		LPPROP pPrev2 = p->m_prop.prev();
		m_prop.prev(pPrev2);
		p->m_prop.prev(pPrev1);
		m_link.exchange(&p->m_link);
	}
	void top(LPTYPE p) { m_link.top(&p->m_link); }
	void end(LPTYPE p) { m_link.end(&p->m_link); }
	void forward()  { LPTYPE p=getOwner(); setOwner(NULL); m_link.forward (); setOwner(p); }
	void backward() { LPTYPE p=getOwner(); setOwner(NULL); m_link.backward(); setOwner(p); }
	void unlink() { m_prop.unlink(); m_link.unlink(); }
	void purge() {
		m_prop.prev(NULL);
		for(LPTYPE p=getChild(),n; p; p=n) {
			n=p->next();
			p->purge();
		}
		unlink();
	}
public:
	// �����N���̎擾
	LPTYPE prev() const { LPLINK p=m_link.prev(); return p?p->object():NULL; }
	LPTYPE next() const { LPLINK p=m_link.next(); return p?p->object():NULL; }
	LPTYPE top() const { LPLINK p=m_link.top(); return p?p->object():NULL; }
	LPTYPE end() const { LPLINK p=m_link.end(); return p?p->object():NULL; }
	INT distance(LPTYPE p) const { return m_link.distance(&p->m_link); }
	size_t count() const { return m_link.count(); }
	LPTYPE nth(size_t n) const { LPLINK p=m_link.nth(n); return p?p->object():NULL; }
	LPTYPE operator[](size_t n) const { return nth(n); }
	LPTYPE operator()(size_t n) const { return nth(n); }
	bool isContain(LPTYPE p) const { return m_link.isContain(&p->m_link); }
public:
	// �e�q�֌W�̐ݒ�
	void setOwner(LPTYPE p) {
		if(p) {
			if(prev()) {
				m_link.unlink();
			} else {
				if(next()) {
					next()->m_prop.prev(m_prop.prev());
					m_link.next(NULL);
				} else if(m_prop.prev()) {
					m_prop.prev()->object()->m_prop.next(m_link.next());
				}
			}
			p->addChild(this);
		} else {
			if(prev()) {
				m_link.unlink();
			} else {
				if(next()) {
					next()->m_prop.prev(m_prop.prev());
					m_link.next(NULL);
				} else if(m_prop.prev()) {
					m_prop.prev()->object()->m_prop.next(m_link.next());
				}
			}
		}
	}
	void setOwner(LPTYPE p, size_t n) {
		if(p) {
			if(prev()) {
				m_link.unlink();
			} else {
				if(next()) {
					next()->m_prop.prev(m_prop.prev());
					m_link.next(NULL);
				} else if(m_prop.prev()) {
					m_prop.prev()->object()->m_prop.next(m_link.next());
				}
			}
			p->addChild(this,n);
		} else {
			if(prev()) {
				m_link.unlink();
			} else {
				if(next()) {
					next()->m_prop.prev(m_prop.prev());
					m_link.next(NULL);
				} else if(m_prop.prev()) {
					m_prop.prev()->object()->m_prop.next(m_link.next());
				}
			}
		}
	}
	void addChild(LPTYPE p) {
		p->setOwner(NULL);
		LPTYPE pChild = getChild();
		if(pChild) {
			pChild->end(p);
		} else {
			m_prop.next(&p->m_prop);
		}
	}
	bool addChild(LPTYPE p, size_t n) {
		p->setOwner(NULL);
		if(n==-1) {
			n = getChildCount();
		}
		if(n==0) {
			LPPROP pPrev = m_prop.next();
			if(pPrev) {
				pPrev->object()->prev(p);
			}
			m_prop.next(p->m_prop);
			return true;
		} else {
			LPTYPE pChild = getChild();
			if(pChild) {
				LPTYPE pPrev = pChild->nth(n-1);
				if(pPrev) {
					pPrev->after(p);
					return true;
				}
			}
			return false;
		}
	}
public:
	// �e�q�֌W�̎擾
	size_t getChildCount() const { LPTYPE p=getChild(); return p?p->count():0; }
	LPTYPE getChild() const { LPPROP p=m_prop.next(); return p?p->object():NULL; }
	LPTYPE getChild(size_t n) const { LPTYPE p=getChild(); return p?p->nth(n):NULL; }
	size_t getOrder() const {
		LPTYPE pOwner = getOwner();
		if(pOwner) {
			size_t nChilds = pOwner->getChildCount();
			for(size_t n=0; n<nChilds; n++) {
				LPTYPE pObj = pOwner->getChild(n);
				ASSERT(pObj);
				if(pObj==this) {
					return n;
				}
			}
		}
		return -1;
	}
	LPTYPE getOwner() const { LPPROP p=top()->m_prop.prev(); return p?p->object():NULL; }
	LPTYPE getRoot() const {
		LPTYPE pOwner = (LPTYPE)this;
		for(LPTYPE p=pOwner; p; p=p->getOwner()) {
			pOwner = p;
		}
		return pOwner;
	}
private:
	LINK m_link;	// �Z�탊���N
	PROP m_prop;	// �q�����N
	LPOBJ m_obj;	// OBJ�|�C���^
};
#pragma warning(pop)
/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

#endif	/* __INC_HMC_LINK__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/

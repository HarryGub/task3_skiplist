////////////////////////////////////////////////////////////////////////////////
// Module Name:  skip_list.h/hpp
// Authors:      Leonid Dworzanski, Sergey Shershakov
// Version:      2.0.0
// Date:         28.10.2018
//
// This is a part of the course "Algorithms and Data Structures"
// provided by  the School of Software Engineering of the Faculty
// of Computer Science at the Higher School of Economics.
////////////////////////////////////////////////////////////////////////////////

// !!! DO NOT include skip_list.h here, 'cause it leads to circular refs. !!!

#include <cstdlib>

//==============================================================================
// class NodeSkipList
//==============================================================================

template <class Value, class Key, int numLevels>
void NodeSkipList<Value, Key, numLevels>::clear(void)
{
    for (int i = 0; i < numLevels; ++i)
        Base::nextJump[i] = 0;

    Base::levelHighest = -1;
}

//------------------------------------------------------------------------------

template <class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(void)
{
    clear();
}

//------------------------------------------------------------------------------

template <class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(const Key& tkey)
{
    clear();

    Base::Base::key = tkey;
}

//------------------------------------------------------------------------------

template <class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(const Key& tkey, const Value& val)
{
    clear();

    Base::Base::key = tkey;
    Base::Base::value = val;
}


//==============================================================================
// class SkipList
//==============================================================================

template <class Value, class Key, int numLevels>
SkipList<Value, Key, numLevels>::SkipList(double probability)
{
    _probability = probability;

    // Lets use m_pPreHead as a final sentinel element
    for (int i = 0; i < numLevels; ++i)
        Base::_preHead->nextJump[i] = Base::_preHead;

    Base::_preHead->levelHighest = numLevels - 1;
}

//------------------------------------------------------------------------------

template<class Value, class Key, int numLevels>
SkipList<Value, Key, numLevels>::~SkipList()
{
    Node* t;
    Node* current = Base::_preHead->next;
    while (current != Base::_preHead)
    {
        t = current->next;
        delete current;
        current = t;
    }
    delete current;
}

//------------------------------------------------------------------------------

template<class Value, class Key, int numLevels>
void SkipList<Value, Key, numLevels>::insert(const Value & val, const Key & key)
{
    Node* current = Base::getPreHead();
    Node* prevNodes[current->levelHighest];
    for (int i = numLevels - 1; i >= 0; i--)
    {
        while (current->nextJump[i]->key < key && current->nextJump[i] != Base::getPreHead())
            current = current->nextJump[i];
        prevNodes[i] = current;
    }
    int numOfLevels = -1;
    while (numOfLevels < Base::getPreHead()->levelHighest && rand() / RAND_MAX < _probability)
        numOfLevels++;
    Node* newN = new Node(key, val);
    newN->levelHighest = numOfLevels;
    while (current->next->key <= key && current->next != Base::getPreHead())
        current = current->next;
    newN->next = current->next;
    current->next = newN;
    for (int i = 0; i <= numOfLevels; i++)
    {
        newN->nextJump[i] = prevNodes[i]->nextJump[i];
        prevNodes[i]->nextJump[i] = newN;
    }
}

//------------------------------------------------------------------------------

template<class Value, class Key, int numLevels>
typename SkipList<Value, Key, numLevels>::Node* SkipList<Value, Key, numLevels>::findFirst(const Key & key) const
{
    Node *current = Base::getPreHead();
    for (int i = numLevels - 1; i >= 0; --i)
        while (current->nextJump[i] != Base::getPreHead() && current->nextJump[i]->key < key)
            current = current->nextJump[i];
    while (current->next != Base::getPreHead() && current->next->key < key)
        current = current->next;
    current = current->next;
    if (current != Base::getPreHead() && current->key == key)
        return current;
    else
        return nullptr;
}

//------------------------------------------------------------------------------

template<class Value, class Key, int numLevels>
typename SkipList<Value, Key, numLevels>::Node  * SkipList<Value, Key, numLevels>::findLastLessThan(const Key & key) const
{
	Node* current = Base::getPreHead();
	for (int i = numLevels - 1; i >= 0; --i)
		while (current->nextJump[i] != Base::getPreHead() && current->nextJump[i]->key < key)
			current = current->nextJump[i];
	if (current)
		return current;
	else
		return Base::getPreHead();
}

//------------------------------------------------------------------------------

template<class Value, class Key, int numLevels>
void SkipList<Value, Key, numLevels>::removeNext(Node * nodeBefore)
{
	if (!nodeBefore || nodeBefore->next == Base::getPreHead() || !nodeBefore->next)
		throw std::invalid_argument("");
	Node* del = nodeBefore->next;
	for (int i = del->levelHighest; i >= 0; i--)
	{
		nodeBefore->nextJump[i] = del->nextJump[i];
		del->nextJump[i] = 0;
	}
	nodeBefore->next = del->next;
	del->next = 0;
	del->levelHighest = -1;
}
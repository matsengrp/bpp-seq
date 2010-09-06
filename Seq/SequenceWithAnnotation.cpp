//
// File: SequenceWithAnnotation.cpp
// Created by: Julien Dutheil
// Created on: Mon Jul 19 2010
//

/*
Copyright or © or Copr. CNRS, (November 17, 2004)

This software is a computer program whose purpose is to provide classes
for sequences analysis.

This software is governed by the CeCILL  license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.
*/

#include "SequenceWithAnnotation.h" // class's header file

#include "AlphabetTools.h"
#include "StringSequenceTools.h"

// From utils:
#include <Utils/TextTools.h>

using namespace bpp;

// From the STL:
#include <iostream>

using namespace std;

/* Constructors: **************************************************************/

SequenceWithAnnotation::SequenceWithAnnotation(const Alphabet* alpha):
  EdSymbolList(alpha),
  name_(),
  comments_()
{}

SequenceWithAnnotation::SequenceWithAnnotation(const std::string& name, const std::string& sequence, const Alphabet* alpha)
throw (BadCharException) :
	EdSymbolList(alpha),
	name_(name),
  comments_()
{
  if (sequence!="")
    setContent(sequence);
}

SequenceWithAnnotation::SequenceWithAnnotation(const std::string& name, const std::string& sequence, const Comments& comments, const Alphabet* alpha)
  throw (BadCharException) :
	EdSymbolList(alpha),
	name_(name),
	comments_(comments)
{
  if (sequence != "")
    setContent(sequence);
}

SequenceWithAnnotation::SequenceWithAnnotation(const std::string& name, const std::vector<std::string>& sequence, const Alphabet* alpha)
throw (BadCharException) :
	EdSymbolList(sequence, alpha),
	name_(name),
  comments_()
{}

SequenceWithAnnotation::SequenceWithAnnotation(const std::string& name, const std::vector<std::string>& sequence, const Comments& comments, const Alphabet* alpha)
  throw (BadCharException) :
	EdSymbolList(sequence, alpha),
	name_(name),
	comments_(comments)
{}

SequenceWithAnnotation::SequenceWithAnnotation(const std::string& name, const std::vector<int>& sequence, const Alphabet* alpha)
  throw (BadIntException) :
	EdSymbolList(sequence, alpha),
	name_(name),
  comments_()
{}

SequenceWithAnnotation::SequenceWithAnnotation(const std::string& name, const std::vector<int>& sequence, const Comments& comments, const Alphabet* alpha)
  throw (BadIntException) :
	EdSymbolList(sequence, alpha),
	name_(name),
	comments_(comments)
{}

/* Copy constructors: *********************************************************/

SequenceWithAnnotation::SequenceWithAnnotation(const Sequence& s) :
	EdSymbolList(s),
	name_(s.getName()),
	comments_(s.getComments())
{}

SequenceWithAnnotation::SequenceWithAnnotation(const SequenceWithAnnotation& s) :
	EdSymbolList(s),
	name_(s.getName()),
	comments_(s.getComments())
{}

/* Assignation operator: ******************************************************/

SequenceWithAnnotation& SequenceWithAnnotation::operator=(const Sequence& s)
{
  EdSymbolList::operator=(s);
	name_     = s.getName();
	comments_ = s.getComments();
  return *this;
}

SequenceWithAnnotation& SequenceWithAnnotation::operator=(const SequenceWithAnnotation& s)
{
  EdSymbolList::operator=(s);
	name_     = s.getName();
	comments_ = s.getComments();
	return *this;
}

/******************************************************************************/

void SequenceWithAnnotation::setContent(const std::string& sequence) throw (BadCharException)
{
  SymbolListEditionEvent event(this);
  fireBeforeSequenceChanged(event);
	// Remove blanks in sequence
	content_ = StringSequenceTools::codeSequence(TextTools::removeWhiteSpaces(sequence), getAlphabet());
  //Warning, an exception may be thrown here!
  fireAfterSequenceChanged(event);
}

/******************************************************************************/

void SequenceWithAnnotation::setToSizeR(unsigned int newSize)
{
	// Size verification
	unsigned int seqSize = content_.size();
	if (newSize == seqSize) return;

	if (newSize < seqSize)
  {
    SymbolListDeletionEvent event(this, newSize, seqSize - newSize);
    fireBeforeSequenceDeleted(event);
		content_.resize(newSize);
    fireAfterSequenceDeleted(event);
		return;
	}

	// Add gaps up to specified size
  SymbolListInsertionEvent event(this, seqSize, newSize - seqSize);
  fireBeforeSequenceInserted(event);
  int gap = getAlphabet()->getGapCharacterCode();
	while (content_.size() < newSize) content_.push_back(gap);
  fireAfterSequenceInserted(event);
}

/******************************************************************************/

void SequenceWithAnnotation::setToSizeL(unsigned int newSize)
{
	// Size verification
	unsigned int seqSize = content_.size();
	if (newSize == seqSize) return;

	if (newSize < seqSize)
  {
		//We must truncate sequence from the left.
    SymbolListDeletionEvent event(this, 0, seqSize - newSize);
    fireBeforeSequenceDeleted(event);
		content_.erase(content_.begin(), content_.begin() + (seqSize - newSize));
    fireAfterSequenceDeleted(event);
		return;
	}

	// Add gaps up to specified size
  SymbolListInsertionEvent event(this, 0, newSize - seqSize);
  fireBeforeSequenceInserted(event);
  int gap = getAlphabet()->getGapCharacterCode();
	content_.insert(content_.begin(), newSize - seqSize, gap);
  fireAfterSequenceInserted(event);
}

/******************************************************************************/

void SequenceWithAnnotation::append(const std::vector<int>& content) throw (BadIntException)
{
  SymbolListInsertionEvent event(this, content_.size(), content.size());
  fireBeforeSequenceInserted(event);
	// Check list for incorrect characters
	for (unsigned int i = 0; i < content.size(); i++)
		if(!getAlphabet()->isIntInAlphabet(content[i]))
      throw BadIntException(content[i], "SequenceWithAnnotation::append", getAlphabet());
	//SequenceWithAnnotation is valid:
	for (unsigned int i = 0; i < content.size(); i++)
		content_.push_back(content[i]);
  
  fireAfterSequenceInserted(event);
}

void SequenceWithAnnotation::append(const std::vector<std::string>& content) throw (BadCharException)
{
  SymbolListInsertionEvent event(this, content_.size(), content.size());
  fireBeforeSequenceInserted(event);
	// Check list for incorrect characters
	for (unsigned int i = 0; i < content.size(); i++)
		if(!getAlphabet()->isCharInAlphabet(content[i]))
      throw BadCharException(content[i], "SequenceWithAnnotation::append", getAlphabet());
	
	//SequenceWithAnnotation is valid:
	for (unsigned int i = 0; i < content.size(); i++)
		content_.push_back(getAlphabet()->charToInt(content[i]));
  
  fireAfterSequenceInserted(event);
}

void SequenceWithAnnotation::append(const std::string& content) throw (BadCharException)
{
	append(StringSequenceTools::codeSequence(content, getAlphabet()));
}

/******************************************************************************/

#include "stdafx.h"
#include <ctype.h>

#include "tinyxml.h"


bool TiXmlBase::condenseWhiteSpace = true;

void TiXmlBase::PutString(const xr_string& str, xr_string* outString)
{
	int i = 0;

	while (i < (int)str.length())
	{
		unsigned char c = (unsigned char)str[i];

		if (c == '&'
			&& i < ((int)str.length() - 2)
			&& str[i + 1] == '#'
			&& str[i + 2] == 'x')
		{
			// Hexadecimal character reference.
			// Pass through unchanged.
			// &#xA9;	-- copyright symbol, for example.
			//
			// The -1 is a bug fix from Rob Laveaux. It keeps
			// an overflow from happening if there is no ';'.
			// There are actually 2 ways to exit this loop -
			// while fails (error case) and break (semicolon found).
			// However, there is no mechanism (currently) for
			// this function to return an error.
			while (i < (int)str.length() - 1)
			{
				outString->append(str.c_str() + i, 1);
				++i;
				if (str[i] == ';')
					break;
			}
		}
		else if (c == '&')
		{
			outString->append(entity[0].str, entity[0].strLength);
			++i;
		}
		else if (c == '<')
		{
			outString->append(entity[1].str, entity[1].strLength);
			++i;
		}
		else if (c == '>')
		{
			outString->append(entity[2].str, entity[2].strLength);
			++i;
		}
		else if (c == '\"')
		{
			outString->append(entity[3].str, entity[3].strLength);
			++i;
		}
		else if (c == '\'')
		{
			outString->append(entity[4].str, entity[4].strLength);
			++i;
		}
		else if (c < 32)
		{
			// Easy pass at non-alpha/numeric/symbol
			// Below 32 is symbolic.
			char buf[32];

			_snprintf_s(buf, sizeof(buf), "&#x%02X;", (unsigned)(c & 0xff));

			//*ME:	warning C4267: convert 'size_t' to 'int'
			//*ME:	Int-Cast to make compiler happy ...
			outString->append(buf, (int)xr_strlen(buf));
			++i;
		}
		else
		{
			*outString += (char)c;	// somewhat more efficient function call.
			++i;
		}
	}
}


TiXmlNode::TiXmlNode(NodeType _type) : TiXmlBase()
{
	parent = 0;
	type = _type;
	firstChild = 0;
	lastChild = 0;
	prev = 0;
	next = 0;
}


TiXmlNode::~TiXmlNode()
{
	TiXmlNode* node = firstChild;
	TiXmlNode* temp = 0;

	while (node)
	{
		temp = node;
		node = node->next;
		xr_delete(temp);
	}
}


void TiXmlNode::CopyTo(TiXmlNode* target) const
{
	target->SetValue(value.c_str());
	target->userData = userData;
}


void TiXmlNode::Clear()
{
	TiXmlNode* node = firstChild;
	TiXmlNode* temp = 0;

	while (node)
	{
		temp = node;
		node = node->next;
		xr_delete(temp);
	}

	firstChild = 0;
	lastChild = 0;
}


TiXmlNode* TiXmlNode::LinkEndChild(TiXmlNode* node)
{
	assert(node->parent == 0 || node->parent == this);
	assert(node->GetDocument() == 0 || node->GetDocument() == this->GetDocument());

	if (node->Type() == TiXmlNode::DOCUMENT)
	{
		xr_delete(node);
		if (GetDocument()) GetDocument()->SetError(TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN);
		return 0;
	}

	node->parent = this;

	node->prev = lastChild;
	node->next = 0;

	if (lastChild)
		lastChild->next = node;
	else
		firstChild = node;			// it was an empty list.

	lastChild = node;
	return node;
}


TiXmlNode* TiXmlNode::InsertEndChild(const TiXmlNode& addThis)
{
	if (addThis.Type() == TiXmlNode::DOCUMENT)
	{
		if (GetDocument()) GetDocument()->SetError(TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN);
		return 0;
	}
	TiXmlNode* node = addThis.Clone();
	if (!node)
		return 0;

	return LinkEndChild(node);
}


TiXmlNode* TiXmlNode::InsertBeforeChild(TiXmlNode* beforeThis, const TiXmlNode& addThis)
{
	if (!beforeThis || beforeThis->parent != this)
	{
		return 0;
	}
	if (addThis.Type() == TiXmlNode::DOCUMENT)
	{
		if (GetDocument()) GetDocument()->SetError(TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN);
		return 0;
	}

	TiXmlNode* node = addThis.Clone();
	if (!node)
		return 0;
	node->parent = this;

	node->next = beforeThis;
	node->prev = beforeThis->prev;
	if (beforeThis->prev)
	{
		beforeThis->prev->next = node;
	}
	else
	{
		assert(firstChild == beforeThis);
		firstChild = node;
	}
	beforeThis->prev = node;
	return node;
}


TiXmlNode* TiXmlNode::InsertAfterChild(TiXmlNode* afterThis, const TiXmlNode& addThis)
{
	if (!afterThis || afterThis->parent != this)
	{
		return 0;
	}
	if (addThis.Type() == TiXmlNode::DOCUMENT)
	{
		if (GetDocument()) GetDocument()->SetError(TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN);
		return 0;
	}

	TiXmlNode* node = addThis.Clone();
	if (!node)
		return 0;
	node->parent = this;

	node->prev = afterThis;
	node->next = afterThis->next;
	if (afterThis->next)
	{
		afterThis->next->prev = node;
	}
	else
	{
		assert(lastChild == afterThis);
		lastChild = node;
	}
	afterThis->next = node;
	return node;
}


TiXmlNode* TiXmlNode::ReplaceChild(TiXmlNode* replaceThis, const TiXmlNode& withThis)
{
	if (replaceThis->parent != this)
		return 0;

	TiXmlNode* node = withThis.Clone();
	if (!node)
		return 0;

	node->next = replaceThis->next;
	node->prev = replaceThis->prev;

	if (replaceThis->next)
		replaceThis->next->prev = node;
	else
		lastChild = node;

	if (replaceThis->prev)
		replaceThis->prev->next = node;
	else
		firstChild = node;

	xr_delete(replaceThis);
	node->parent = this;
	return node;
}


bool TiXmlNode::RemoveChild(TiXmlNode* removeThis)
{
	if (removeThis->parent != this)
	{
		assert(0);
		return false;
	}

	if (removeThis->next)
		removeThis->next->prev = removeThis->prev;
	else
		lastChild = removeThis->prev;

	if (removeThis->prev)
		removeThis->prev->next = removeThis->next;
	else
		firstChild = removeThis->next;

	xr_delete(removeThis);
	return true;
}

const TiXmlNode* TiXmlNode::FirstChild(pcstr _value) const
{
	const TiXmlNode* node;
	for (node = firstChild; node; node = node->next)
	{
		if (xr_strcmp(node->Value(), _value) == 0)
			return node;
	}
	return 0;
}


const TiXmlNode* TiXmlNode::LastChild(pcstr _value) const
{
	const TiXmlNode* node;
	for (node = lastChild; node; node = node->prev)
	{
		if (xr_strcmp(node->Value(), _value) == 0)
			return node;
	}
	return 0;
}


const TiXmlNode* TiXmlNode::IterateChildren(const TiXmlNode* previous) const
{
	if (!previous)
	{
		return FirstChild();
	}
	else
	{
		assert(previous->parent == this);
		return previous->NextSibling();
	}
}


const TiXmlNode* TiXmlNode::IterateChildren(pcstr val, const TiXmlNode* previous) const
{
	if (!previous)
	{
		return FirstChild(val);
	}
	else
	{
		assert(previous->parent == this);
		return previous->NextSibling(val);
	}
}


const TiXmlNode* TiXmlNode::NextSibling(pcstr _value) const
{
	const TiXmlNode* node;
	for (node = next; node; node = node->next)
	{
		if (xr_strcmp(node->Value(), _value) == 0)
			return node;
	}
	return 0;
}


const TiXmlNode* TiXmlNode::PreviousSibling(pcstr _value) const
{
	const TiXmlNode* node;
	for (node = prev; node; node = node->prev)
	{
		if (xr_strcmp(node->Value(), _value) == 0)
			return node;
	}
	return 0;
}


void TiXmlElement::RemoveAttribute(pcstr name)
{
	xr_string str(name);
	TiXmlAttribute* node = attributeSet.Find(str);

	if (node)
	{
		attributeSet.Remove(node);
		xr_delete(node);
	}
}

const TiXmlElement* TiXmlNode::FirstChildElement() const
{
	const TiXmlNode* node;

	for (node = FirstChild();
		 node;
		 node = node->NextSibling())
	{
		if (node->ToElement())
			return node->ToElement();
	}
	return 0;
}


const TiXmlElement* TiXmlNode::FirstChildElement(pcstr _value) const
{
	const TiXmlNode* node;

	for (node = FirstChild(_value);
		 node;
		 node = node->NextSibling(_value))
	{
		if (node->ToElement())
			return node->ToElement();
	}
	return 0;
}


const TiXmlElement* TiXmlNode::NextSiblingElement() const
{
	const TiXmlNode* node;

	for (node = NextSibling();
		 node;
		 node = node->NextSibling())
	{
		if (node->ToElement())
			return node->ToElement();
	}
	return 0;
}


const TiXmlElement* TiXmlNode::NextSiblingElement(pcstr _value) const
{
	const TiXmlNode* node;

	for (node = NextSibling(_value);
		 node;
		 node = node->NextSibling(_value))
	{
		if (node->ToElement())
			return node->ToElement();
	}
	return 0;
}


const TiXmlDocument* TiXmlNode::GetDocument() const
{
	const TiXmlNode* node;

	for (node = this; node; node = node->parent)
	{
		if (node->ToDocument())
			return node->ToDocument();
	}
	return 0;
}


TiXmlElement::TiXmlElement(pcstr _value)
	: TiXmlNode(TiXmlNode::ELEMENT)
{
	firstChild = lastChild = 0;
	value = _value;
}

TiXmlElement::TiXmlElement(const xr_string& _value)
	: TiXmlNode(TiXmlNode::ELEMENT)
{
	firstChild = lastChild = 0;
	value = _value;
}

TiXmlElement::TiXmlElement(const TiXmlElement& copy)
	: TiXmlNode(TiXmlNode::ELEMENT)
{
	firstChild = lastChild = 0;
	copy.CopyTo(this);
}


void TiXmlElement::operator=(const TiXmlElement& base)
{
	ClearThis();
	base.CopyTo(this);
}


TiXmlElement::~TiXmlElement()
{
	ClearThis();
}


void TiXmlElement::ClearThis()
{
	Clear();
	while (attributeSet.First())
	{
		TiXmlAttribute* node = attributeSet.First();
		attributeSet.Remove(node);
		xr_delete(node);
	}
}


pcstr TiXmlElement::Attribute(pcstr name) const
{
	const TiXmlAttribute* node = attributeSet.Find(name);
	if (node)
		return node->Value();
	return 0;
}

const xr_string* TiXmlElement::Attribute(const xr_string& name) const
{
	const TiXmlAttribute* node = attributeSet.Find(name);
	if (node)
		return &node->ValueStr();
	return 0;
}

pcstr TiXmlElement::Attribute(pcstr name, int* i) const
{
	pcstr s = Attribute(name);
	if (i)
	{
		if (s)
		{
			*i = atoi(s);
		}
		else
		{
			*i = 0;
		}
	}
	return s;
}

const xr_string* TiXmlElement::Attribute(const xr_string& name, int* i) const
{
	const xr_string* s = Attribute(name);
	if (i)
	{
		if (s)
		{
			*i = atoi(s->c_str());
		}
		else
		{
			*i = 0;
		}
	}
	return s;
}

pcstr TiXmlElement::Attribute(pcstr name, double* d) const
{
	pcstr s = Attribute(name);
	if (d)
	{
		if (s)
		{
			*d = atof(s);
		}
		else
		{
			*d = 0;
		}
	}
	return s;
}

const xr_string* TiXmlElement::Attribute(const xr_string& name, double* d) const
{
	const xr_string* s = Attribute(name);
	if (d)
	{
		if (s)
		{
			*d = atof(s->c_str());
		}
		else
		{
			*d = 0;
		}
	}
	return s;
}

int TiXmlElement::QueryIntAttribute(pcstr name, int* ival) const
{
	const TiXmlAttribute* node = attributeSet.Find(name);
	if (!node)
		return TIXML_NO_ATTRIBUTE;
	return node->QueryIntValue(ival);
}

int TiXmlElement::QueryIntAttribute(const xr_string& name, int* ival) const
{
	const TiXmlAttribute* node = attributeSet.Find(name);
	if (!node)
		return TIXML_NO_ATTRIBUTE;
	return node->QueryIntValue(ival);
}

int TiXmlElement::QueryDoubleAttribute(pcstr name, double* dval) const
{
	const TiXmlAttribute* node = attributeSet.Find(name);
	if (!node)
		return TIXML_NO_ATTRIBUTE;
	return node->QueryDoubleValue(dval);
}

int TiXmlElement::QueryDoubleAttribute(const xr_string& name, double* dval) const
{
	const TiXmlAttribute* node = attributeSet.Find(name);
	if (!node)
		return TIXML_NO_ATTRIBUTE;
	return node->QueryDoubleValue(dval);
}

void TiXmlElement::SetAttribute(pcstr name, int val)
{
	char buf[64];

	_snprintf_s(buf, sizeof(buf), "%d", val);

	SetAttribute(name, buf);
}

void TiXmlElement::SetAttribute(const xr_string& name, int val)
{
	char buf[64];

	_snprintf_s(buf, sizeof(buf), "%d", val);

	SetAttribute(name, buf);
}

void TiXmlElement::SetDoubleAttribute(pcstr name, double val)
{
	char buf[256];

	_snprintf_s(buf, sizeof(buf), "%f", val);

	SetAttribute(name, buf);
}


void TiXmlElement::SetAttribute(pcstr cname, pcstr cvalue)
{
	xr_string _name(cname);
	xr_string _value(cvalue);

	TiXmlAttribute* node = attributeSet.Find(_name);
	if (node)
	{
		node->SetValue(_value);
		return;
	}

	TiXmlAttribute* attrib = xr_new<TiXmlAttribute>(cname, cvalue);
	if (attrib)
	{
		attributeSet.Add(attrib);
	}
	else
	{
		TiXmlDocument* document = GetDocument();
		if (document) document->SetError(TIXML_ERROR_OUT_OF_MEMORY, 0, 0, TIXML_ENCODING_UNKNOWN);
	}
}

void TiXmlElement::SetAttribute(const xr_string& name, const xr_string& _value)
{
	TiXmlAttribute* node = attributeSet.Find(name);
	if (node)
	{
		node->SetValue(_value);
		return;
	}

	TiXmlAttribute* attrib = xr_new<TiXmlAttribute>(name, _value);
	if (attrib)
	{
		attributeSet.Add(attrib);
	}
	else
	{
		TiXmlDocument* document = GetDocument();
		if (document) document->SetError(TIXML_ERROR_OUT_OF_MEMORY, 0, 0, TIXML_ENCODING_UNKNOWN);
	}
}

void TiXmlElement::Print(FILE* cfile, int depth) const
{
	int i;
	assert(cfile);
	for (i = 0; i < depth; i++)
	{
		fprintf(cfile, "    ");
	}

	fprintf(cfile, "<%s", value.c_str());

	const TiXmlAttribute* attrib;
	for (attrib = attributeSet.First(); attrib; attrib = attrib->Next())
	{
		fprintf(cfile, " ");
		attrib->Print(cfile, depth);
	}

	// There are 3 different formatting approaches:
	// 1) An element without children is printed as a <foo /> node
	// 2) An element with only a text child is printed as <foo> text </foo>
	// 3) An element with children is printed on multiple lines.
	TiXmlNode* node;
	if (!firstChild)
	{
		fprintf(cfile, " />");
	}
	else if (firstChild == lastChild && firstChild->ToText())
	{
		fprintf(cfile, ">");
		firstChild->Print(cfile, depth + 1);
		fprintf(cfile, "</%s>", value.c_str());
	}
	else
	{
		fprintf(cfile, ">");

		for (node = firstChild; node; node = node->NextSibling())
		{
			if (!node->ToText())
			{
				fprintf(cfile, "\n");
			}
			node->Print(cfile, depth + 1);
		}
		fprintf(cfile, "\n");
		for (i = 0; i < depth; ++i)
		{
			fprintf(cfile, "    ");
		}
		fprintf(cfile, "</%s>", value.c_str());
	}
}


void TiXmlElement::CopyTo(TiXmlElement* target) const
{
	// superclass:
	TiXmlNode::CopyTo(target);

	// Element class: 
	// Clone the attributes, then clone the children.
	const TiXmlAttribute* attribute = 0;
	for (attribute = attributeSet.First();
		 attribute;
		 attribute = attribute->Next())
	{
		target->SetAttribute(attribute->Name(), attribute->Value());
	}

	TiXmlNode* node = 0;
	for (node = firstChild; node; node = node->NextSibling())
	{
		target->LinkEndChild(node->Clone());
	}
}

bool TiXmlElement::Accept(TiXmlVisitor* visitor) const
{
	if (visitor->VisitEnter(*this, attributeSet.First()))
	{
		for (const TiXmlNode* node = FirstChild(); node; node = node->NextSibling())
		{
			if (!node->Accept(visitor))
				break;
		}
	}
	return visitor->VisitExit(*this);
}


TiXmlNode* TiXmlElement::Clone() const
{
	TiXmlElement* clone = xr_new<TiXmlElement>(Value());
	if (!clone)
		return 0;

	CopyTo(clone);
	return clone;
}


pcstr TiXmlElement::GetText() const
{
	const TiXmlNode* child = this->FirstChild();
	if (child)
	{
		const TiXmlText* childText = child->ToText();
		if (childText)
		{
			return childText->Value();
		}
	}
	return 0;
}


TiXmlDocument::TiXmlDocument() : TiXmlNode(TiXmlNode::DOCUMENT)
{
	tabsize = 4;
	useMicrosoftBOM = false;
	ClearError();
}

TiXmlDocument::TiXmlDocument(pcstr documentName) : TiXmlNode(TiXmlNode::DOCUMENT)
{
	tabsize = 4;
	useMicrosoftBOM = false;
	value = documentName;
	ClearError();
}

TiXmlDocument::TiXmlDocument(const xr_string& documentName) : TiXmlNode(TiXmlNode::DOCUMENT)
{
	tabsize = 4;
	useMicrosoftBOM = false;
	value = documentName;
	ClearError();
}

TiXmlDocument::TiXmlDocument(const TiXmlDocument& copy) : TiXmlNode(TiXmlNode::DOCUMENT)
{
	copy.CopyTo(this);
}


void TiXmlDocument::operator=(const TiXmlDocument& copy)
{
	Clear();
	copy.CopyTo(this);
}


bool TiXmlDocument::LoadFile(TiXmlEncoding encoding)
{
	return LoadFile(Value(), encoding);
}


bool TiXmlDocument::SaveFile() const
{
	return SaveFile(Value());
}

bool TiXmlDocument::LoadFile(pcstr _filename, TiXmlEncoding encoding)
{
	// There was a really terrifying little bug here. The code:
	//		value = filename
	// in the STL case, cause the assignment method of the xr_string to
	// be called. What is strange, is that the xr_string had the same
	// address as it's c_str() method, and so bad things happen. Looks
	// like a bug in the Microsoft STL implementation.
	// Add an extra string to avoid the crash.
	xr_string filename(_filename);
	value = filename;

	// reading in binary mode so that tinyxml can normalize the EOL
	FILE* file = fopen(value.c_str(), "rb");

	if (file)
	{
		bool result = LoadFile(file, encoding);
		fclose(file);
		return result;
	}
	else
	{
		SetError(TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN);
		return false;
	}
}

bool TiXmlDocument::LoadFile(FILE* file, TiXmlEncoding encoding)
{
	if (!file)
	{
		SetError(TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN);
		return false;
	}

	// Delete the existing data:
	Clear();
	location.Clear();

	// Get the file size, so we can pre-allocate the string. HUGE speed impact.
	long length = 0;
	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Strange case, but good to handle up front.
	if (length == 0)
	{
		SetError(TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, TIXML_ENCODING_UNKNOWN);
		return false;
	}

	// If we have a file, assume it is all one big XML file, and read it in.
	// The document parser may decide the document ends sooner than the entire file, however.
	xr_string data;
	data.reserve(length);

	// Subtle bug here. TinyXml did use fgets. But from the XML spec:
	// 2.11 End-of-Line Handling
	// <snip>
	// <quote>
	// ...the XML processor MUST behave as if it normalized all line breaks in external 
	// parsed entities (including the document entity) on input, before parsing, by translating 
	// both the two-character sequence #xD #xA and any #xD that is not followed by #xA to 
	// a single #xA character.
	// </quote>
	//
	// It is not clear fgets does that, and certainly isn't clear it works cross platform. 
	// Generally, you expect fgets to translate from the convention of the OS to the c/unix
	// convention, and not work generally.

	/*
	while( fgets( buf, sizeof(buf), file ) )
	{
		data += buf;
	}
	*/

	char* buf = xr_alloc<char>(length + 1);
	buf[0] = 0;

	if (fread(buf, length, 1, file) != 1)
	{
		xr_free(buf);
		SetError(TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN);
		return false;
	}

	pcstr lastPos = buf;
	pcstr p = buf;

	buf[length] = 0;
	while (*p)
	{
		assert(p < (buf + length));
		if (*p == 0xa)
		{
			// Newline character. No special rules for this. Append all the characters
			// since the last string, and include the newline.
			data.append(lastPos, (p - lastPos + 1));	// append, include the newline
			++p;									// move past the newline
			lastPos = p;							// and point to the new buffer (may be 0)
			assert(p <= (buf + length));
		}
		else if (*p == 0xd)
		{
			// Carriage return. Append what we have so far, then
			// handle moving forward in the buffer.
			if ((p - lastPos) > 0)
			{
				data.append(lastPos, p - lastPos);	// do not add the CR
			}
			data += (char)0xa;						// a proper newline

			if (*(p + 1) == 0xa)
			{
				// Carriage return - new line sequence
				p += 2;
				lastPos = p;
				assert(p <= (buf + length));
			}
			else
			{
				// it was followed by something else...that is presumably characters again.
				++p;
				lastPos = p;
				assert(p <= (buf + length));
			}
		}
		else
		{
			++p;
		}
	}
	// Handle any left over characters.
	if (p - lastPos)
	{
		data.append(lastPos, p - lastPos);
	}
	xr_free(buf);
	buf = 0;

	Parse(data.c_str(), 0, encoding);

	if (Error())
		return false;
	else
		return true;
}


bool TiXmlDocument::SaveFile(pcstr filename) const
{
	// The old c stuff lives on...
	FILE* fp = fopen(filename, "w");
	if (fp)
	{
		bool result = SaveFile(fp);
		fclose(fp);
		return result;
	}
	return false;
}


bool TiXmlDocument::SaveFile(FILE* fp) const
{
	if (useMicrosoftBOM)
	{
		const unsigned char TIXML_UTF_LEAD_0 = 0xefU;
		const unsigned char TIXML_UTF_LEAD_1 = 0xbbU;
		const unsigned char TIXML_UTF_LEAD_2 = 0xbfU;

		fputc(TIXML_UTF_LEAD_0, fp);
		fputc(TIXML_UTF_LEAD_1, fp);
		fputc(TIXML_UTF_LEAD_2, fp);
	}
	Print(fp, 0);
	return (ferror(fp) == 0);
}


void TiXmlDocument::CopyTo(TiXmlDocument* target) const
{
	TiXmlNode::CopyTo(target);

	target->error = error;
	target->errorDesc = errorDesc.c_str();

	TiXmlNode* node = 0;
	for (node = firstChild; node; node = node->NextSibling())
	{
		target->LinkEndChild(node->Clone());
	}
}


TiXmlNode* TiXmlDocument::Clone() const
{
	TiXmlDocument* clone = xr_new<TiXmlDocument>();
	if (!clone)
		return 0;

	CopyTo(clone);
	return clone;
}


void TiXmlDocument::Print(FILE* cfile, int depth) const
{
	assert(cfile);
	for (const TiXmlNode* node = FirstChild(); node; node = node->NextSibling())
	{
		node->Print(cfile, depth);
		fprintf(cfile, "\n");
	}
}


bool TiXmlDocument::Accept(TiXmlVisitor* visitor) const
{
	if (visitor->VisitEnter(*this))
	{
		for (const TiXmlNode* node = FirstChild(); node; node = node->NextSibling())
		{
			if (!node->Accept(visitor))
				break;
		}
	}
	return visitor->VisitExit(*this);
}


const TiXmlAttribute* TiXmlAttribute::Next() const
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if (next->value.empty() && next->name.empty())
		return 0;
	return next;
}

const TiXmlAttribute* TiXmlAttribute::Previous() const
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if (prev->value.empty() && prev->name.empty())
		return 0;
	return prev;
}

void TiXmlAttribute::Print(FILE* cfile, int /*depth*/, xr_string* str) const
{
	xr_string n;
	xr_string v;

	PutString(name, &n);
	PutString(value, &v);

	if (value.find('\"') == xr_string::npos)
	{
		if (cfile)
		{
			fprintf(cfile, "%s=\"%s\"", n.c_str(), v.c_str());
		}
		if (str)
		{
			(*str) += n; (*str) += "=\""; (*str) += v; (*str) += "\"";
		}
	}
	else
	{
		if (cfile)
		{
			fprintf(cfile, "%s='%s'", n.c_str(), v.c_str());
		}
		if (str)
		{
			(*str) += n; (*str) += "='"; (*str) += v; (*str) += "'";
		}
	}
}


int TiXmlAttribute::QueryIntValue(int* ival) const
{
	if (sscanf(value.c_str(), "%d", ival) == 1)
		return TIXML_SUCCESS;
	return TIXML_WRONG_TYPE;
}

int TiXmlAttribute::QueryDoubleValue(double* dval) const
{
	if (sscanf(value.c_str(), "%lf", dval) == 1)
		return TIXML_SUCCESS;
	return TIXML_WRONG_TYPE;
}

void TiXmlAttribute::SetIntValue(int _value)
{
	char buf[64];

	_snprintf_s(buf, sizeof(buf), "%d", _value);

	SetValue(buf);
}

void TiXmlAttribute::SetDoubleValue(double _value)
{
	char buf[256];

	_snprintf_s(buf, sizeof(buf), "%lf", _value);

	SetValue(buf);
}

int TiXmlAttribute::IntValue() const
{
	return atoi(value.c_str());
}

double  TiXmlAttribute::DoubleValue() const
{
	return atof(value.c_str());
}


TiXmlComment::TiXmlComment(const TiXmlComment& copy) : TiXmlNode(TiXmlNode::COMMENT)
{
	copy.CopyTo(this);
}


void TiXmlComment::operator=(const TiXmlComment& base)
{
	Clear();
	base.CopyTo(this);
}


void TiXmlComment::Print(FILE* cfile, int depth) const
{
	assert(cfile);
	for (int i = 0; i < depth; i++)
	{
		fprintf(cfile, "    ");
	}
	fprintf(cfile, "<!--%s-->", value.c_str());
}


void TiXmlComment::CopyTo(TiXmlComment* target) const
{
	TiXmlNode::CopyTo(target);
}


bool TiXmlComment::Accept(TiXmlVisitor* visitor) const
{
	return visitor->Visit(*this);
}


TiXmlNode* TiXmlComment::Clone() const
{
	TiXmlComment* clone = xr_new<TiXmlComment>();

	if (!clone)
		return 0;

	CopyTo(clone);
	return clone;
}


void TiXmlText::Print(FILE* cfile, int depth) const
{
	assert(cfile);
	if (cdata)
	{
		int i;
		fprintf(cfile, "\n");
		for (i = 0; i < depth; i++)
		{
			fprintf(cfile, "    ");
		}
		fprintf(cfile, "<![CDATA[%s]]>\n", value.c_str());	// unformatted output
	}
	else
	{
		xr_string buffer;
		PutString(value, &buffer);
		fprintf(cfile, "%s", buffer.c_str());
	}
}


void TiXmlText::CopyTo(TiXmlText* target) const
{
	TiXmlNode::CopyTo(target);
	target->cdata = cdata;
}


bool TiXmlText::Accept(TiXmlVisitor* visitor) const
{
	return visitor->Visit(*this);
}


TiXmlNode* TiXmlText::Clone() const
{
	TiXmlText* clone = 0;
	clone = xr_new<TiXmlText>("");

	if (!clone)
		return 0;

	CopyTo(clone);
	return clone;
}


TiXmlDeclaration::TiXmlDeclaration(pcstr _version,
								   pcstr _encoding,
								   pcstr _standalone)
	: TiXmlNode(TiXmlNode::DECLARATION)
{
	version = _version;
	encoding = _encoding;
	standalone = _standalone;
}

TiXmlDeclaration::TiXmlDeclaration(const xr_string& _version,
								   const xr_string& _encoding,
								   const xr_string& _standalone)
	: TiXmlNode(TiXmlNode::DECLARATION)
{
	version = _version;
	encoding = _encoding;
	standalone = _standalone;
}

TiXmlDeclaration::TiXmlDeclaration(const TiXmlDeclaration& copy)
	: TiXmlNode(TiXmlNode::DECLARATION)
{
	copy.CopyTo(this);
}


void TiXmlDeclaration::operator=(const TiXmlDeclaration& copy)
{
	Clear();
	copy.CopyTo(this);
}


void TiXmlDeclaration::Print(FILE* cfile, int /*depth*/, xr_string* str) const
{
	if (cfile) fprintf(cfile, "<?xml ");
	if (str)	 (*str) += "<?xml ";

	if (!version.empty())
	{
		if (cfile) fprintf(cfile, "version=\"%s\" ", version.c_str());
		if (str)
		{
			(*str) += "version=\""; (*str) += version; (*str) += "\" ";
		}
	}
	if (!encoding.empty())
	{
		if (cfile) fprintf(cfile, "encoding=\"%s\" ", encoding.c_str());
		if (str)
		{
			(*str) += "encoding=\""; (*str) += encoding; (*str) += "\" ";
		}
	}
	if (!standalone.empty())
	{
		if (cfile) fprintf(cfile, "standalone=\"%s\" ", standalone.c_str());
		if (str)
		{
			(*str) += "standalone=\""; (*str) += standalone; (*str) += "\" ";
		}
	}
	if (cfile) fprintf(cfile, "?>");
	if (str)	 (*str) += "?>";
}


void TiXmlDeclaration::CopyTo(TiXmlDeclaration* target) const
{
	TiXmlNode::CopyTo(target);

	target->version = version;
	target->encoding = encoding;
	target->standalone = standalone;
}


bool TiXmlDeclaration::Accept(TiXmlVisitor* visitor) const
{
	return visitor->Visit(*this);
}


TiXmlNode* TiXmlDeclaration::Clone() const
{
	TiXmlDeclaration* clone = xr_new<TiXmlDeclaration>();

	if (!clone)
		return 0;

	CopyTo(clone);
	return clone;
}


void TiXmlUnknown::Print(FILE* cfile, int depth) const
{
	for (int i = 0; i < depth; i++)
		fprintf(cfile, "    ");
	fprintf(cfile, "<%s>", value.c_str());
}


void TiXmlUnknown::CopyTo(TiXmlUnknown* target) const
{
	TiXmlNode::CopyTo(target);
}


bool TiXmlUnknown::Accept(TiXmlVisitor* visitor) const
{
	return visitor->Visit(*this);
}


TiXmlNode* TiXmlUnknown::Clone() const
{
	TiXmlUnknown* clone = xr_new<TiXmlUnknown>();

	if (!clone)
		return 0;

	CopyTo(clone);
	return clone;
}


TiXmlAttributeSet::TiXmlAttributeSet()
{
	sentinel.next = &sentinel;
	sentinel.prev = &sentinel;
}


TiXmlAttributeSet::~TiXmlAttributeSet()
{
	assert(sentinel.next == &sentinel);
	assert(sentinel.prev == &sentinel);
}


void TiXmlAttributeSet::Add(TiXmlAttribute* addMe)
{
	assert(!Find(xr_string(addMe->Name())));	// Shouldn't be multiply adding to the set.

	addMe->next = &sentinel;
	addMe->prev = sentinel.prev;

	sentinel.prev->next = addMe;
	sentinel.prev = addMe;
}

void TiXmlAttributeSet::Remove(TiXmlAttribute* removeMe)
{
	TiXmlAttribute* node;

	for (node = sentinel.next; node != &sentinel; node = node->next)
	{
		if (node == removeMe)
		{
			node->prev->next = node->next;
			node->next->prev = node->prev;
			node->next = 0;
			node->prev = 0;
			return;
		}
	}
	assert(0);		// we tried to remove a non-linked attribute.
}

const TiXmlAttribute* TiXmlAttributeSet::Find(const xr_string& name) const
{
	for (const TiXmlAttribute* node = sentinel.next; node != &sentinel; node = node->next)
	{
		if (node->name == name)
			return node;
	}
	return 0;
}


const TiXmlAttribute* TiXmlAttributeSet::Find(pcstr name) const
{
	for (const TiXmlAttribute* node = sentinel.next; node != &sentinel; node = node->next)
	{
		if (xr_strcmp(node->name.c_str(), name) == 0)
			return node;
	}
	return 0;
}

TiXmlHandle TiXmlHandle::FirstChild() const
{
	if (node)
	{
		TiXmlNode* child = node->FirstChild();
		if (child)
			return TiXmlHandle(child);
	}
	return TiXmlHandle(0);
}


TiXmlHandle TiXmlHandle::FirstChild(pcstr value) const
{
	if (node)
	{
		TiXmlNode* child = node->FirstChild(value);
		if (child)
			return TiXmlHandle(child);
	}
	return TiXmlHandle(0);
}


TiXmlHandle TiXmlHandle::FirstChildElement() const
{
	if (node)
	{
		TiXmlElement* child = node->FirstChildElement();
		if (child)
			return TiXmlHandle(child);
	}
	return TiXmlHandle(0);
}


TiXmlHandle TiXmlHandle::FirstChildElement(pcstr value) const
{
	if (node)
	{
		TiXmlElement* child = node->FirstChildElement(value);
		if (child)
			return TiXmlHandle(child);
	}
	return TiXmlHandle(0);
}


TiXmlHandle TiXmlHandle::Child(int count) const
{
	if (node)
	{
		int i;
		TiXmlNode* child = node->FirstChild();
		for (i = 0;
			 child && i < count;
			 child = child->NextSibling(), ++i)
		{
			// nothing
		}
		if (child)
			return TiXmlHandle(child);
	}
	return TiXmlHandle(0);
}


TiXmlHandle TiXmlHandle::Child(pcstr value, int count) const
{
	if (node)
	{
		int i;
		TiXmlNode* child = node->FirstChild(value);
		for (i = 0;
			 child && i < count;
			 child = child->NextSibling(value), ++i)
		{
			// nothing
		}
		if (child)
			return TiXmlHandle(child);
	}
	return TiXmlHandle(0);
}


TiXmlHandle TiXmlHandle::ChildElement(int count) const
{
	if (node)
	{
		int i;
		TiXmlElement* child = node->FirstChildElement();
		for (i = 0;
			 child && i < count;
			 child = child->NextSiblingElement(), ++i)
		{
			// nothing
		}
		if (child)
			return TiXmlHandle(child);
	}
	return TiXmlHandle(0);
}


TiXmlHandle TiXmlHandle::ChildElement(pcstr value, int count) const
{
	if (node)
	{
		int i;
		TiXmlElement* child = node->FirstChildElement(value);
		for (i = 0;
			 child && i < count;
			 child = child->NextSiblingElement(value), ++i)
		{
			// nothing
		}
		if (child)
			return TiXmlHandle(child);
	}
	return TiXmlHandle(0);
}


bool TiXmlPrinter::VisitEnter(const TiXmlDocument&)
{
	return true;
}

bool TiXmlPrinter::VisitExit(const TiXmlDocument&)
{
	return true;
}

bool TiXmlPrinter::VisitEnter(const TiXmlElement& element, const TiXmlAttribute* firstAttribute)
{
	DoIndent();
	buffer += "<";
	buffer += element.Value();

	for (const TiXmlAttribute* attrib = firstAttribute; attrib; attrib = attrib->Next())
	{
		buffer += " ";
		attrib->Print(0, 0, &buffer);
	}

	if (!element.FirstChild())
	{
		buffer += " />";
		DoLineBreak();
	}
	else
	{
		buffer += ">";
		if (element.FirstChild()->ToText()
			&& element.LastChild() == element.FirstChild()
			&& element.FirstChild()->ToText()->CDATA() == false)
		{
			simpleTextPrint = true;
			// no DoLineBreak()!
		}
		else
		{
			DoLineBreak();
		}
	}
	++depth;
	return true;
}


bool TiXmlPrinter::VisitExit(const TiXmlElement& element)
{
	--depth;
	if (!element.FirstChild())
	{
		// nothing.
	}
	else
	{
		if (simpleTextPrint)
		{
			simpleTextPrint = false;
		}
		else
		{
			DoIndent();
		}
		buffer += "</";
		buffer += element.Value();
		buffer += ">";
		DoLineBreak();
	}
	return true;
}


bool TiXmlPrinter::Visit(const TiXmlText& text)
{
	if (text.CDATA())
	{
		DoIndent();
		buffer += "<![CDATA[";
		buffer += text.Value();
		buffer += "]]>";
		DoLineBreak();
	}
	else if (simpleTextPrint)
	{
		buffer += text.Value();
	}
	else
	{
		DoIndent();
		buffer += text.Value();
		DoLineBreak();
	}
	return true;
}


bool TiXmlPrinter::Visit(const TiXmlDeclaration& declaration)
{
	DoIndent();
	declaration.Print(0, 0, &buffer);
	DoLineBreak();
	return true;
}


bool TiXmlPrinter::Visit(const TiXmlComment& comment)
{
	DoIndent();
	buffer += "<!--";
	buffer += comment.Value();
	buffer += "-->";
	DoLineBreak();
	return true;
}


bool TiXmlPrinter::Visit(const TiXmlUnknown& unknown)
{
	DoIndent();
	buffer += "<";
	buffer += unknown.Value();
	buffer += ">";
	DoLineBreak();
	return true;
}

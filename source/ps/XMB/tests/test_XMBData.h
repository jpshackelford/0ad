/* Copyright (C) 2021 Wildfire Games.
 * This file is part of 0 A.D.
 *
 * 0 A.D. is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * 0 A.D. is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 0 A.D.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "lib/self_test.h"

#include "ps/XML/Xeromyces.h"
#include "ps/XMB/XMBStorage.h"
#include "scriptinterface/ScriptInterface.h"

#include <libxml/parser.h>
#include <memory>

class TestXMBData : public CxxTest::TestSuite
{
private:
	std::shared_ptr<u8> m_Buffer;

	std::unique_ptr<ScriptInterface> m_ScriptInterface;

	CXeromyces parseXML(const char* doc)
	{
		xmlDocPtr xmlDoc = xmlReadMemory(doc, int(strlen(doc)), "", NULL,
			XML_PARSE_NONET|XML_PARSE_NOCDATA);
		CXeromyces xmb;
		bool ok = xmb.m_Data.LoadXMLDoc(xmlDoc);
		xmlFreeDoc(xmlDoc);
		TS_ASSERT_EQUALS(ok, true);

		TS_ASSERT(xmb.Initialise(xmb.m_Data));
		return xmb;
	}

	CXeromyces parseJS(const std::string rootName, const char* code)
	{
		ScriptRequest rq(*m_ScriptInterface);
		JS::RootedValue val(rq.cx);
		m_ScriptInterface->Eval(code, &val);
		CXeromyces xmb;
		bool ok = xmb.m_Data.LoadJSValue(*m_ScriptInterface, val, rootName);
		TS_ASSERT_EQUALS(ok, true);

		TS_ASSERT(xmb.Initialise(xmb.m_Data));
		return xmb;
	}

	void setUp()
	{
		m_ScriptInterface = std::make_unique<ScriptInterface>("Test", "Test", g_ScriptContext);
	}

	void tearDown()
	{
		m_ScriptInterface.reset();
		m_Buffer.reset();
	}

public:
	void test_basic()
	{
		basic(parseXML("<test>\n<foo x=' y '> bar </foo><foo>\n\n\nbar</foo>\n</test>"), true);
		// Array format for same-named elements.
		basic(parseJS("test", "({ 'foo': [{ '@x': ' y ', '_string': 'bar' }, { '_string': '\\n\\n\\nbar' }] })"), false);
		// Alternative format for same-named elements.
		basic(parseJS("test", "({ 'foo@0@': { '@x': ' y ', '_string': 'bar' }, 'foo@1@': { '_string': '\\n\\n\\nbar' }})"), false);
	}

	void basic(const CXeromyces& xmb, bool checkLines)
	{

		TS_ASSERT_DIFFERS(xmb.GetElementID("test"), -1);
		TS_ASSERT_DIFFERS(xmb.GetElementID("foo"), -1);
		TS_ASSERT_EQUALS(xmb.GetElementID("bar"), -1);

		TS_ASSERT_DIFFERS(xmb.GetAttributeID("x"), -1);
		TS_ASSERT_EQUALS(xmb.GetAttributeID("y"), -1);
		TS_ASSERT_EQUALS(xmb.GetAttributeID("test"), -1);

		int el_test = xmb.GetElementID("test");
		int el_foo = xmb.GetElementID("foo");
		int at_x = xmb.GetAttributeID("x");

		XMBElement root = xmb.GetRoot();
		TS_ASSERT_EQUALS(root.GetNodeName(), el_test);
		if (checkLines)
			TS_ASSERT_EQUALS(root.GetLineNumber(), -1);
		TS_ASSERT_EQUALS(CStr(root.GetText()), "");

		TS_ASSERT_EQUALS(root.GetChildNodes().size(), 2);
		XMBElement child = root.GetChildNodes()[0];
		TS_ASSERT_EQUALS(child.GetNodeName(), el_foo);
		if (checkLines)
			TS_ASSERT_EQUALS(child.GetLineNumber(), 2);
		TS_ASSERT_EQUALS(child.GetChildNodes().size(), 0);
		TS_ASSERT_EQUALS(CStr(child.GetText()), "bar");

		if (checkLines)
			TS_ASSERT_EQUALS(root.GetChildNodes()[1].GetLineNumber(), 5);

		TS_ASSERT_EQUALS(child.GetAttributes().size(), 1);
		XMBAttribute attr = child.GetAttributes()[0];
		TS_ASSERT_EQUALS(attr.Name, at_x);
		TS_ASSERT_EQUALS(CStr(attr.Value), " y ");
	}

	void test_GetFirstNamedItem()
	{
		GetFirstNamedItem(parseXML("<test> <x>A</x> <x>B</x> <y>C</y> <z>D</z> </test>"), true);
		GetFirstNamedItem(parseJS("test", "({ 'x': [{ '_string': 'A' }, 'B'], 'y': 'C', 'z': 'D' })"), false);
		GetFirstNamedItem(parseJS("test", "({ 'x@0@': 'A', 'x@1@': 'B', 'y': 'C', 'z': 'D' })"), false);
	}

	void GetFirstNamedItem(const CXeromyces& xmb, bool checkLines)
	{
		XMBElement root = xmb.GetRoot();
		TS_ASSERT_EQUALS(root.GetChildNodes().size(), 4);

		XMBElement x = root.GetChildNodes().GetFirstNamedItem(xmb.GetElementID("x"));
		XMBElement y = root.GetChildNodes().GetFirstNamedItem(xmb.GetElementID("y"));
		XMBElement w = root.GetChildNodes().GetFirstNamedItem(xmb.GetElementID("w"));

		TS_ASSERT_EQUALS(x.GetNodeName(), xmb.GetElementID("x"));
		TS_ASSERT_EQUALS(CStr(x.GetText()), "A");

		TS_ASSERT_EQUALS(y.GetNodeName(), xmb.GetElementID("y"));
		TS_ASSERT_EQUALS(CStr(y.GetText()), "C");

		TS_ASSERT_EQUALS(w.GetNodeName(), -1);
		TS_ASSERT_EQUALS(CStr(w.GetText()), "");
		if (checkLines)
			TS_ASSERT_EQUALS(w.GetLineNumber(), -1);
		TS_ASSERT_EQUALS(w.GetChildNodes().size(), 0);
		TS_ASSERT_EQUALS(w.GetAttributes().size(), 0);
	}

	void test_doctype_ignored()
	{
		CXeromyces xmb (parseXML("<!DOCTYPE foo SYSTEM \"file:///dev/urandom\"><foo/>"));

		TS_ASSERT_DIFFERS(xmb.GetElementID("foo"), -1);
	}

	void test_complex_parse()
	{
		CXeromyces xmb (parseXML("<test>\t\n \tx &lt;>&amp;&quot;&apos;<![CDATA[foo]]>bar\n<x/>\nbaz<?cheese?>qux</test>"));
		TS_ASSERT_EQUALS(CStr(xmb.GetRoot().GetText()), "x <>&\"'foobar\n\nbazqux");
	}

	void test_unicode()
	{
		CXeromyces xmb (parseXML("<?xml version=\"1.0\" encoding=\"utf-8\"?><foo x='&#x1234;\xE1\x88\xB4'>&#x1234;\xE1\x88\xB4</foo>"));
		CStrW text;

		text = xmb.GetRoot().GetText().FromUTF8();
		TS_ASSERT_EQUALS((int)text.length(), 2);
		TS_ASSERT_EQUALS(text[0], 0x1234);
		TS_ASSERT_EQUALS(text[1], 0x1234);

		text = xmb.GetRoot().GetAttributes()[0].Value.FromUTF8();
		TS_ASSERT_EQUALS((int)text.length(), 2);
		TS_ASSERT_EQUALS(text[0], 0x1234);
		TS_ASSERT_EQUALS(text[1], 0x1234);
	}

	void test_iso88591()
	{
		CXeromyces xmb (parseXML("<?xml version=\"1.0\" encoding=\"iso-8859-1\"?><foo x='&#x1234;\xE1\x88\xB4'>&#x1234;\xE1\x88\xB4</foo>"));
		CStrW text;

		text = xmb.GetRoot().GetText().FromUTF8();
		TS_ASSERT_EQUALS((int)text.length(), 4);
		TS_ASSERT_EQUALS(text[0], 0x1234);
		TS_ASSERT_EQUALS(text[1], 0x00E1);
		TS_ASSERT_EQUALS(text[2], 0x0088);
		TS_ASSERT_EQUALS(text[3], 0x00B4);

		text = xmb.GetRoot().GetAttributes()[0].Value.FromUTF8();
		TS_ASSERT_EQUALS((int)text.length(), 4);
		TS_ASSERT_EQUALS(text[0], 0x1234);
		TS_ASSERT_EQUALS(text[1], 0x00E1);
		TS_ASSERT_EQUALS(text[2], 0x0088);
		TS_ASSERT_EQUALS(text[3], 0x00B4);
	}
};

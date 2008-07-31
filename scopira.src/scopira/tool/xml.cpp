
/*
 *  Copyright (c) 2003-2007    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/tool/xml.h>

#include <scopira/tool/prop.h>
#include <scopira/tool/util.h>

#include <vector>

//BBlibs libxml-2.0 scopira
//BBtargets libscopiraxml.so

using namespace scopira::tool;

//
//
// xml_doc
//
//

void xml_doc::clear(void)
{
  if (dm_doc) {
    xmlFreeDoc(dm_doc);
    dm_doc = 0;
  }
}

void xml_doc::init(const std::string &rootnodename)
{
  clear();
  dm_doc = xmlNewDoc(reinterpret_cast<const xmlChar*>("1.0"));
  assert(dm_doc);

  xmlNodePtr nd = xmlNewNode(0, reinterpret_cast<const xmlChar*>(rootnodename.c_str()));
  assert(nd);
  nd = xmlDocSetRootElement(dm_doc, nd);
  assert(!nd);
}

void xml_doc::add_pi(const std::string &name, const std::string &content)
{
  xmlNodePtr nd;
 
  nd = xmlNewDocPI(dm_doc, reinterpret_cast<const xmlChar*>(name.c_str()),
     reinterpret_cast<const xmlChar*>(content.c_str()));
  assert(nd);
  //nd = xmlAddChild(reinterpret_cast<xmlNodePtr>(dm_doc), nd);
  nd = xmlAddPrevSibling(xmlDocGetRootElement(dm_doc), nd);
  assert(nd);
}

void xml_doc::load_ptr(xmlDocPtr newdoc)
{
  clear();
  dm_doc = newdoc;
}

bool xml_doc::load_file(const std::string &filename)
{
  clear();
  dm_doc = xmlParseFile(filename.c_str());
 
  return dm_doc != 0;
}

bool xml_doc::load_string(const std::string &data)
{
  clear();
  dm_doc = xmlParseMemory(data.c_str(), data.size());
 
  return dm_doc != 0;
}

bool xml_doc::load_c_str(const char *str)
{
  clear();
  dm_doc = xmlParseDoc(reinterpret_cast<xmlChar *>(const_cast<char*>(str)));
 
  return dm_doc != 0;
}

static void load_prop_branch(xml_node nd, property *p)
{
  property::key_iterator ki;
  property::iterator ii;
  xml_node sub;

  assert(p);

  ki = p->get_key_iterator();
  for (; ki.valid(); ++ki)
    for (ii=p->get(*(*ki)); ii.valid(); ++ii) {
      if ((*ii)->is_node()) {
        //sub = nd.add_child("node");
        //sub.add_attrib("name", *(*ki));
        //load_prop_branch(sub, (*ii));
        // alternate-form
        sub = nd.add_child("properties");
        sub.add_attrib("name", *(*ki));
        load_prop_branch(sub, (*ii));
      } else {
        // add a property node for the string property
        //sub = nd.add_child("prop");
        //sub.add_attrib("name", *(*ki));
        //sub.add_content_child((*ii)->get_value_as_string());
        // alternate-form
        sub = nd.add_child(*(*ki));
        sub.add_content_child((*ii)->get_value_as_string());
      }
    }
}

void xml_doc::load_prop(property *p)
{
  xml_node nd;

  init("properties");
  nd = get_root();

  load_prop_branch(nd, p);
}

xml_node xml_doc::get_root(void) const
{
  return xml_node(xmlDocGetRootElement(dm_doc));
}

bool xml_doc::save_file(const std::string &filename, bool compress) const
{
  
  int r, m;
  
  m = xmlGetCompressMode();
  xmlSetCompressMode(compress ? 9 : 0);
  //r = xmlSaveFile(filename.c_str(), dm_doc);
  r = xmlSaveFormatFile(filename.c_str(), dm_doc, 1);
  xmlSetCompressMode(m);
  
  return r>0;
}

bool xml_doc::save_string(std::string &out) const
{

  int sz;
  xmlChar *mem;

  //r = xmlSaveFileTo(buf, dm_doc, 0);

  mem = 0;
  sz = 0;
  xmlDocDumpMemory(dm_doc, &mem, &sz);
  if (mem) {
    out.assign(reinterpret_cast<char*>(mem), sz);
    xmlFree(mem);
  }

  return mem != 0;
}

static void save_prop_branch(xml_node nd, property *p)
{
  xml_node c;
  xml_attrib at;
  property *subp;
  const char *cc;

  assert(p);

  for (c = nd.get_first_child_type(xml_node::element_c); c.is_valid(); c.next_type(xml_node::element_c)) {
    if (c.get_name() == "properties") {
      cc = 0;
      for (at = c.get_first_attrib(); at.is_valid(); at.next())
        if (at.get_name() == "name")
          cc = at.get_text_c_content();
      if (cc) {
        p->add_end(cc, subp = new property_node); // add a new sub property
        save_prop_branch(c, subp);
      }
    } else {
      cc = c.get_text_c_content();
      p->add_end(c.get_name(), new property(cc));
    }
  }
}

void xml_doc::save_prop(property *p) const
{
  assert(p);
  save_prop_branch(get_root(), p);
}

//
//
// xml_node
//
//

void xml_node::next_type(int typ)
{
  next();
  while (is_valid() && get_type() != typ)
    next();
}

void xml_node::next_name(const std::string &name)
{
  next_type(element_c);
  while (is_valid() && name != reinterpret_cast<const char *>(dm_node->name))
    next_type(element_c);
}

int xml_node::count_children(const std::string &name) const
{

  xml_node n = get_first_child_name(name);

  int r = 0;

  while (n.is_valid()) {
    r++;
    n.next_name(name);
  }

  return r;
}

xml_node xml_node::get_first_child_type(int typ) const
{
  xml_node ret = get_first_child();

  if (ret.is_valid() && ret.get_type() != typ)
    ret.next_type(typ);
  
  return ret;
}

xml_node xml_node::get_first_child_name(const std::string &name) const
{
  xml_node ret = get_first_child_type(element_c);

  while (ret.is_valid() && name != reinterpret_cast<const char *>(ret.dm_node->name))
    ret.next_type(element_c);

  return ret;
}

xml_attrib xml_node::get_first_attrib(void) const
{
  return xml_attrib(dm_node->properties);
}

const char * xml_node::get_attrib_c_text(const std::string &aname) const
{
  xml_attrib a(get_first_attrib());

  while (a.is_valid() && aname != a.get_c_name())
    a.next();
 
  if (a.is_valid()) {
    assert(a.get_text_c_content());
    return a.get_text_c_content();
  } else
    return ""; //not found
}

const char * xml_node::get_child_c_text(const std::string &nname) const
{
  xml_node subnode;

  subnode = get_first_child_name(nname);

  if (subnode.is_null())
    return "";
  return subnode.get_text_c_content();
}

bool xml_node::has_attrib(const std::string &aname) const
{
  for (xml_attrib a(get_first_attrib()); a.is_valid(); a.next())
    if (aname == a.get_c_name())
      return true;

  return false;
}

xml_node xml_node::add_child(const std::string &name)
{
  xmlNodePtr nd;
 
  nd = xmlNewNode(0, reinterpret_cast<const xmlChar*>(name.c_str()));
  assert(nd);
  nd = xmlAddChild(dm_node, nd);
  assert(nd);

  return xml_node(nd);
}

xml_node xml_node::add_comment(const std::string &comment_contents)
{
  xmlNodePtr nd;
 
  nd = xmlNewComment(reinterpret_cast<const xmlChar*>(comment_contents.c_str()));
  assert(nd);
  nd = xmlAddChild(dm_node, nd);
  assert(nd);

  return xml_node(nd);
}

void xml_node::clear_this(void)
{
  assert(is_valid());
  xmlUnlinkNode(dm_node);
  xmlFreeNode(dm_node);
}

void xml_node::add_attrib(const std::string &key, const std::string &val)
{
 
  xmlAttrPtr at;

  at = xmlNewProp(dm_node, reinterpret_cast<const xmlChar*>(key.c_str()),
    reinterpret_cast<const xmlChar*>(val.c_str()));
  assert(at);

}

xml_node xml_node::add_content_child(const std::string &content)
{
  xmlNodePtr nd;
 
  nd = xmlNewText(reinterpret_cast<const xmlChar*>(content.c_str()));
  assert(nd);
  nd = xmlAddChild(dm_node, nd);
  assert(nd);

  return xml_node(nd);
}

void xml_node::clear_content_children(void)
{
  while (true) {
    xml_node n = get_first_child_type(text_c);

    if (!n.is_valid())
      return; // done
    xmlUnlinkNode(n.dm_node);
    xmlFreeNode(n.dm_node);
  }
}

const char * xml_node::get_text_c_content(void) const
{
  if (!is_valid())
    return "";

  xml_node n = get_first_child_type(text_c);

  if (n.is_valid()) {
    assert(n.get_c_content());
    return n.get_c_content();
  } else
    return "";
}

const char * xml_node::get_leaf_text_content(const std::string &pathnodespec) const
{
  std::vector<std::string> parts;
  xml_node cur;

  string_tokenize(pathnodespec, parts, "/");

  if (parts.empty())
    return "";

  cur = *this;
  for (int x=0; x<parts.size(); ++x) {
    cur = cur.get_first_child_name(parts[x]);
    if (cur.is_null())
      return "";
  }

  return cur.get_text_c_content();
}

void xml_node::load_string_attrib(const std::string &name, std::string &outval)
{
  for (xml_attrib at = get_first_attrib(); at.is_valid(); at.next())
    if (name == at.get_c_name()) {
      outval = at.get_text_c_content();
      return;
    }

  outval.clear(); //nothin'
}

const char * xml_attrib::get_text_c_content(void) const
{
  if (!is_valid())
    return 0;

  xml_node n(dm_attr->children);

  assert(n.is_valid());
  assert(n.get_type() == xml_node::text_c);

  return n.get_c_content();

}

#ifdef SCO_DEBUG_xml
#include <scopira/tool/fileflow.h>
/* my /tmp/1.xml looks like this

<?xml version="1.0"?>
<entries>
  <module gui="yes" blah="blahval">
    <name>Hi</name>
    <slots>
      <input type="haha">in-matrix</input>
    </slots>
    <desc>Super module HX</desc>
  </module>
  <module>
    <name>M2</name>
    <desc>Some other module, yo</desc>
  </module>
  <module>
    <name>M-super</name>
    <desc>FIN</desc>
  </module>
</entries>


*/
int main(void)
{
  fileflow out(fileflow::stderr_c, 0);
  xml_doc D;
  xml_node mod, nam;
  xml_attrib at;
  bool b;

  /*b = D.parse_c_str(
    "<?xml version=\"1.0\" ?>"
    "<entries>"
    "  <module gui=\"yes\">"
    "    <name>Hi</name>"
    "    <slots>"
    "      <input type=\"haha\">in-matrix</input>"
    "    </slots>"
    "    <desc>Super module HX</desc>"
    "  </module>"
    "</entries>"
    );*/
  b = D.load_file("/tmp/1.xml");
  assert(b);

  for (mod = D.get_root().get_first_child(xml_node::element_c); mod.is_valid(); mod.next(xml_node::element_c)) {
    nam = mod.get_first_child("name", xml_node::element_c);
    out << " NAME: " << nam.get_text_c_content() << '\n';
    nam = mod.get_first_child("desc", xml_node::element_c);
    out << " DESC: " << nam.get_text_c_content() << '\n';
    at = mod.get_first_attrib();
    if (at.is_valid())
      out << " GUI! = \"" << at.get_text_c_content() << "\"\n";
  }

  D.init("budah");
  mod = D.get_root();
  mod.add_content_child("ONE");
  nam = mod.add_child("henry");
  nam.add_attrib("has_gui", "YES");
  nam.add_content_child("X-MAN");
  mod.add_content_child("TWO");

  std::string S;
  D.save_string(S);
  out << "STRING: \"" << S << "\"\n";

  D.save_file("/tmp/output.xml");
  //out << "Name: " << D.get_name() << '\n';

  return 0;
}
#endif


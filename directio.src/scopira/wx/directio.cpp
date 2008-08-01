
/*
 *  Copyright (c) 2004-2008    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <wx/xml/xml.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <scopira/wx/directio.h>

#include <scopira/tool/file.h>
#include <scopira/tool/fileflow.h>
#include <scopira/tool/output.h>

//
// The exact details/manutia of the xml parsing and twin file opening
// should be factored out into some common functions that the create/load/bin/directio_slice
// can all share
//

using namespace scopira::tool;
using namespace scopira::basekit;

namespace { class node_path
{
  public:
    class error {};
    class xml_error : public error {};
    class numeric_error : public error {};
    class verify_error : public error {};

  public:
    node_path(wxXmlDocument *doc)
      : dm_node(doc->GetRoot()) {}
    node_path(wxXmlNode *node)
      : dm_node(node) {}

    // get and make if needed
    node_path operator[](const wxString &subpath);
    // get and throw exception if not found
    node_path operator()(const wxString &subpath) const;

    wxXmlNode * operator->(void) const { return dm_node; }
    
    // assign to content node
    void operator = (const wxString &content);
    // assign to content node (numeric)
    void operator = (double d);

    wxString AsString(void) const { return dm_node->GetNodeContent(); }
    long AsLong(void) const;

    /// its like an assert
    static void verify(bool b) { if (!b) throw verify_error(); }

  private:
    wxXmlNode *dm_node;
};
}//namespace

node_path node_path::operator[] (const wxString &subpath)
{
  assert(dm_node);

  wxXmlNode *subnode = dm_node->GetChildren();

  // find the sub node
  for (; subnode; subnode = subnode->GetNext())
    if (subnode->GetName() == subpath)
      return subnode;
  // didnt find the child, add a new node
  dm_node->AddChild(subnode = new wxXmlNode(0, wxXML_ELEMENT_NODE, subpath));

  return subnode;
}

node_path node_path::operator()(const wxString &subpath) const
{
  assert(dm_node);

  wxXmlNode *subnode = dm_node->GetChildren();

  // find the sub node
  for (; subnode; subnode = subnode->GetNext())
    if (subnode->GetName() == subpath)
      return subnode;

  throw xml_error();
}

void node_path::operator = (const wxString &content)
{
  // find the text node
  assert(dm_node);

  wxXmlNode *subnode = dm_node->GetChildren();

  // find the sub node
  for (; subnode; subnode = subnode->GetNext())
    if (subnode->GetType() == wxXML_TEXT_NODE)
      break;

  if (!subnode)
    dm_node->AddChild(subnode = new wxXmlNode(0, wxXML_TEXT_NODE, L""));
  subnode->SetContent(content);
}

void node_path::operator = (double d)
{
  wxString s;
  s << d;
  (*this) = s;
}

long node_path::AsLong(void) const
{
  long ret;
  if (dm_node->GetNodeContent().ToLong(&ret))
    return ret;
  throw numeric_error();
}

static wxString justfilename(const wxString &filename)
{
  wxString name, ext;
  wxFileName::SplitPath(filename, 0, &name, &ext);

  return name + L"." + ext;
}

bool scopira::wx::save_directio_file_impl(const wxString &filename,
    short numdimen, const size_t *dimen, size_t elemsize, const char *id,
    const void *data)
{
  wxString datafile = filename + L".data";
  wxXmlDocument doc;
  size_t numelem = 1;
  // first, try to open the file

  wxLogNull nully;  //disable inane logging

  if (doc.Load(filename)) {
    // make sure the file type is proper
    if (doc.GetRoot()->GetName() != L"scopira_dio")
      return false;
  } else {
    // create a new file
    doc.SetRoot(new wxXmlNode(0, wxXML_ELEMENT_NODE, L"scopira_dio"));
  }

  node_path(doc.GetRoot())[L"elem_type"] = wxString::FromAscii(id);
  node_path(doc.GetRoot())[L"num_dimen"] = numdimen;
  for (short d=0; d<numdimen; ++d) {
    wxString field;

    field << L"size_" << d;
    node_path(doc.GetRoot())[field] = dimen[d];

    numelem *= dimen[d];
  }
  node_path(doc.GetRoot())[L"data_file"] = justfilename(datafile);

  // save the xml file
  if (!doc.Save(filename))
    return false;

  // save the data file porttion
  wxFile outbin(datafile, wxFile::write);

  outbin.Write(data, elemsize*numelem);

  return true;
}

// returns filename, but using srcpath's path
static wxString swappath(const wxString &filename, const wxString &srcpath)
{
  wxString path, name, ext;
  wxFileName::SplitPath(filename, 0, &name, &ext);
  wxFileName::SplitPath(srcpath, &path, 0, 0);

  return path + name + L"." + ext;
}


bool scopira::wx::load_directio_file_impl(const wxString &filename,
    short numdimen, size_t *dimen, size_t elemsize, const char *id,
    void * &outary)
{
  wxLogNull nully;  //disable inane logging

  try {
    wxXmlDocument doc;
    wxString datafile;
    size_t numelem = 1;

    if (!doc.Load(filename))
      return false;

    node_path::verify(node_path(doc.GetRoot())(L"elem_type").AsString() == wxString::FromAscii(id));
    node_path::verify(node_path(doc.GetRoot())(L"num_dimen").AsLong() == numdimen);

    datafile = node_path(doc.GetRoot())(L"data_file").AsString();

    for (short d=0; d<numdimen; ++d) {
      wxString field;

      field << L"size_" << d;
      dimen[d] = node_path(doc.GetRoot())(field).AsLong();

      numelem *= dimen[d];
    }

    node_path::verify(numelem>0);

    wxFile inbin(datafile, wxFile::read);

    if (!inbin.IsOpened())
      inbin.Open(swappath(datafile, filename), wxFile::read);
    if (!inbin.IsOpened())
      return false;

    char *c;

    assert(elemsize>0);
    assert(numelem>0);

    c = new char[elemsize*numelem];
    if (inbin.Read(c, elemsize*numelem) != elemsize*numelem) {
      delete []c;
      return false;
    }
    outary = c;
    return true;
  }
  catch (const node_path::error&) {
    // fall through to the return false
  }
  return false;
}

//#define TEST_DIRECTIO
#ifdef TEST_DIRECTIO
int main(void)
{
  narray<double,2> N, OUT;

  N.resize(5, 5);
  N.set_all(1);
  N.diagonal_slice().set_all(555);

  scopira::basekit::save_directio_file(L"datafile.xml", N);

  if (!scopira::basekit::load_directio_file(L"datafile.xml", OUT))
    OUTPUT << "failed to read the datafile.xml\n";
  else
    OUTPUT << "Hello!\n" << OUT << '\n';
}
#endif


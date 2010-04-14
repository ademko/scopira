
/*
 *  Copyright (c) 2010    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/qtkit/NodePath.h>

#include <assert.h>

using namespace scopira::qtkit;

//
//
// NodePath stuff, xml helper stuff
//
//

NodePath NodePath::operator[] (const QString &subpath)
{
  assert(!dm_node.isNull());

  QDomNodeList list = dm_node.elementsByTagName(subpath);

  if (list.isEmpty()) {
    // didnt find the child, add a new node
    QDomElement newelement = dm_node.ownerDocument().createElement(subpath);
    dm_node.appendChild(newelement);
    return newelement;
  } else
    return list.at(0).toElement();
}

NodePath NodePath::operator()(const QString &subpath) const
{
  assert(!dm_node.isNull());

  QDomNodeList list = dm_node.elementsByTagName(subpath);

  if (list.isEmpty())
    throw xml_error();
  else
    return list.at(0).toElement();
}

NodePath NodePath::append(const QString &subpath)
{
  assert(!dm_node.isNull());

  QDomElement newnode;

  // didnt find the child, add a new node
  newnode = dm_node.ownerDocument().createElement(subpath);
  dm_node.appendChild(newnode);

  return newnode;
}

bool NodePath::erase(const QString &subpath)
{
  assert(!dm_node.isNull());

  QDomNodeList list = dm_node.elementsByTagName(subpath);

  if (list.isEmpty())
    return false;

  dm_node.removeChild(list.at(0));
  return true;
}

bool NodePath::loadNextSibling(void)
{
  QDomElement next = dm_node.nextSiblingElement(dm_node.tagName());

  if (next.isNull())
    return false;

  dm_node = next;

  return true;
}

void NodePath::setPropVal(const QString &key, const QString &val)
{
  dm_node.setAttribute(key, val);
}

void NodePath::setPropVal(const QString &key, double val)
{
  setPropVal(key, QString().setNum(val));
}

QString NodePath::getPropAsString(const QString &key) const
{
  assert(!dm_node.isNull());

  if (dm_node.hasAttribute(key))
    return dm_node.attribute(key);
  else
    throw prop_error();
}

long NodePath::getPropAsLong(const QString &key) const
{
  long ret;
  bool ok;

  ret = getPropAsString(key).toLong(&ok);
  if (ok)
    return ret;
  throw numeric_error();
}

double NodePath::getPropAsDouble(const QString &key) const
{
  double ret;
  bool ok;

  ret = getPropAsString(key).toDouble(&ok);
  if (ok)
    return ret;
  throw numeric_error();
}

void NodePath::operator = (const QString &content)
{
  assert(!dm_node.isNull());

  for (QDomNode curchild = dm_node.firstChild(); !curchild.isNull(); curchild = curchild.nextSibling())
    if (curchild.isText()) {
      curchild.setNodeValue(content);
      return;
    }
  dm_node.appendChild(dm_node.ownerDocument().createTextNode(content));
}

void NodePath::operator = (double d)
{
  (*this) = QString().setNum(d);
}

long NodePath::asLong(void) const
{
  long ret;
  bool ok;

  ret = asString().toLong(&ok);
  if (ok)
    return ret;
  throw numeric_error();
}

double NodePath::asDouble(void) const
{
  double ret;
  bool ok;

  ret = asString().toDouble(&ok);
  if (ok)
    return ret;
  throw numeric_error();
}

QString NodePath::sanitizeField(const QString &fieldname)
{
  QString ret(fieldname);

  for (int i=0; i<ret.size(); ++i) {
    QChar c = ret[i];

    if (!c.isLetterOrNumber() && c != '_')
      ret[i] = '_';
  }

  return ret;
}

// end of NodePath


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

#ifndef __INCLUDED_SCOPIRA_QTKIT_NODEPATH_H__
#define __INCLUDED_SCOPIRA_QTKIT_NODEPATH_H__

#include <exception>

#include <QDomDocument>

namespace scopira
{
  namespace qtkit
  {
    class NodePath;
  }
}

/**
 * This is a helper class for parsing wxXml file structures.
 *
 * @author Aleksander Demko
 */ 
class scopira::qtkit::NodePath
{
  public:
    class error : public std::exception {
      virtual const char* what() const throw ()
      { return "NodePath::error"; }
    };
    class xml_error : public error {};
    class numeric_error : public error {};
    class verify_error : public error {};
    class prop_error : public error {};

  public:
    /**
     * Associate a NodePath based off the root node of the given document.
     *
     * @author Aleksander Demko
     */ 
    NodePath(QDomDocument doc)
      : dm_node(doc.documentElement()) {}
    /**
     * Associate a NodePath based off the given node
     *
     * @author Aleksander Demko
     */ 
    NodePath(QDomElement node)
      : dm_node(node) {}

    /// implicit converter to QDomElement
    operator QDomElement(void) const { return dm_node; }

    // get and make if needed
    NodePath operator[](const QString &subpath);
    // get and throw exception if not found
    NodePath operator()(const QString &subpath) const;

    // apend a new node with the given name (this always adds new nodes). node names need not be unique
    NodePath append(const QString &subpath);
    // erase the given subnode, if it exists, returns true of something was deleted
    bool erase(const QString &subpath);

    // loads the next sibling. return true if it was loaded, false on failure (on failre, "this" is now undefined
    // a sibling is the next sibling node that has the same name as this
    bool loadNextSibling(void);

    /// sets the property value
    void setPropVal(const QString &key, const QString &val);
    /// sets the property value
    void setPropVal(const QString &key, double val);

    // gets a prop as a string, throws if not found

    QString getPropAsString(const QString &key);
    // gets a prop as a long, throws if not found
    long getPropAsLong(const QString &key);
    // gets a prop as a double, throws if not found
    double getPropAsDouble(const QString &key);

    // in the future, add defaults that never throw/error

    QDomElement * operator->(void) { return &dm_node; }
    const QDomElement * operator->(void) const { return &dm_node; }
    
    // assign to content node
    void operator = (const QString &content);
    // assign to content node (numeric)
    void operator = (double d);

    QString asString(void) const { return dm_node.text(); }
    long asLong(void) const;

    /// its like an assert, but throws an exception on failure
    static void verify(bool b) { if (!b) throw verify_error(); }

    /// keeps only alphnum + underscore in the name
    static QString sanitizeField(const QString &fieldname);

  private:
    QDomElement dm_node;
};

#endif


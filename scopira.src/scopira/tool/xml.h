
/*
 *  Copyright (c) 2003    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INLUCDED__SCOPIRA_TOOL_XML_H_
#define __INLUCDED__SCOPIRA_TOOL_XML_H_

#include <libxml/parser.h>

#include <string>

#include <scopira/tool/object.h>
#include <scopira/tool/export.h>

// THIS FILE HAS BEEN FULLY DOCUMENTED

namespace scopira
{
  namespace tool
  {
    class xml_doc;
    class xml_node;
    class xml_attrib;
    // forward
    class property;
  }
}

//
// TIPS:
//  use xmllint.
//

/**
 * An XML document.
 * Not copyable.
 *
 * @author Aleksander Demko
 */
class scopira::tool::xml_doc : public scopira::tool::object
{
  private:
    xmlDocPtr dm_doc;

  public:
    /// ctor, creates a NULL doc, you should parse or init one
    /**
     * Constructs an empty document. You must then call one of the
     * init or load routines.
     * @author Aleksander Demko
     */ 
    xml_doc(void) : dm_doc(0) { }
    /// Destructor
    ~xml_doc() { clear(); }

    /**
     * Gets the name of this document as a C string
     * @author Aleksander Demko
     */ 
    const char * get_c_name(void) const { return dm_doc->name; }
    /**
     * Gets the name of this document
     * @author Aleksander Demko
     */ 
    std::string get_name(void) const
      { return std::string(reinterpret_cast<const char*>(dm_doc->name)); }

    /**
     * Clear and free any internal data
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void clear(void);

    /**
     * Constructs a new blank documement with the given root node name
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void init(const std::string &rootnodename);

    /**
     * Load a document from the given file. Will be decompressed if
     * need be.
     * @param filename the file to load from.
     * @return true on success
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool load_file(const std::string &filename);

    /**
     * Loads/parses a document from an in memory string
     * @param data the input string
     * @return true on success
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool load_string(const std::string &data);

    /**
     * Loads/parses a document from an in memory (C) string
     * @param data the input string
     * @return true on success
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool load_c_str(const char *str);

    /**
     * Loads from a property tree
     * @param p the property tree to use
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void load_prop(property *p);

    /**
     * Gets the root node.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT xml_node get_root(void) const;

    /**
     * Save the current document to file
     * @param filename the file to write out to
     * @param compress should the output file be compressed (default = false)
     * @return true on success
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool save_file(const std::string &filename, bool compress = false) const;

    /**
     * Saves the current document to the given output string
     * @param out the string to write to
     * @return true on success
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool save_string(std::string &out) const;

    /**
     * Saves this document to a property structure
     * @param p the structure to write out to
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void save_prop(property *p) const;

  private:
    // diable
    xml_doc(const xml_doc &) { }
};

/**
 * A pointer/link to a NODE within an existing XML document.
 * This is an ultra light, full copyable, non-virtual
 * class.
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::xml_node
{
  public:
    /// iterator-filter types
    enum {
      element_c = XML_ELEMENT_NODE,
      //attrib_c = XML_ATTRIBUTE_NODE,
      text_c = XML_TEXT_NODE,
    };

  private:
    xmlNodePtr dm_node;

  public:
    /**
     * Null pointer constructor
     * @author Aleksander Demko
     */ 
    xml_node(void) : dm_node(0) { }
    /**
     * Initialization constructor
     * @author Aleksander Demko
     */ 
    explicit xml_node(xmlNodePtr ptr) : dm_node(ptr) { }

    /**
     * Gets the node type
     * @author Aleksander Demko
     */ 
    int get_type(void) const { return dm_node->type; }

    /**
     * Gets the name of this node, (C) string
     * @author Aleksander Demko
     */ 
    const char * get_c_name(void) const
      { return reinterpret_cast<const char*>(dm_node->name); }
    /**
     * Gets the name of this node
     * @author Aleksander Demko
     */ 
    std::string get_name(void) const
      { return std::string(reinterpret_cast<const char*>(dm_node->name)); }

    /**
     * Gets the text of this node, as a C string.
     * This may return null!
     *
     * @author Aleksander Demko
     */ 
    const char * get_c_content(void) const
      { return reinterpret_cast<const char *>(dm_node->content); }

    /**
     * Is this an empty node?
     * @author Aleksander Demko
     */ 
    bool is_null(void) const { return dm_node == 0; }
    /**
     * Is this node not-empty (valid)?
     * @author Aleksander Demko
     */ 
    bool is_valid(void) const { return dm_node != 0; }

    /**
     * Advance this node to the next node in the list
     * @author Aleksander Demko
     */ 
    void next(void) { dm_node = dm_node->next; }
    /**
     * Advance this node to the next node in the list
     * that has the given type
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void next_type(int typ);
    /**
     * Advance this node to the next node in the list
     * that has the given name
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void next_name(const std::string &name);

    /**
     * Counts the number of children nodes with the given name
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT int count_children(const std::string &name) const;

    /**
     * Get the first child node
     * May return an empty node.
     * @author Aleksander Demko
     */ 
    xml_node get_first_child(void) const
      { return xml_node(dm_node->children); }
    /**
     * Get the first child node with the given type
     * May return an empty node.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT xml_node get_first_child_type(int typ) const;
    /**
     * Get the first child node with the given name
     * May return an empty node.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT xml_node get_first_child_name(const std::string &name) const;

    /**
     * Gets the first attribute of this node
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT xml_attrib get_first_attrib(void) const;
    /**
     * Super helper.
     * Return the full attrib value of the given key.
     *
     * Returns "" on not found.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT const char * get_attrib_c_text(const std::string &aname) const;
    /**
     * Super helper.
     * Returns the full c_area of the SUB node with the given name.
     *
     * Returns "" on any and all errors.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT const char * get_child_c_text(const std::string &nname) const;

    /**
     * Does this node have an attribute with the given name?
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool has_attrib(const std::string &aname) const;

    /**
     * Adds a new child node with the given name and return it
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT xml_node add_child(const std::string &name);
    /**
     * Sets/adds the given attribute and value
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void add_attrib(const std::string &key, const std::string &val);
    /**
     * Adds the given content/text block to this node
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT xml_node add_content_child(const std::string &content);

    /**
     * Gets the first text/content block's text.
     * If this node has no text blocks, an empty (but otherwise valid)
     * string will be returned.
     * Never returns null when the node itself is valid.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT const char * get_text_c_content(void) const;
    
    /**
     * Gets the first text/content block's text.
     *
     * This is a convience verion, that allows you to drill down into
     * sub nodes, using / in the names.
     * It never returns null or fails, but will return empty
     * string if it can't find a particular node.
     *
     * Examples: "blah" "leafnode/nodename" "subnode/leafnode/nodename"
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT const char * get_leaf_text_content(const std::string &pathnodespec) const;
    
    // Mass loader helpers

    /**
     * Loads the given attribute. outval will be set to ""
     * if the given attribute could not be found.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void load_string_attrib(const std::string &name, std::string &outval);
    /**
     * Applies load_xml to all the sub elements in the iteration.
     * @author Aleksander Demko
     */ 
    template <class I>
      void children_load_xml(const std::string &elementname, I iter) const {
        for (xml_node sub = get_first_child_name(elementname);
            sub.is_valid(); sub.next_name(elementname), ++iter)
          (*iter).load_xml(sub);
      }
    /**
     * Loads attributes from all the sub elements in the iteration
     * @author Aleksander Demko
     */ 
    template <class I>
      void children_load_attrib(const std::string &elementname,
          const std::string &attribname, I iter) const {
        for (xml_node sub = get_first_child_name(elementname);
            sub.is_valid(); sub.next_name(elementname), ++iter)
          sub.load_string_attrib(attribname, *iter);
      }
    /**
     * Loads texts/content blocks from all the sub elementname in the iteration
     * @author Aleksander Demko
     */ 
    template <class I>
      void children_load_text(const std::string &elementname, I iter) const {
        for (xml_node sub = get_first_child_name(elementname);
            sub.is_valid(); sub.next_name(elementname), ++iter)
          *iter = sub.get_text_c_content();
      }
};

/**
 * An XML node attribute pointer/link.
 *
 * @author Aleksander Demko
 */
class scopira::tool::xml_attrib
{
  private:
    xmlAttrPtr dm_attr;

  public:
    /**
     * Null constructor
     * @author Aleksander Demko
     */ 
    xml_attrib(void) : dm_attr(0) { }
    /**
     * Initializaing construtor.
     * @author Aleksander Demko
     */ 
    xml_attrib(xmlAttrPtr ptr) : dm_attr(ptr) { }

    /**
     * Gets the name of this attribute, as a C string
     * @author Aleksander Demko
     */ 
    const char * get_c_name(void) const
      { return reinterpret_cast<const char*>(dm_attr->name); }
    /**
     * Gets the name of this attribute
     * @author Aleksander Demko
     */ 
    std::string get_name(void) const
      { return std::string(reinterpret_cast<const char*>(dm_attr->name)); }

    /**
     * Is this attribute null/empty?
     * @author Aleksander Demko
     */ 
    bool is_null(void) const { return dm_attr == 0; }
    /**
     * Is this attribute not null/empty?
     * @author Aleksander Demko
     */ 
    bool is_valid(void) const { return dm_attr != 0; }

    /**
     * Gets the next attribute in the set
     * @author Aleksander Demko
     */ 
    void next(void) { dm_attr = dm_attr->next; }

    /**
     * Gets the value/text/content of this attribute.
     * Return null on failure.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT const char * get_text_c_content(void) const;
};

#endif


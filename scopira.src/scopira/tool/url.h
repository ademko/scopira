
/*
 *  Copyright (c) 2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INLUCDED__SCOPIRA_TOOL_URL_H_
#define __INLUCDED__SCOPIRA_TOOL_URL_H_

#include <string>

#include <scopira/tool/export.h>

namespace scopira
{
  namespace tool
  {
    class url;

    class itflow_i; //fwd
    class otflow_i; //fwd
    class oflow_i; //fwd
  }
}

/**
 * A web-like URL (uniform resource locator) class.
 * This class represents one URL and allows access to
 * its various components.
 *
 * The components are:
 *  protocol://hostname:port/filename
 *  port will not include the : (how can it, it's an integer)
 *  filename will contain the leading /, however. filename definatly may be empty-string
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::url
{
  protected:
    /// the full url
    std::string dm_full;

    // the various components of the url

    /// protocol (never empty)
    std::string::size_type dm_proto_begin, dm_proto_end;
    /// hostname (may be empty)
    std::string::size_type dm_host_begin, dm_host_end;
    /// port (0 for non explicitly provided)
    int dm_port;
    /// filename (after the directory path)
    std::string::size_type dm_file_begin, dm_file_end;

  public:
    /**
     * Empty url constructor.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT url(void);
    /**
     * Constructs a url from the given url string.
     * This version is only intended for string literals -- strings
     * where you know the url is of the valid form.
     * If the url may be malformed, use the empty contrustor
     * and use the set_url call.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT url(const std::string &fullurl);
    /**
     * Constructs a url for the the host and port.
     * The protocol will be "scopira"
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT url(const std::string &hostname, int port);
    /**
     * Constructs a url by components.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT url(const std::string &proto, const std::string &hostname, int port, const std::string &filename);
    /**
     * Simple Destructor
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT ~url();

    /**
     * Useful when part of other objects that are serializable.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool load(scopira::tool::itflow_i& in);
    /**
     * Useful when part of other objects that are serializable.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void save(scopira::tool::otflow_i& out) const;

    /**
     * Clears the url
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void clear_url(void);

    /**
     * Sets the url from a full url string.
     * Returns true on successully parse.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool set_url(const std::string &fullurl);
    /**
     * Sets the url by host and port.
     * The protocol will be "scopira"
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void set_url(const std::string &hostname, int port);
    /**
     * Sets the url by components
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void set_url(const std::string &proto, const std::string &hostname, int port, const std::string &filename);

    /**
     * Gets the full url
     * @author Aleksander Demko
     */
    const std::string & get_url(void) const { return dm_full; }

    /**
     * Gets the protocol segment
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT std::string get_proto(void) const;
    /**
     * Gets the hostname segment
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT std::string get_hostname(void) const;
    /**
     * Gets the port. If the url doesn't have an explicit
     * port specified, then this will be 0.
     * @author Aleksander Demko
     */
    int get_port(void) const { return dm_port; }
    /**
     * Gets the filename segment
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT std::string get_filename(void) const;
};

SCOPIRA_EXPORT scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, const scopira::tool::url& u);

#endif


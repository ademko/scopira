
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
 
#ifndef __INCLUDED_SCOPIRA_TOOL_ARCHIVEFLOW_H__
#define __INCLUDED_SCOPIRA_TOOL_ARCHIVEFLOW_H__

#include <scopira/tool/polyflow.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace tool
  {
    class archiveiflow;
    class archiveoflow;

    //make quick one liners in the future?
    //bool write_object_archive(const std::string &filename, scopira::tool::object *o);
    // templated?
    //bool read_object_archive(const std::string &filename, scopira::tool::object *o);
  }
}

/**
 * Opens an archive for reading.
 *
 * An archive contains a header, after that you simply read the various
 * types using this objects' flow interfaces.
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::archiveiflow : public scopira::tool::isoiflow
{
  public:
    /// ctor
    SCOPIRA_EXPORT archiveiflow(void);

    /// failed state?
    SCOPIRA_EXPORT virtual bool failed(void) const;

    /**
     * Attempts to open a previously created archive.
     *
     * The that file is not an archive, or the tag within the archive
     * does not begin with tagprefix, the stream goes into a failed state and
     * empty string is returned.
     *
     * On success, the actual tag is returned.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT std::string open(const std::string &filename, const std::string &tagprefix);
};

/**
 * A object that represents a writable on-disk "archive". An archive
 * is basically a serialization stream with a header at the front,
 * which contains some versioning tag.
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::archiveoflow : public scopira::tool::isooflow
{
  public:
    /// ctor
    SCOPIRA_EXPORT archiveoflow(void);

    /// failed state?
    SCOPIRA_EXPORT virtual bool failed(void) const;

    /**
     * Open method.
     *
     * Returns true on success.
     *
     * Should this support text file formats in the future?
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void open(const std::string &filename, const std::string &tagkey);
};

#endif


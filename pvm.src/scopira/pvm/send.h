
/*
 *  Copyright (c) 2006    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_PVM_SEND_H__
#define __INCLUDED_SCOPIRA_PVM_SEND_H__

#include <scopira/tool/binflow.h>

namespace scopira
{
  namespace pvm
  {
    class send_pack;
    class recv_pack;

    /**
     * Prove the receive queue to see if a message matching the parameters
     * is waiting.
     *
     * If there is a message (returns true), then you MUST then receive the message using the (void)
     * recv_pack constructor.
     *
     * @param srctaskid the source task, or -1 for any source
     * @param msgtag the msg tag, or -1 for any tag
     * @param outtaskid if non-null, the actual source will be written here of any successfuly probed message
     * @param outmsgtag if non-null, the actual msgtag will be written here of any successfuly probed message
     * @return true if a message is in the queue
     * @author Aleksander Demko
     */ 
    bool probe_pack(int srctaskid, int msgtag = -1, int *outtaskid = 0, int *outmsgtag = 0);
    /**
     * Prove the receive queue to see if a message matching the parameters
     * is waiting.
     *
     * If there is a message (returns true), then you MUST then receive the message using the (void)
     * recv_pack constructor.
     *
     * @param the timeout to wait, in msec (1000 msec = 1 second). 0 == indefinate wait
     * @param srctaskid the source task, or -1 for any source
     * @param msgtag the msg tag, or -1 for any tag
     * @param outtaskid if non-null, the actual source will be written here of any successfuly probed message
     * @param outmsgtag if non-null, the actual msgtag will be written here of any successfuly probed message
     * @return true if a message is in the queue
     * @author Aleksander Demko
     */ 
    bool probe_pack_timeout(int timeout, int srctaskid, int msgtag = -1, int *outtaskid = 0, int *outmsgtag = 0);
  }
}

/**
 * Encapsulates a complete send-packet in PVM.
 * Upon this object's destruction, the object will be send.
 *
 * @author Aleksander Demko
 */ 
class scopira::pvm::send_pack : public scopira::tool::bin64oflow
{
  public:
    /**
     * Prepares a message package to desttaskid with the given msgtag
     *
     * @param desttaskid the destination task
     * @param msgtag the message tag. can be any integer >= 0
     * @author Aleksander Demko
     */ 
    send_pack(int desttaskid, int msgtag = 0);
    /// destructor
    virtual ~send_pack();

    /// is this flow in a failed state?
    virtual bool failed(void) const { return false; }

    /// internal write implementation
    virtual size_t write(const byte_t *_buf, size_t _size);

  private:
    int dm_desttaskid, dm_msgtag;
};

/**
 * Receives and unpacks a message packet sent my send_pack.
 *
 * @author Aleksander Demko
 */
class scopira::pvm::recv_pack : public scopira::tool::bin64iflow
{
  public:
    /**
     * This will unpack a message ONLY after a successful call to
     * probe_pack_timeout.
     *
     * @author Aleksander Demko
     */
    recv_pack(void);

    /**
     * Receives a message from the given source.
     *
     * @param srctaskid the source taskid, or -1 for any
     * @param msgtag the message tag to look for, or -1 for any
     * @author Aleksander Demko
     */
    recv_pack(int srctaskid, int msgtag = -1);
    /// destructor
    virtual ~recv_pack();

    /// is this flow in a failed state?
    virtual bool failed(void) const { return false; }

    /// internal read implementation
    virtual size_t read(byte_t *_buf, size_t _maxsize);

    /**
     * Returns the src taskid, if using the int,int ctor
     *
     * @author Aleksander Demko
     */
    int get_source(void) const { return dm_srctaskid; }

    /**
     * Returns the msgtag, if using the int,int ctor
     *
     * @author Aleksander Demko
     */
    int get_msgtag(void) const { return dm_msgtag; }

  private:
    int dm_srctaskid, dm_msgtag;
};

#endif



/*
 *  Copyright (c) 2002    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */
 
#ifndef __INCLUDED_SCOPIRA_TOOL_DISTFLOW_H__
#define __INCLUDED_SCOPIRA_TOOL_DISTFLOW_H__

#include <vector>

#include <scopira/tool/flow.h>

namespace scopira
{
  namespace tool
  {
    class teeoflow;
    class distoflow;
  }
}

/**
 * a distoflow subset that only handles distribution to TWO oflows.
 * The name tee comes from the unix command.
 * Either oflow (A or B) may be null.
 * @author Aleksander Demko
 */
class scopira::tool::teeoflow : public scopira::tool::oflow_i
{
  protected:
    /// output links
    typedef scopira::tool::count2_ptr< scopira::tool::oflow_i > holder_t;

    holder_t dm_a, dm_b;    /// the two flows

  public:
    /// default constructor
    explicit teeoflow(bool doref);

    /// are we in a failed state?
    virtual bool failed(void) const;

    /// write a raw block of data
    virtual size_t write(const byte_t* _buf, size_t _size);

    /// sets the A (first) flow. may be null
    void set_flow_a(oflow_i* r);
    /// sets the B (first) flow. may be null
    void set_flow_b(oflow_i* r);
};

/**
 * distributes all its output to its list of oflow_i.
 *
 * @author Aleksander Demko
 */
class scopira::tool::distoflow : public scopira::tool::oflow_i
{
  protected:
    /// output links
    typedef std::vector< scopira::tool::count2_ptr< scopira::tool::oflow_i > > oflowvec_t;

    oflowvec_t dm_flow;
    bool dm_doref;

  public:
    /// default constructor
    explicit distoflow(bool doref);

    /// are we in a failed state?
    virtual bool failed(void) const;

    /// write a raw block of data
    virtual size_t write(const byte_t* _buf, size_t _size);

    void add_flow(oflow_i* r);
    void remove_flow(oflow_i* r);
    void clear_flow(void);
    bool contains_flow(const oflow_i* r) const;
    int size_flow(void) const;
};

#endif



/*
 *  Copyright (c) 2005    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_LAB_SIDEKICKVIEW_H__
#define __INCLUDED_SCOPIRA_LAB_SIDEKICKVIEW_H__

#include <scopira/core/model.h>
#include <scopira/core/view.h>
#include <scopira/core/sidekick.h>

namespace scopira
{
  namespace lab
  {
    template <class MODELT> class sidekickview;
  }
}

/**
 * sidekickview provides a foundation for turning "lab" programs
 * (programs that popup *_windows and/or check/use config stuff
 * to also work as view_i's.
 *
 * To turn your lab-aware method into a sidekickview, you descend from this
 * class and supply a run() method. In this method you can perform any
 * of your usual lab stuff.
 *
 * You may also inspect dm_model for your bound input/output model. However,
 * (and this is important), you must enclose all accesses to this structure within lab_locks,
 * as your run() runs in the worker thread and this class is owned by the gui thread.
 * This magic, however, doesn't work under MS Windows.
 *
 * @author Aleksander Demko
 */ 
template <class MODELT> class scopira::lab::sidekickview : public virtual scopira::core::view_i,
  public virtual scopira::core::sidekick_i
{
  public:
    /// dispatcher to run()
    virtual void bind_model(scopira::core::model_i *sus);
  protected:
    /// ctor
    sidekickview(void);
  protected:
    typedef MODELT model_type;
    scopira::core::model_ptr<MODELT> dm_model;
};

template <class MODELT>
scopira::lab::sidekickview<MODELT>::sidekickview(void)
  : dm_model(this)
{
}

template <class MODELT>
void scopira::lab::sidekickview<MODELT>::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<MODELT*>(sus);
  if (!dm_model.get())
    return; // nothin to work on

  // finally, queue the sidekick for worker running
  scopira::core::enqueue_sidekick(this, 0);
}

#endif



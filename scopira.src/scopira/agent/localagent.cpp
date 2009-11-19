
/*
 *  Copyright (c) 2005-2008    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/agent/localagent.h>

#include <ctype.h>

#include <scopira/tool/objflowloader.h>
#include <scopira/tool/fileflow.h>
#include <scopira/tool/printflow.h>
#include <scopira/tool/util.h>
#include <scopira/core/loop.h>
#include <scopira/agent/ids.h>
#include <scopira/agent/register.h>

//BBtargets libscopira.so

using namespace scopira::tool;
using namespace scopira::agent;
using scopira::basekit::narray;


//
//
// machine_spec
//
//

machine_spec::machine_spec(void)
{
  pm_numcpu = 0;
  pm_intelorder = true;
  pm_64bit = false;
  pm_speed = 0;
  pm_ram = 0;
}

bool machine_spec::load(scopira::tool::itflow_i& in)
{
  return
    in.read_short(pm_numcpu) &&
    in.read_bool(pm_intelorder) &&
    in.read_bool(pm_64bit) &&
    in.read_int(pm_speed) &&
    in.read_int(pm_ram);
}

void machine_spec::save(scopira::tool::otflow_i& out) const
{
  out.write_short(pm_numcpu);
  out.write_bool(pm_intelorder);
  out.write_bool(pm_64bit);
  out.write_int(pm_speed);
  out.write_int(pm_ram);
}

static int get_ram(void)
{
#ifdef PLATFORM_linux
  /// open /proc/cpuinfo and count some lines
  fileflow inf("/proc/meminfo", fileflow::input_c);

  if (inf.failed())
    return 0;

  printiflow input(false, &inf);
  std::string line;
  int ret = 0;

  line.reserve(250);

  while (input.read_string(line))
    if (line.compare(0, 9, "MemTotal:") == 0) {
      std::string numbers;

      numbers.reserve(250);
      for (int i=0; i<line.size(); ++i)
        if (isdigit(line[i]))
          numbers.push_back(line[i]);
      if (!string_to_int(numbers, ret))
        ret = 0;
    }

  return ret;
#else
  return 0;
#endif
}

void machine_spec::set_auto_detect(void)
{
  long num = 5;
  char *pnum = reinterpret_cast<char*>(&num);

  pm_numcpu = num_system_cpus();
  pm_intelorder = *pnum == 5;
#ifdef PLATFORM_64
  pm_64bit = true;
#else
  pm_64bit = false;
#endif
  pm_speed = 100; // TODO
  pm_ram = get_ram();
}

scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, const scopira::agent::machine_spec &spec)
{
  o << "[#cpu=" << spec.pm_numcpu << " intelorder=" << spec.pm_intelorder <<
    " 64bit=" << spec.pm_64bit << " speed=" << spec.pm_speed << " ram=" << spec.pm_ram << ']';
  return o;
}

//
//
// local_agent
//
//

local_agent::local_agent(void)
{
  dm_spec.set_auto_detect();
  assert(dm_spec.pm_numcpu>0);

  dm_agentclock.start();

  // start the threads

  // lets do two for now, but in the future, we'll have base this off a
  // config param or autodetect
  {
    event_ptr<kernel_area> L(dm_kernel);

    for (int x=0; x<dm_spec.pm_numcpu; ++x)
      L->pm_threads.push_back(new thread(worker_func, this));

    // now start all the threads
    for (int x=0; x<L->pm_threads.size(); ++x)
      L->pm_threads[x]->start();
  }

  dm_kernel.pm_condition.notify_all();
}

local_agent::~local_agent()
{
  {
    event_ptr<kernel_area> L(dm_kernel);

    if (L->pm_alive) {
      L->pm_alive = false;
      L.notify_all();
    }
  }

  bool alive = true;
  int x = 0;
  count_ptr<thread> curthread;
  while (alive) {
    // grab the next thread
    // (we dont want to hold this lock while we wait, though
    {
      event_ptr<kernel_area> L(dm_kernel);

      if (x<L->pm_threads.size()) {
        curthread = L->pm_threads[x];
        ++x;
      } else {
        alive = false;
        break;  //out of the while
      }
    }

    // wait for the threads to die
    curthread->wait_stop();
  }
  curthread = 0;

}

void local_agent::notify_stop(void)
{
  {
    event_ptr<kernel_area> L(dm_kernel);

    L->pm_alive = false;
    L.notify_all();
  }

  // do this last so that all the threads see the dead state
  kill_all_local_tasks();
}

void local_agent::wait_stop(void)
{
  event_ptr<kernel_area> L(dm_kernel);

  while (L->pm_alive)
    L.wait();
}

void local_agent::reg_context(scopira::tool::uuid &ctxid, taskmsg_reactor_i *reac)
{
  count_ptr<process_t> p;

  dm_uugen.next(ctxid);

  p = new process_t(ctxid, process_t::ps_context_c);

  // set the reactor, if any
  if (reac) {
    event_ptr<process_t::state_area> L(p->pm_state);
    L->pm_reactor = reac;
  }

  {
    write_locker_ptr<ps_area> L(dm_psarea);

    L->pm_ps[ctxid] = p;
  }

  set_min_threads();
}

void local_agent::unreg_context(scopira::tool::uuid ctxid)
{
  assert(is_alive_object());
  write_locker_ptr<ps_area> L(dm_psarea);

  assert(!ctxid.is_zero());
  assert(L->pm_ps.find(ctxid) != L->pm_ps.end());
  L->pm_ps.erase(L->pm_ps.find(ctxid));
}

int local_agent::find_services(scopira::tool::uuid &serviceid, scopira::basekit::narray<scopira::tool::uuid> &out)
{
  std::vector<scopira::tool::uuid> targetlist;    // no pre reserve, this will usually be a small/empty list for the most part

  // find all the local tasks that support this service
  if (serviceid == agent_service_c || serviceid == worker_agent_service_c) {
    // send back an agent list instead
    targetlist.push_back(where_this_c);
  } else {
    read_locker_ptr<ps_area> L(dm_psarea);

    for (psmap_t::const_iterator ii=L->pm_ps.begin(); ii != L->pm_ps.end(); ++ii)
      if ((*ii->second).pm_services.count(serviceid))
        targetlist.push_back(ii->first);
  }

  // finally, return this list to the caller
  out.resize(targetlist.size());
  std::copy(targetlist.begin(), targetlist.end(), out.begin());

  return targetlist.size();
}

int local_agent::universe_size(void)
{
  assert(dm_spec.pm_numcpu>0);
  return dm_spec.pm_numcpu;
}

scopira::tool::uuid local_agent::get_agent_id(void)
{
  return where_this_c;
}

scopira::tool::uuid local_agent::launch_task(const std::type_info &t, scopira::tool::uuid where)
{
  //local_agent version of launch_task ignores the where
  uuid psid;
  count_ptr<process_t> p;

  assert(objflowloader::instance()->has_typeinfo(t) && "[Given type to launch_task is not registered]");

  // create the process
  dm_uugen.next(psid);

  p = new process_t(psid);

  p->load_services(t);

  object *o = objflowloader::instance()->load_object(t);
  assert(o);
  assert(dynamic_cast<agent_task_i*>(o));
  p->pm_task = dynamic_cast<agent_task_i*>(o);

  {
    event_ptr<process_t::state_area> LL(p->pm_state);

    LL->pm_mode = process_t::ps_ready_c;
  }

  // insert the process
  {
    write_locker_ptr<ps_area> L(dm_psarea);

    L->pm_ps[psid] = p;
  }

  set_min_threads();

  return psid;
}

scopira::tool::uuid local_agent::launch_group(int numps, const std::type_info &t)
{
  int x;

  assert(numps>0 && "[launch_group numps must be >0");

  narray<uuid> id;
  std::vector<count_ptr<process_t> > p;

  assert(objflowloader::instance()->has_typeinfo(t) && "[Given type to launch_group is not registered]");

  id.resize(numps);
  p.resize(numps);

  // create the ids
  for (x=0; x<id.size(); ++x)
    dm_uugen.next(id[x]);

  // create the processes
  for (x=0; x<id.size(); ++x) {
    p[x] = new process_t(x, id);

    p[x]->load_services(t);

    // instantiate the slave nodes
    object *o = objflowloader::instance()->load_object(t);
    assert(o);
    assert(dynamic_cast<agent_task_i*>(o));
    p[x]->pm_task = dynamic_cast<agent_task_i*>(o);
  }

  for (x=0; x<id.size(); ++x) {
    event_ptr<process_t::state_area> LL(p[x]->pm_state);

    LL->pm_mode = process_t::ps_ready_c;
  }

  // insert the processes
  {
    write_locker_ptr<ps_area> L(dm_psarea);

    for (x=0; x<id.size(); ++x)
      L->pm_ps[id[x]] = p[x];
  }

  set_min_threads();

  return id[0];
}

void local_agent::launch_slaves(scopira::tool::uuid masterid,
      int numtotalps, const std::type_info &t,
      narray<uuid> &peers)
{
  int x;
  std::vector<count_ptr<process_t> > p;

  assert(numtotalps>0 && "[launch_slaves numps must be >0");

  assert(!masterid.is_zero());
  assert(objflowloader::instance()->has_typeinfo(t) && "[Given type to launch_slaves is not registered]");

  // peer 0 is special, as its the caller process
  peers.resize(numtotalps);
  p.resize(numtotalps);

  // create the ids
  peers[0] = masterid;
  for (x=1; x<peers.size(); ++x)
    dm_uugen.next(peers[x]);

  // create the processes
  // the first one is the caller
  for (x=1; x<peers.size(); ++x) {
    p[x] = new process_t(x, peers);

    p[x]->load_services(t);

    // instantiate the slave nodes
    object *o = objflowloader::instance()->load_object(t);
    assert(o);
    assert(dynamic_cast<agent_task_i*>(o));
    p[x]->pm_task = dynamic_cast<agent_task_i*>(o);
  }

  for (x=1; x<peers.size(); ++x) {
    event_ptr<process_t::state_area> LL(p[x]->pm_state);

    LL->pm_mode = process_t::ps_ready_c;
  }

  // insert the processes
  {
    write_locker_ptr<ps_area> L(dm_psarea);

    for (x=1; x<peers.size(); ++x)
      L->pm_ps[peers[x]] = p[x];

    //while we're here, lets go fetch process 0 and update it for fun
    assert(L->pm_ps.find(peers[0]) != L->pm_ps.end());
    p[0] = L->pm_ps[peers[0]];
    assert(p[0].get());
    // update its info to reflect its new, master status
    assert(p[0]->pm_index == 0);
    p[0]->pm_peers = peers;
  }

  set_min_threads();
}

void local_agent::kill_task(scopira::tool::uuid ps)
{
  count_ptr<process_t> foundps;

  // find the process and set its kill flag
  {
    read_locker_ptr<ps_area> L(dm_psarea);
    psmap_t::const_iterator ii;

    ii = L->pm_ps.find(ps);

    if (ii == L->pm_ps.end())
      return;   // not found

    foundps = ii->second;
  }

  assert(foundps.get());

  // set the processes kill flag, if its not dead already
  {
    event_ptr<process_t::state_area> L(foundps->pm_state);

    if (L->pm_mode != process_t::ps_done_c)
      L->pm_killreq = true;

    // notify everyone
    L.notify_all();
  }
}

bool local_agent::wait_task(scopira::tool::uuid ps, int msec)
{
  count_ptr<process_t> foundps;

  // find the process and set its kill flag
  {
    read_locker_ptr<ps_area> L(dm_psarea);
    psmap_t::const_iterator ii;

    ii = L->pm_ps.find(ps);

    if (ii == L->pm_ps.end())
      return true;   // not found

    foundps = ii->second;
  }

  assert(foundps.get());

  // set the processes kill flag, if its not dead already
  {
    event_ptr<process_t::state_area> L(foundps->pm_state);

    if (L->pm_mode == process_t::ps_done_c)
      return true; // already dead

    if (msec == 0) { // do indefinate loop?
      while (L->pm_mode != process_t::ps_done_c)
        L.wait();
      return true; //always dead after this
    }

    // do one sleep
    L.wait(msec);   // TODO this is wrong, as we must take into account cummulative waits and such

    return L->pm_mode == process_t::ps_done_c;
  }
}

bool local_agent::is_alive_task(scopira::tool::uuid ps)
{
  count_ptr<process_t> foundps;

  // find the process and set its kill flag
  {
    read_locker_ptr<ps_area> L(dm_psarea);
    psmap_t::const_iterator ii;

    ii = L->pm_ps.find(ps);

    if (ii == L->pm_ps.end())
      return false;   // not found

    foundps = ii->second;
  }

  assert(foundps.get());

  event_ptr<process_t::state_area> L(foundps->pm_state);
  return L->pm_mode != process_t::ps_done_c;
}

bool local_agent::is_killed_task(scopira::tool::uuid ps)
{
  count_ptr<process_t> foundps;

  // find the process and set its kill flag
  {
    read_locker_ptr<ps_area> L(dm_psarea);
    psmap_t::const_iterator ii;

    ii = L->pm_ps.find(ps);

    if (ii == L->pm_ps.end()) {
      assert(false);    // this task should always exist
      return false;   // not found
    }

    foundps = ii->second;
  }

  assert(foundps.get());

  event_ptr<process_t::state_area> L(foundps->pm_state);
  return L->pm_killreq;
}

bool local_agent::wait_msg(const uuid_query &srcq, scopira::tool::uuid &foundsrc, scopira::tool::uuid dest, int timeout)
{
  count_ptr<process_t> ps;

  assert(!dest.is_zero());
  assert(timeout>=0 || timeout == -7734);

  get_ps(dest, ps);

  if (!ps.get())
    return false;

  while (true) {
    event_ptr<process_t::state_area> LL(ps->pm_state);

    // check if we have a msg
    for (msglist_t::iterator ii = LL->pm_msgqueue.begin(); ii != LL->pm_msgqueue.end(); ++ii)
      if (srcq.query(ii->pm_src)) {
        foundsrc = ii->pm_src;
        return true; // found a msg!
      }

    // no? do some waiting
    if (timeout < 0)
      return false;   // 2nd pass of a timed wait OR -7734/has_msg, fail.
    else if (timeout > 0) {
      LL.wait(timeout);   //TODO do proper compound
      timeout = -1;
    } else
      LL.wait();
  }
}

void local_agent::send_msg(scopira::tool::uuid src, scopira::tool::uuid dest, scopira::tool::bufferflow *buf)
{
  count_ptr<process_t> ps;

  // is dest zero, bcast it to all the people in this processes group
  // by simply doing a recursive send
  if (dest.is_zero()) {
    // broad cast time
    assert(!src.is_zero());

    get_ps(src, ps);
    if (!ps.get())
      return;   // not found!

    // send to everyone but myself (yes, we can share the buffer object)
    for (int x=0; x<ps->pm_peers.size(); ++x)
      if (x != ps->pm_index)
        send_msg(src, ps->pm_peers[x], buf);

    return;
  }

  // find the process locally
  get_ps(dest, ps);

  // if the process isnt local, then call the hanlder (if any) and exit now
  if (!ps.get()) {
    // not found! lets send it off anyways via the la_send_msg function as a last resort
    la_send_msg(src, dest, buf);
    return;   // not found!
  }

  // finally, deliver the sevent to the local task process
  taskmsg_reactor_i *reac;

  // ok, do the queue and notify then, locally
  {
    event_ptr<process_t::state_area> LL(ps->pm_state);

    LL->pm_msgqueue.push_back(msg_t(src, buf));

    reac = LL->pm_reactor;
  }
  ps->pm_state.pm_condition.notify_all();

  // call the reactor, if any
  if (reac)
    reac->react_taskmsg(this, src, dest);
}

void local_agent::send_msg_bcast(scopira::tool::uuid src, scopira::tool::uuid destserviceid, scopira::tool::bufferflow *buf)
{
  // do local delivery of this bcast msg
  std::vector<scopira::tool::uuid> targetlist;    // no pre reserve, this will usually be a small/empty list for the most part

  // find all the local tasks that support this service
  {
    read_locker_ptr<ps_area> L(dm_psarea);

    for (psmap_t::const_iterator ii=L->pm_ps.begin(); ii != L->pm_ps.end(); ++ii)
      if ((*ii->second).pm_services.count(destserviceid))
        targetlist.push_back(ii->first);
  }

  // finally, deliver the msg to all of them via send_msg calls
  for (int x=0; x<targetlist.size(); ++x)
    send_msg(src, targetlist[x], buf);
}

void local_agent::recv_msg(const uuid_query &srcq, scopira::tool::uuid &foundsrc, scopira::tool::uuid dest,
  scopira::tool::count_ptr<scopira::tool::bufferflow> &buf)
{
  count_ptr<process_t> ps;

  buf = 0;

  assert(!dest.is_zero());

  get_ps(dest, ps);

  if (!ps.get())
    return;   // not found!

  while (true) {
    event_ptr<process_t::state_area> LL(ps->pm_state);

    for (msglist_t::iterator ii = LL->pm_msgqueue.begin(); ii != LL->pm_msgqueue.end(); ++ii)
      if (srcq.query(ii->pm_src)) {
        // found a msg!
        foundsrc = ii->pm_src;
        buf = ii->pm_buf;
        LL->pm_msgqueue.erase(ii);

        LL.notify_all();

        return;
      }

    // indefinate wait
    LL.wait();
  }
}

bool local_agent::la_launch_task(int myindex, const narray<uuid> &taskids, const std::string &typestring, short phase)
{
  count_ptr<process_t> p;
  count_ptr<agent_task_i> thetask;

  if (phase == 2) {
    // ok, just do phase 2 (enable our already created event)
    {
      write_locker_ptr<ps_area> L(dm_psarea);

      assert(L->pm_ps.find(taskids[myindex]) != L->pm_ps.end());

      event_ptr<process_t::state_area> PL(L->pm_ps[taskids[myindex]]->pm_state);

      PL->pm_mode = process_t::ps_ready_c;
    }

    set_min_threads();

    return true;
  }

  assert(phase == 1);

  assert(objflowloader::instance()->has_typeinfo(typestring) && "[unkown type to la_launch_task]");

  object *o = objflowloader::instance()->load_object(objflowloader::instance()->get_typeinfo(typestring));
  assert(o);
  assert(dynamic_cast<agent_task_i*>(o));
  thetask = dynamic_cast<agent_task_i*>(o);
  assert(thetask.get());

  p = new process_t(myindex, taskids);

  p->load_services(objflowloader::instance()->get_typeinfo(typestring));
  p->pm_task = thetask;

  {
    event_ptr<process_t::state_area> LL(p->pm_state);

    LL->pm_mode = process_t::ps_born_c;
  }

  // insert the processes
  {
    write_locker_ptr<ps_area> L(dm_psarea);

    L->pm_ps[taskids[myindex]] = p;
  }

  set_min_threads();

  return true;
}

scopira::tool::uuid local_agent::la_launch_proxy(agent_task_i *at)
{
  count_ptr<process_t> p;
  uuid theid;
  narray<uuid> tasklist;

  assert(at && at->is_alive_object());

  dm_uugen.next(theid);
  tasklist.resize(1);
  tasklist[0] = theid;
  p = new process_t(0, tasklist);

  p->pm_task = at;

  {
    event_ptr<process_t::state_area> LL(p->pm_state);

    LL->pm_mode = process_t::ps_ready_c;
  }

  // insert the processes
  {
    write_locker_ptr<ps_area> L(dm_psarea);

    L->pm_ps[theid] = p;
  }

  set_min_threads();

  return theid;
}

void local_agent::la_update_slave_master(scopira::tool::uuid master,
    narray<uuid> &peers)
{
  read_locker_ptr<ps_area> L(dm_psarea);
  count_ptr<process_t> p;

  assert(L->pm_ps.find(master) != L->pm_ps.end());

  p = L->pm_ps.find(master)->second.get();

  assert(p.get());
  // update its info to reflect its new, master status
  assert(p->pm_index == 0);
  assert(p->pm_peers.size() == 1);
  p->pm_peers = peers;
}

void local_agent::la_print_status(void)
{
  read_locker_ptr<ps_area> L(dm_psarea);
  int x=0;

  for (psmap_t::const_iterator ii=L->pm_ps.begin(); ii != L->pm_ps.end(); ++ii) {
    OUTPUT << x << ": " << ii->first << '\n';
    ++x;
  }
}

void* local_agent::worker_func(void *data)
{
  local_agent *here = reinterpret_cast<local_agent*>(data);
  bool alive, stillrun;
  count_ptr<process_t> ps;
  int retcode;

  assert(here);
  assert(here->is_alive_object());

  // worker main loop
  alive = true;
  while (alive) {
    ps = 0;

    {
      event_ptr<kernel_area> L(here->dm_kernel);
      psmap_t::const_iterator nextii, curii;

      while (alive && ps.is_null()) {
        if (!L->pm_alive) {
          alive = false;   // best die, i guess
          break;
        }

				{
					read_locker_ptr<ps_area> PS(here->dm_psarea);

					// grab the next job, if any
					if (L->pm_nextps.is_zero())
						nextii = PS->pm_ps.begin();
					else {
						nextii = PS->pm_ps.find(L->pm_nextps);
						if (nextii == PS->pm_ps.end())
							nextii = PS->pm_ps.begin();
					}

					// ok, so nextii should be setup, lets find the next ps to process
					if (nextii != PS->pm_ps.end()) {
						curii = nextii;
						// find the next availabe process to run
						while (true) {
							count_ptr<process_t> subps;

							subps = curii->second;
							assert(subps.get());
							{
								event_ptr<process_t::state_area> PL(subps->pm_state);

								if (
										// ready to run
										PL->pm_mode == process_t::ps_ready_c ||
										// sleeping on a msg, and a msg comes
										(PL->pm_mode == process_t::ps_sleep_c && PL->pm_onmsg && !PL->pm_msgqueue.empty()) ||
										// sleeping on a timeout, and that time comes
										(PL->pm_mode == process_t::ps_sleep_c && PL->pm_ontime != 0 && here->dm_agentclock.get_running_time() >= PL->pm_ontime) ||
										// to be killed
										(PL->pm_mode == process_t::ps_sleep_c && PL->pm_killreq)
									 ) {
									// BINGO, we found a process we can run
									PL->pm_mode = process_t::ps_running_c;
									ps = subps;
									// set the pm_nextps
									++curii;
									if (curii == PS->pm_ps.end())
										L->pm_nextps = uuid();
									else
										L->pm_nextps = curii->first;
									break;
								}
							}// state area lock

							// not a runnable one? increment
							++curii;
							if (curii == PS->pm_ps.end())
								curii = PS->pm_ps.begin();

							// have we looped back?
							if (curii == nextii)
								break;  // done, nothing found
						}// while (true) (hunt for runnable)
					}//if-hunt
				}//read_locker_ptr<ps_area>

        if (ps.is_null())
          L.wait(1000);   // gotta wait for something to do (need the timeout to constantly check of time-sleep jobs)
      }//while(alive && ps is null)
    }//event_ptr L

    if (!alive)
      break;

    // run the job, if any
    retcode = 0;
    if (ps.get()) {
      task_context ctx(ps->pm_index, ps->pm_peers);
      assert(!ps->pm_id.is_zero());

      stillrun = !event_ptr<process_t::state_area>(ps->pm_state)->pm_killreq;
      while (stillrun) {
        double timedelta = 0;
        // run the thing
        //OUTPUT << "<\033[33mrun\033[0m>\n";
        retcode = ps->pm_task->run(ctx);
        //OUTPUT << "</\033[33mrun\033[0m>\n";

        stillrun = (retcode & 0xFF) == agent_task_i::run_again_0_c;

        event_ptr<process_t::state_area> LL(ps->pm_state);

        // decompose their return code into flags within their process struct
        LL->pm_canmove = (retcode & agent_task_i::run_canmove_c) != 0;
        LL->pm_onmsg = (retcode & agent_task_i::run_onmsg_c) != 0;

        // do the run nxet time
        switch (retcode & 0xFF) { // lower 8 bits are for run next
          case agent_task_i::run_again_1_c: timedelta = 1; break;
          case agent_task_i::run_again_10_c: timedelta = 10; break;
          case agent_task_i::run_again_100_c: timedelta = 100; break;
        }
        if (timedelta == 0)
          LL->pm_ontime = 0;
        else
          LL->pm_ontime = here->dm_agentclock.get_running_time() + timedelta;

        // check for kill signals
        if (stillrun && LL->pm_killreq)
          stillrun = false;
      }//while stillrun

      // check if we need to sleep this process
      if (retcode != 0) {
        // non-done ret code, yet stillrun is false, therefore: sleep
        event_ptr<process_t::state_area> LL(ps->pm_state);

        LL->pm_mode = process_t::ps_sleep_c;
      } else {
        // terminate the job
        // detach it from the map
        {
          write_locker_ptr<ps_area> L(here->dm_psarea);

          assert(L->pm_ps.find(ps->pm_id) != L->pm_ps.end());
          L->pm_ps.erase(L->pm_ps.find(ps->pm_id));
        }
        // signal its destruction
        {
          event_ptr<process_t::state_area> LL(ps->pm_state);

          LL->pm_mode = process_t::ps_done_c;
        }

        ps->pm_state.pm_condition.notify_all();

        // finally, upstream this to any decendants
        here->la_dead_task(ps->pm_id);
      }//end of else

    }//if ps.get()
  }//while alive (big loop)

  return 0;
}

void local_agent::get_ps(scopira::tool::uuid id, scopira::tool::count_ptr<process_t> &foundps) const
{
  read_locker_ptr<ps_area> L(dm_psarea);
  psmap_t::const_iterator ii;

  ii = L->pm_ps.find(id);

  if (ii == L->pm_ps.end()) {
    foundps = 0;
    return;
  }

  foundps = ii->second;
}

void local_agent::set_min_threads(void)
{
  size_t newnumt = 0;

	{
		read_locker_ptr<ps_area> L(dm_psarea);
  	newnumt = L->pm_ps.size();
	}

  event_ptr<kernel_area> L(dm_kernel);

  // just using pm_ps->size() is simplest.
  // trying to be tricky (and using numbers small with this) had unforseen issues.

  while (L->pm_threads.size() < newnumt) {
    thread *t;
    L->pm_threads.push_back(t = new thread(worker_func, this));
    t->start();
  }

  dm_kernel.pm_condition.notify_all();
}

void local_agent::kill_all_local_tasks(void)
{
  typedef std::vector<count_ptr<process_t> > pslist_t;
  pslist_t pslist;
  pslist_t::iterator jj;

  // really paranoid here
  // we're doing this in two completely seperate locking areas to avoid deadlocks and race conditions
  // even though it means we have to use a temporary, internal list of processes

  // find the process and set its kill flag
  {
    read_locker_ptr<ps_area> L(dm_psarea);
    psmap_t::const_iterator ii;

    pslist.resize(L->pm_ps.size());
    jj = pslist.begin();

    for (ii = L->pm_ps.begin(); ii != L->pm_ps.end(); ++ii, ++jj)
      *jj = ii->second;
  }

  // set the kill flag for all the processes
  for (jj=pslist.begin(); jj != pslist.end(); ++jj) {
    event_ptr<process_t::state_area> L((*jj)->pm_state);

    if (L->pm_mode != process_t::ps_done_c)
      L->pm_killreq = true;

    // notify everyone
    L.notify_all();
  }
}

void local_agent::process_t::load_services(const std::type_info &nfo)
{
  service_registrar *note = service_registrar::instance();
  std::vector<uuid> serv;

  if (!note)
    return; // nothing to do
  note->get_service(objflowloader::instance()->get_name(nfo), serv);
  for (int x=0; x<serv.size(); ++x)
    pm_services.insert(serv[x]);
}



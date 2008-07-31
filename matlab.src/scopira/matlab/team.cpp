
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

#include <scopira/matlab/team.h>

#include <scopira/tool/output.h>
#include <scopira/tool/uuid.h>
#include <scopira/core/register.h>
#include <scopira/agent/context.h>

//BBtargets libscopiramatlab.so
//BBlibs scopira

using namespace scopira::tool;
using namespace scopira::agent;
using namespace scopira::matlab;

static scopira::core::register_flow<scopira::matlab::job_server> r1("scopira::matlab::job_server");
static scopira::core::register_flow<scopira::matlab::query_server> r2("scopira::matlab::query_server");

scopira::tool::uuid scopira::matlab::job_service("c8fcad89-d1eb-4353-8755-a88ef9018426");

scopira::matlab::link_loop::singleton_job_client *scopira::matlab::link_loop::singleton_job_client::dm_instance;

//
//
// job_server
//
//

int scopira::matlab::job_server::run(scopira::agent::task_context &ctx)
{
  OUTPUT << "TeamServer started, uuid=" << ctx.get_id() << "\n";

  ctx.enable_service(job_service);

  uuid src;
  int cmd;

  while (true) {
    recv_msg RECV(ctx, uuid());

    if (!RECV.read_int(cmd))
      continue;
    src = RECV.get_source();
    OUTPUT << "TeamServer processing cmd=" << cmd << " uuid=" << src << '\n';

    switch (cmd) {
      // send the ping back
      case 1: {
          send_msg M(ctx, src);

          M.write_int(1);
          break;
        }
      case 10: {
          std::string teamname;
          int teamsz = 0;
          bool good;

          good = RECV.read_string(teamname) && RECV.read_int(teamsz) && teamsz>1;

          if (!good) {
            send_msg M(ctx, src);
            M.write_int(1);   // fail em now
            break;
          }

          // insert the new job (or overwrite an existing, whatever)
          job_team &jg = dm_jobs[teamname];

          jg.pm_peers.resize(teamsz);
          jg.pm_peers[0] = src;
          // we reply to all when the last job has joined
          break;
        }
      case 11: {
          std::string teamname;
          bool good;
          int nextindex;

          good = RECV.read_string(teamname);

          if (good)
            good = dm_jobs.find(teamname) != dm_jobs.end();

          if (!good) {
            send_msg M(ctx, src);
            M.write_int(1);   // fail em now
            break;
          }

          // insert the new job
          job_team &jg = dm_jobs[teamname];

          // find the next open slot in this team
          for (nextindex = 0; nextindex<jg.pm_peers.size(); ++nextindex)
            if (jg.pm_peers[nextindex].is_zero())
              break;  // found one

          if (nextindex == jg.pm_peers.size()) {
            send_msg M(ctx, src);
            M.write_int(1);   // fail em now, no room left in this team
            break;
          }

          jg.pm_peers[nextindex] = src;

          // was this the last position? if so, bcast everyone the success
          if (nextindex + 1 == jg.pm_peers.size()) {
            OUTPUT << "TeamServer team-complete: " << teamname << '\n';
            for (int i=0; i<jg.pm_peers.size(); ++i) {
              send_msg M(ctx, jg.pm_peers[i]);

              M.write_int(15);
              M.write_int(i);
              M.write_int(jg.pm_peers.size());
              for (int j=0; j<jg.pm_peers.size(); ++j)
                jg.pm_peers[j].save(M);
            }//for i
          }//if nextindex + 1
          break;
        }//case 11
      case 9999:
        return run_done_c;
    }//switch
  }//while true

  OUTPUT << "TeamServer done\n";

  return run_done_c;
}

//
//
// query_server
//
//

int scopira::matlab::query_server::run(scopira::agent::task_context &ctx)
{
  int cmd = 0;
  uuid server;

  {
    send_msg M(ctx, service_broadcast(job_service));
    M.write_int(1);
  }

  if (!ctx.wait_msg(uuid(), 3000)) {
    OUTPUT << "TeamServer not found in time, quiting..\n";
    return run_done_c;
  }

  // ok, receive the msg
  {
    recv_msg M(ctx, uuid());

    M.read_int(cmd);

    server = M.get_source();
  }

  if (cmd == 1)
    OUTPUT << "TeamServer found @" << server << '\n';

  return run_done_c;
}

//
//
// job_client
//
//


job_client::job_client(void)
{
  dm_myindex = 0;
  dm_madeserver = false;
}

job_client::~job_client(void)
{
  if (dm_server.is_zero())
    return;
  
  // we're connected to the server, send them the quit message
  send_msg M(dm_ctx, dm_server);
  M.write_int(dm_madeserver ? 9999 : 99);   // 9999 tells the server to die
}

bool job_client::make_team(const std::string &teamname, int teamsz)
{
  assert(dm_server.is_zero());
  // find the manager
  {
    send_msg M(dm_ctx, service_broadcast(job_service));
    M.write_int(1);
  }

  if (!dm_ctx.wait_msg(uuid(), 1000)) {
    dm_server = dm_ctx.launch_task("scopira::matlab::job_server");
    OUTPUT << "TeamServer not found, created @" << dm_server << '\n';
    dm_madeserver = true;
  } else {
    // ok, receive the msg
    recv_msg M(dm_ctx, uuid());
    int x;

    M.read_int(x);

    if (x != 1)
      return false;// what the

    dm_server = M.get_source();
    OUTPUT << "TeamServer found @" << dm_server << '\n';
  }

  // send them the make team request
  {
    send_msg M(dm_ctx, dm_server);

    M.write_int(10);
    M.write_string(teamname);
    M.write_int(teamsz);
  }

  {
    recv_msg M(dm_ctx, dm_server);
    int x;

    M.read_int(x);

    if (x != 15)
      return false;// what the... can't do it?
    // ok, team created, lets parse the rest of the info
    M.read_int(dm_myindex);
    M.read_int(x);

    if (x<=0 || dm_myindex<0 || dm_myindex>=x)
      return false;

    dm_peers.resize(x);
    OUTPUT << "Successfully joined a team of size=" << x << '\n';

    for (int i=0; i<x; ++i)
      dm_peers[i].load(M);
  }

  return true;
}

bool job_client::join_team(const std::string &teamname)
{
  assert(dm_server.is_zero());
  // find the manager
  {
    send_msg M(dm_ctx, service_broadcast(job_service));
    M.write_int(1);
  }

  if (!dm_ctx.wait_msg(uuid(), 3000)) {
    OUTPUT << "TeamServer not found in time, quiting..\n";
    return false;
  }

  // ok, receive the msg
  {
    recv_msg M(dm_ctx, uuid());
    int x;

    M.read_int(x);

    if (x != 1)
      return false;// what the

    dm_server = M.get_source();
    OUTPUT << "TeamServer found @" << dm_server << '\n';
  }

  // send them the make team request
  {
    send_msg M(dm_ctx, dm_server);

    M.write_int(11);
    M.write_string(teamname);
  }

  {
    recv_msg M(dm_ctx, dm_server);
    int x;

    M.read_int(x);

    if (x != 15)
      return false;// what the... can't do it?
    // ok, team created, lets parse the rest of the info
    M.read_int(dm_myindex);
    M.read_int(x);

    if (x<=0 || dm_myindex<0 || dm_myindex>=x)
      return false;

    dm_peers.resize(x);
    OUTPUT << "Successfully joined a team of size=" << x << '\n';

    for (int i=0; i<x; ++i)
      dm_peers[i].load(M);
  }

  return true;
}

//
//
// link_loop::singleton_job_client
//
//

link_loop::singleton_job_client::singleton_job_client(void)
{
  assert(dm_instance == 0);
  dm_instance = this;
}

link_loop::singleton_job_client::~singleton_job_client()
{
  assert(dm_instance == this);
  dm_instance = 0;
}


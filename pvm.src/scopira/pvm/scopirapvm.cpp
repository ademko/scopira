
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

#include <scopira/pvm/loop.h>
#include <scopira/pvm/util.h>
#include <scopira/pvm/send.h>

#include <assert.h>

//BBlibs pvm scopira raygun
//BBtargets libscopirapvm.so

using namespace scopira::pvm;

//
//
// pvm_session
//
//

pvm_session * pvm_session::dm_instance;   // just to make sure there is only one session, ever

pvm_session::pvm_session(void)
{
  dm_failed = dm_instance != 0;
  if (dm_failed)
    return;

  dm_instance = this;

  // should this do the pvm_loop stuff or not (not really needed?)
  pvm_mytid();
}

pvm_session::~pvm_session()
{
  if (dm_failed)
    return;

  dm_instance = 0;

  //this _seems_ to be ok... one would think that calling pvm_exit could only be done
  //once per program, but aparently callin this, and then pvm_mytid later is OK
  pvm_exit();
}

//
//
// pvm_loop
//
//

pvm_loop * pvm_loop::dm_instance;

pvm_loop::pvm_loop(void)
{
  assert(dm_instance == 0);
  dm_instance = this;

  pvm_mytid();
}

pvm_loop::~pvm_loop()
{
  assert(dm_instance == this);
  dm_instance = 0;

  pvm_exit();
}

//
//
// util.h
//
//

int scopira::pvm::spawn_one(const std::string &fullfilename)
{
  int ret, info;

  info = pvm_spawn(const_cast<char*>(fullfilename.c_str()), 0, PvmTaskDefault, 0, 1, &ret);

  if (info < 1)
    return 0;
  else
    return ret;
}

void scopira::pvm::spawn_many(const std::string &fullfilename, int numtasks, scopira::basekit::narray<int> &out)
{
  assert(numtasks>0);
  out.resize(numtasks);
  int info;

  info = pvm_spawn(const_cast<char*>(fullfilename.c_str()), 0, PvmTaskDefault, 0, numtasks, out.c_array());

  if (info < numtasks)
    out.resize(0);
}

int scopira::pvm::find_index(const scopira::basekit::narray<int> &peers, int tid)
{
  for (int x=0; x<peers.size(); ++x)
    if (peers[x] == tid)
      return x;

  return -1;
}

int scopira::pvm::default_group_size(void)
{
  int numhost, numarch;
  pvmhostinfo *p;

  pvm_config(&numhost, &numarch, &p);

  return numhost;
}

//
//
// send.h
//
//

bool scopira::pvm::probe_pack(int srctaskid, int msgtag, int *outtaskid, int *outmsgtag)
{
  int buf;
  struct timeval vl;

  // very similar to probe_pack_timeout
  vl.tv_sec = 0;
  vl.tv_usec = 0;
  buf = pvm_trecv(srctaskid, msgtag, &vl);

  if (buf <= 0)
    return false;

  int b, m, t;
  pvm_bufinfo(buf, &b, &m, &t);

  if (outtaskid)
    *outtaskid = t;
  if (outmsgtag)
    *outmsgtag = m;

  return true;
}

bool scopira::pvm::probe_pack_timeout(int timeout, int srctaskid, int msgtag, int *outtaskid, int *outmsgtag)
{
  int buf;
  struct timeval vl;

  assert(timeout >= 0);

  vl.tv_sec = timeout/1000;
  vl.tv_usec = timeout%1000 * 1000;
  buf = pvm_trecv(srctaskid, msgtag, timeout>0 ? &vl : 0);

  if (buf <= 0)
    return false;

  int b, m, t;
  pvm_bufinfo(buf, &b, &m, &t);

  if (outtaskid)
    *outtaskid = t;
  if (outmsgtag)
    *outmsgtag = m;

  return true;
}

//
//
// send_pack
//
//

send_pack::send_pack(int desttaskid, int msgtag)
  : scopira::tool::bin64oflow(false, this), dm_desttaskid(desttaskid), dm_msgtag(msgtag)
{
  pvm_initsend(PvmDataRaw);
}

send_pack::~send_pack()
{
  pvm_send(dm_desttaskid, dm_msgtag);
}

size_t send_pack::write(const byte_t *_buf, size_t _size)
{
  pvm_pkbyte(reinterpret_cast<char*>(const_cast<byte_t*>(_buf)), _size, 1);

  return _size;
}

//
//
// recv_pack
//
//

recv_pack::recv_pack(void)
  : scopira::tool::bin64iflow(false, this)
{
  // nothing needed, as probe_pack_timeout already did the receive
  dm_srctaskid = 0;
  dm_msgtag = 0;
}

recv_pack::recv_pack(int srctaskid, int msgtag)
  : scopira::tool::bin64iflow(false, this)
{
  int buf, b;

  buf = pvm_recv(srctaskid, msgtag);
  pvm_bufinfo(buf, &b, &dm_msgtag, &dm_srctaskid);
}

recv_pack::~recv_pack()
{
}

size_t recv_pack::read(byte_t *_buf, size_t _maxsize)
{
  pvm_upkbyte(reinterpret_cast<char*>(_buf), _maxsize, 1);

  return _maxsize; // cant we get the exact number out of pvm_upkbyte or something?
}



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

#include <pakit/util.h>

//BBlibs scopira
//BBtargets libpakit.so

#include <scopira/tool/stringflow.h>
#include <scopira/tool/printflow.h>
#include <scopira/tool/fileflow.h>
#include <scopira/tool/cacheflow.h>
#include <scopira/tool/sort_imp.h>
#include <scopira/basekit/vectormath.h>

using namespace scopira::tool;
using namespace scopira::basekit;
using namespace pakit;

//
//
// ibdtext_io_alg
//
//

ibdtext_io_alg::ibdtext_io_alg(void)
  : dm_type(variable_labels_c)
{
  dm_swaptrain = false;
  dm_trainonly = false;
}

bool ibdtext_io_alg::load_file(std::string &title, pakit::patterns_t &outpat)
{
  int x, y, outidx, numpat, numtrain, numtest, patlen;
  bool vartype;
  fileflow *fileF;
  stringiflow sflow(0);
  printiflow fin(true,0), lin(false,0);
  std::string str, label;

  title.reserve(256);
  str.reserve(64*1024);
  label.reserve(256);

  vartype = dm_type == 1;

  fin.open(new cacheiflow(true, fileF = new fileflow(dm_filename, fileflow::input_c)));

  if (fileF->failed()) {
    dm_error = "Failed to open file for reading: ";
    dm_error += dm_filename;
    return false;
  }

  lin.open(&sflow);

  // read the title line (line one)
  fin.read_string(title);
  outpat.pm_comment = title;
  
  // read header
  // line two
  fin.read_string(str);
  sflow.open(&str);
  lin.read_int(numpat);
  lin.read_int(numtrain);

  // line three
  fin.read_string(str);
  sflow.open(&str);  
  lin.read_int(patlen);
  lin.read_int(y);   // read numclass into a dummy var

  // proper checking
  numtest = numpat - numtrain;

  if (numpat <= 0 || numtrain < 0 || patlen <= 0 || y < 0 || numtest < 0) {
    dm_error = "File is in incorrect format: ";
    dm_error += dm_filename;
    return false;
  }

  // alloc output stuff
  if ( dm_trainonly ) {
    if ( dm_swaptrain ) {
      outpat.pm_labels.ref().resize(numtest);
      outpat.pm_data.ref().resize(patlen, numtest);
      outpat.pm_classes.ref().resize(numtest);
      outpat.pm_training.ref().resize(numtest);
    }
    else {
      outpat.pm_labels.ref().resize(numtrain);
      outpat.pm_data.ref().resize(patlen, numtrain);
      outpat.pm_classes.ref().resize(numtrain);
      outpat.pm_training.ref().resize(numtrain);
    }
  }
  else {
    outpat.pm_labels.ref().resize(numpat);
    outpat.pm_data.ref().resize(patlen, numpat);
    outpat.pm_classes.ref().resize(numpat);
    outpat.pm_training.ref().resize(numpat);
  }

  for (y=0, outidx = 0; y<numpat && !fin.failed(); y++) {

    if ( dm_trainonly &&
         ( ( !dm_swaptrain && y>=numtrain ) ||
           ( dm_swaptrain && y<numtrain ) ) ) {

      // Go through the motions, but don't put this data into the output buffer
      fin.read_string(str);
      sflow.open(&str);
      if (vartype)
        lin.read_string_word(label);
      else
        lin.read_string(label,19);
      for (x=0; x<patlen; x++) {
        double d;
        lin.read_double(d);
      }
      int iii;
      lin.read_int(iii);

      continue;
    }

    fin.read_string(str);
    sflow.open(&str);

    // read label
    if (vartype)
      lin.read_string_word(label);
    else
      lin.read_string(label, 19);
    outpat.pm_labels.ref()[outidx] = label;
    //fprintf(stderr,"<%s>", label.c_str());

    // read feature data
    for (x=0; x<patlen; x++) {
      double d;
      lin.read_double(d);
      outpat.pm_data.ref()(x,outidx) = d;
      //fprintf(stderr,"[%f]", lin.read_double());
    }

    // read class
    //fprintf(stderr,"<%d>\n", lin.read_int());
    int iii;
    lin.read_int(iii);
    outpat.pm_classes.ref()[outidx] = iii;

    // set training
    if (dm_swaptrain)
      outpat.pm_training.ref()(outidx) = y>=numtrain;
    else
      outpat.pm_training.ref()(outidx) = y<numtrain;
    outidx++;
  }

  lin.close();
  fin.close();

  return true;
}

bool ibdtext_io_alg::save_file(const std::string &title, const pakit::patterns_t &inpat)
{
  size_t x, y, numtrain;
  int numclass;
  printoflow fout(true,0);
  count_ptr<narray_o<double,2> > _data;
  count_ptr<stringvector_o> _labels;
  count_ptr<narray_o<int,1> > _classes;
  count_ptr<narray_o<bool,1> > _training;

  _data = inpat.pm_data;

  if (_data.is_null() || _data->empty()) {
    dm_error = "Feature matrix is empty";
    return false;
  }

  _classes = inpat.pm_classes;
  if (_classes.is_null()) {
    _classes = new narray_o<int,1>(_data->height());
    _classes->set_all(1);
    //inpat.pm_classes = _classes.get();
  }
  if (_classes->empty()) {
    dm_error = "Class vector is empty";
    return false;
  }

  _training = inpat.pm_training;
  if (_training.is_null()) {
    _training = new narray_o<bool,1>(_data->height());
    _training->set_all(true);
    //training = _training.get();
  }
  if (_training->empty()) {
    dm_error = "Training vector is empty";
    return false;
  }

  if (_data->height() != _classes->size()) {
    dm_error = "Class vector does not match the feature matrix";
    return false;
  }

  _labels = inpat.pm_labels;
  if (_labels.get() && _data->height() != _labels->size()) {
    dm_error =  "Given string label vector does not match the feature matrix";
    return false;
  }

  // count classes and training
  numtrain = 0;
  numclass = 0;
  for (x=0; x<_training->size(); x++) {
    if (_training->get(x)) {
      numtrain++;
      if (_classes->get(x) > numclass)
        numclass = _classes->get(x);
    }
  }

  // open the file
  fout.open(new cacheoflow(true, new fileflow(dm_filename, fileflow::output_c | fileflow::trunc_c)));

  if (fout.failed()) {
    dm_error = "Failed to open file for writing: ";
    dm_error += dm_filename;
    return false;
  }

  // write it out
  if (title.empty())
    fout.write_string(dm_filename);
  else
    fout.write_string(title);

  fout.printf("\n%d %d\n%d %d\n", _data->height(), numtrain, _data->width(), numclass);

  // Two passes - first for _training, second for test
  for (y=0; y<_data->height(); y++)
    if (_training->get(y)) {
      std::string ti = _labels.get() ?_labels->get(y):int_to_string(y);
      fout.write_string(pad_left(ti, 19));

      for (x=0; x<_data->width(); x++)
        //fout.printf(" %6.6f", _data->get(x, y));
        fout << " " << _data->get(x, y);

      fout.printf(" %d\n", _classes->get(y));
    }

  for (y=0; y<_data->height(); y++)
    if (!_training->get(y)) {
      std::string ti =  _labels.get() ? _labels->get(y):int_to_string(y);
      fout.write_string(pad_left(ti, 19));

      for (x=0; x<_data->width(); x++)
        //fout.printf(" %6.6f", _data->get(x, y));
        fout << " " << _data->get(x, y);

      fout.printf(" %d\n", _classes->get(y));
    }

  return true;
}

sort_data_alg::sort_data_alg(void)
  : patterns_t(false)
{
}

bool sort_data_alg::is_sorted_by_class(void) const
{
  narray<int,1> &classes = pm_classes.ref();

  if (classes.empty())
    return true;    // empty is sorted, I guess

  int x, last;
  
  last = 1;

  for (x=0; x<classes.size(); ++x)
    if (classes[x]>last)
      last = classes[x];
    else
      if (classes[x]>0 && classes[x]<last)
        return false;   // found a failed case

  return true;
}

class sort_data_alg::class_sort_view
{
  private:
    const patterns_t &dm_parent;
    bool dm_sort_by_class;  // sort by training if this is false
  public:
    class_sort_view(bool sorttype, const patterns_t &parent) : dm_parent(parent), dm_sort_by_class(sorttype) { }
    int compare_element(int lidx, int ridx) const {
      if (dm_sort_by_class) {
        if (dm_parent.pm_classes.ref()[lidx] < dm_parent.pm_classes.ref()[ridx])
          return -1;
        else
          if (dm_parent.pm_classes.ref()[lidx] > dm_parent.pm_classes.ref()[ridx])
            return 1;
          else
            return 0;
      } else {
        if (dm_parent.pm_training.ref()[lidx] == dm_parent.pm_training.ref()[ridx])
          return 0;
        else
          return dm_parent.pm_training.ref()[lidx] ? -1 : 1;
      }
    }
    void swap_element(int lidx, int ridx) const;
};

void sort_data_alg::class_sort_view::swap_element(int lidx, int ridx) const
{
  std::string s = dm_parent.pm_labels->get(lidx);
  int cl = dm_parent.pm_classes->get(lidx);
  bool train = dm_parent.pm_training->get(lidx);
  narray<double> v;

  v.copy(dm_parent.pm_data->row_slice(lidx));

  dm_parent.pm_labels->set(lidx, dm_parent.pm_labels->get(ridx));
  dm_parent.pm_classes.ref()[lidx] = dm_parent.pm_classes->get(ridx);
  dm_parent.pm_training.ref()[lidx] = dm_parent.pm_training->get(ridx);
  dm_parent.pm_data->row_slice(lidx).copy(
      dm_parent.pm_data->row_slice(ridx));

  dm_parent.pm_labels->set(ridx, s);
  dm_parent.pm_classes.ref()[ridx] = cl;
  dm_parent.pm_training.ref()[ridx] = train;
  dm_parent.pm_data->row_slice(ridx).copy(v);
}

void sort_data_alg::sort_by_class(void)
{
  narray<int,1> &classes = pm_classes.ref();

  if (classes.size() <= 1)
    return;

  assert(classes.size() == pm_data->height());
  assert(classes.size() == pm_labels->size());
  assert(classes.size() == pm_training->size());

  class_sort_view cv(true, *this);
  scopira::tool::qsort(cv, 0, classes.size()-1);
}

void sort_data_alg::sort_by_training(void)
{
  // classes and training are the same length, doesnt matter which
  // we use, so we might aswell CnP code from sort_by_class
  narray<int,1> &classes = pm_classes.ref();

  if (classes.size() <= 1)
    return;

  assert(classes.size() == pm_data->height());
  assert(classes.size() == pm_labels->size());
  assert(classes.size() == pm_training->size());

  class_sort_view cv(false, *this);
  scopira::tool::qsort(cv, 0, classes.size()-1);
}

bool sort_data_alg::is_sorted_by_training(void) const
{
  narray<bool,1> &training = pm_training.ref();
  int x;

  for (x=0; x<training.size(); ++x)
    if (!training[x])
      break;
  for (; x<training.size(); ++x)
    if (training[x])
      break;

  return x == training.size();
}

bool sort_data_alg::class_stats(int *min, int *max, int *minimin) const
{
  narray<int,1> &classes = pm_classes.ref();

  if (classes.empty())
    return false;

  if (min)
    *min = classes[0];
  if (max)
    *max = classes[0];
  if (minimin)
    *minimin = classes[0];

  for (int x=1; x<classes.size(); ++x) {
    int val = classes[x];
    if (min && val < *min)
      *min = val;
    if (max && val > *max)
      *max = val;
    if (minimin && (
          (*minimin<1 && val>=1) ||
          (*minimin<1 && val<1 && val<*minimin) ||
          (*minimin>=1 && val>=1 && val<*minimin)
          ) )
      *minimin = val;
  }

  return true;
}

void sort_data_alg::add_centroids(bool super, bool perclass, pakit::patterns_t &output) const
{
  int minclass, maxclass, numclass, basey, klass, y;
  pakit::patterns_t inpats(true);

  // Any centroids or placeholders left over from a previous run?  
  // Be sure to clobber them!
  size_t newsize = 0;
  for (int pidx = 0; pidx<pm_data->height(); ++pidx)
    if ( ( pm_classes->get(pidx) > 0 ) ||
         ( pm_labels->get(pidx).find( /*C*/"entroid" ) != std::string::npos &&
           pm_labels->get(pidx).find( /*P*/"laceholder" ) != std:: string::npos ) )
      newsize++;

  inpats.pm_labels->resize(newsize);
  inpats.pm_data->resize(pm_data->width(), newsize);
  inpats.pm_classes->resize(newsize);
  inpats.pm_training->resize(newsize);
  assert(newsize>0);

  for ( int pidx = 0, validpidx = 0; pidx < pm_data->height(); ++pidx ) {
    if ( ( pm_classes->get(pidx) > 0 ) ||
         ( pm_labels->get(pidx).find( /*C*/"entroid" ) != std::string::npos &&
           pm_labels->get(pidx).find( /*P*/"laceholder" ) != std:: string::npos ) ) {
      inpats.pm_data->xslice(0, validpidx, pm_data->width()).copy(
          pm_data->xslice(0, pidx, pm_data->width()));
      inpats.pm_classes->set(validpidx, pm_classes->get(pidx));
      inpats.pm_training->set(validpidx, pm_training->get(pidx));
      inpats.pm_labels->set(validpidx, pm_labels->get(pidx));
      validpidx++;
    }
  }

  if (super)
    ++newsize;

  numclass = 0;
  if (perclass) {
    class_stats(0, &maxclass, &minclass);
    if (minclass>=1)
      numclass = maxclass - minclass + 1;
    else
      numclass = maxclass;
    newsize += numclass;
  }

  // prepare the output data
  output.pm_labels->resize(newsize);
  output.pm_data->resize(pm_data->width(), newsize);
  output.pm_classes->resize(newsize);
  output.pm_training->resize(newsize);

  basey = 0;

  // do global centroid, if any
  if (super) {
    output.pm_labels->set(basey, "centroid");
    calc_global_centroid(inpats.pm_data->all_slice(), output.pm_data->row_slice(basey));
    output.pm_classes.ref()[0] = 0;
    output.pm_training.ref()[0] = false;
    ++basey;
  }

  // go through all the data, calculating centroids
  intvec_o numinclass;
  numinclass.resize(numclass);
  numinclass.clear();
  doublematrix_o sumclass;
  sumclass.resize(numclass,inpats.pm_data->width());
  sumclass.clear();
  for (klass = 0; klass<numclass; ++klass) {
    for ( int pidx = 0; pidx < inpats.pm_data->height(); pidx++ ) {
      if ( inpats.pm_classes->get(pidx) == klass+1 ) {
        numinclass[klass]++;
        for ( int aidx = 0; aidx < inpats.pm_data->width(); aidx++ ) {
          sumclass.set( klass, aidx, sumclass.get( klass, aidx ) + inpats.pm_data->get( aidx, pidx ) );
        }
      }
    }
    y = basey + klass;
    // If no samples associated with class, it's a placeholder.
    // Use the global centroid (recompute here in case it wasn't computed above)
    if ( numinclass[klass] == 0 ) {
      output.pm_labels->set(y, "placeholder_" + int_to_string(klass+1));
      calc_global_centroid(inpats.pm_data->all_slice(), output.pm_data->row_slice(y));
    }
    else {
      output.pm_labels->set(y, "centroid_" + int_to_string(klass+1));
      for ( int aidx = 0; aidx < inpats.pm_data->width(); aidx++ ) {
        output.pm_data->set( aidx, y, sumclass.get( klass, aidx ) / numinclass[klass] );
      }
    }
    output.pm_classes.ref()[y] = -(klass+1);
    output.pm_training.ref()[y] = false;
  }

  basey += numclass;

  // copy the rest of the data over
  output.pm_data->xyslice(0, basey, inpats.pm_data->width(), inpats.pm_data->height()).copy(
      inpats.pm_data->all_slice());
  output.pm_classes->xslice(basey, inpats.pm_classes->size()).copy(
      inpats.pm_classes->all_slice());
  output.pm_training->xslice(basey, inpats.pm_training->size()).copy(
      inpats.pm_training->all_slice());
  // string vec does do slicing, do it manually
  for (y=0; y<inpats.pm_labels->size(); ++y)
    output.pm_labels->set(basey + y, inpats.pm_labels->get(y));
  
  //comment
  output.pm_comment = pm_comment;
}

void sort_data_alg::calc_global_centroid(const scopira::basekit::nslice<double,2> &pts,const scopira::basekit::nslice<double> &centout)
{
  assert(pts.width() == centout.size());
  for (size_t x=0; x<centout.size(); ++x)
    mean(pts.yslice(x, 0, pts.height()), centout[x]);
}


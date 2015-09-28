#pragma once

#include <cmath>

#include <vector>
#include <map>
#include <limits>

#include "../core/general.h"
#include "basic.h"

namespace MM_NAMESPACE() {
  namespace MATH {


/* StreamingKMeans 
 * Inspired by Lloyd's 'Web-Scale K-Means Clustering' and of course LLoyd's,
 * k-means++ initial centering and finally re-interpret random sample drawing
 * to streaming ...
 * - at some point a num_members[] may overrun -> bad, zero-division!
 *   -  maybe apply some 'relaxation' reducing all proportionally? (TODO)
 * - 
 */
  template <typename T = float, 
            typename L = std::vector<T>,
            typename I = std::vector<size_t> >
  class tStreamingKMeans {
   public:
    // data representation
    typedef T value_type;
    typedef const T& const_reference;
    // consistent type for indexing
    typedef typename L::size_type idx_type;
    typedef const typename L::size_type& idx_cref;
    // storage centeroid vals
    typedef L list_type;
    typedef const L& list_cref;
    typedef typename L::const_iterator list_citer;
    // maps data-idx -> center-idx (using a vector :D)
    typedef I idx_list_type;
    typedef typename I::const_iterator idx_list_citer;

   private:
    typedef tStreamingKMeans<T, L, I> class_type;
    // keep number of 'members' for each center
    idx_list_type num_members;
    // keeps the center-points
    list_type centers;
    // batch storage
    list_type batch;
    // false, once the first data-update happend
    bool first_run;

   public:
    /* all 'config' items in public -> don't touch without reset() */
    // number of centers to be found -> 'k'
    idx_type num_centers;
    // min/max size for updating, never len(batch) < k
    idx_type min_batch_size;
    idx_type max_batch_size;  // <---- TODO!
    // fully fill batch before applying updated
    bool always_fill_batch;
    // number of update-iterations 
    idx_type num_iters;
    // number of initial seeding tries 
    idx_type num_inits;       // <---- TODO!

    /* 'k' denotes the number of clusters...  */
    tStreamingKMeans<T, L, I>(idx_cref k) : first_run(true), num_centers(k), \
        min_batch_size(k), max_batch_size(k*10), always_fill_batch(true), 
        num_iters(4), num_inits(10) {
      reset();
    }
    
    /* resetting object, config-vars remain untouched! */
    void reset() {
      num_members.clear();
      centers.clear();
      batch.clear();

      assert (num_centers > 0);
      assert (num_centers <= min_batch_size);
      assert (num_iters > 0);
      
      num_members.resize(num_centers, 0);
      centers.resize(num_centers, 0);
      first_run = true;
    }

    /* get centeroid values */
    list_cref get_centroids() const {
       return centers;
    }

    /* add single data-point */
    void add_data(const_reference data) {
       batch.push_back(data);
       update();  
    }

    /* add all data inside the container 'data' */
    void add_data(list_type data) {
       batch.reserve(batch.size() + data.size());
       batch.insert(batch.end(), data.begin(), data.end()); 
       update();
    }


    /* Updates the centroids in order to optimize towards 'k'-means */
    void update() {
      if (first_run) {
        // first_run -> fill until > min_batch_size!
        if (min_batch_size > batch.size())
          return;

        // first udpate starts now, choose centroids 'randomly' (FIXME?)
        first_run = false;
        for (idx_type i = 0; i < num_centers; ++i)
          centers[i] =
                batch.at((num_iters * min_batch_size * (i + 1)) % num_centers);

      } else if (always_fill_batch && min_batch_size > batch.size())
        return;

      idx_type best_idx = 0;
      value_type best_val, tmp_val, rate;

      // execute update 'num_iters'-times
      for (idx_type iter = 0; iter < num_iters; ++iter) {

        // over all data items inside the batch storage
        for (list_citer d_it = batch.begin(); d_it != batch.end(); ++d_it) {

          // pick the idx of the nearest centroid
          best_val = std::numeric_limits<value_type>::max();
          for (idx_type i = 0; i < num_centers; ++i) {
            tmp_val = std::abs(*d_it - centers.at(i));
            if (tmp_val < best_val) {
              best_idx = i;
              best_val = tmp_val;
            }
          }

          num_members[best_idx]++; 
          rate = (1.0f / static_cast<value_type>(num_members.at(best_idx)));
          centers[best_idx] = (1.0f - rate) * centers.at(best_idx) \
                            + (rate * (*d_it));
        } 
      } 
    }
 };


}}

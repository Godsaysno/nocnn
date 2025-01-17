#ifndef __ESTIMATION_H__
#define __ESTIMATION_H__

#include <vector>
#include "noc.h"
#include "cnn.h"

using namespace std;

// ----------------------------------------------------------------------

typedef struct LayerStat
{
  long         comm_latency;
  long         comp_latency;
  long         mmem_latency;

  double       comm_energy;
  double       comp_energy;
  double       lmem_energy;
  double       mmem_energy;

  double       lmem_energy_leakage;
  double       mmem_energy_leakage;
  double       comm_energy_leakage;
  double       comp_energy_leakage;
  
  int          active_cores;
  long         ops_per_core;
  
  long         main_memory_traffic_load;
  long         main_memory_traffic_store;
  
  void reset() {
    comm_latency = 0;
    comp_latency = 0;
    mmem_latency = 0;
    
    comm_energy  = 0.0;
    comp_energy  = 0.0;
    lmem_energy  = 0.0;
    mmem_energy  = 0.0;

    lmem_energy_leakage = 0.0;
    mmem_energy_leakage = 0.0;
    comm_energy_leakage = 0.0;
    comp_energy_leakage = 0.0;
    
    active_cores = 0;
    ops_per_core = 0;
  
    main_memory_traffic_load  = 0;
    main_memory_traffic_store = 0;
  }

  void addLatencyComponents(TLatencyComponents lc) {
    comm_latency += lc.l_comm;
    mmem_latency += lc.l_mmem;
    comp_latency += lc.l_comp;
  }
  
  void addEnergyComponents(TEnergyComponents ec) {
    comm_energy += ec.e_comm;
    comp_energy += ec.e_comp;
    lmem_energy += ec.e_lmem;
    mmem_energy += ec.e_mmem;
    lmem_energy_leakage += ec.e_lmem_leakage;
    mmem_energy_leakage += ec.e_mmem_leakage;
    comm_energy_leakage += ec.e_comm_leakage;
    comp_energy_leakage += ec.e_comp_leakage;
  }
  
} TLayerStat;
  
typedef struct GlobalStats
{
  long               total_comm_latency;
  long               total_comp_latency;
  long               total_mmem_latency;

  double             total_comm_energy;
  double             total_comp_energy;
  double             total_mmem_energy;
  double             total_lmem_energy;

  double             total_mmem_energy_leakage;
  double             total_lmem_energy_leakage;
  double             total_comm_energy_leakage;
  double             total_comp_energy_leakage;
  
  long               total_main_memory_traffic_load;
  long               total_main_memory_traffic_store;
  
  vector<TLayerStat> layer_stats;

  void reset() {
    total_comm_latency = 0;
    total_comp_latency = 0;
    total_mmem_latency = 0;
      
    total_comm_energy  = 0.0;
    total_comp_energy  = 0.0;
    total_mmem_energy  = 0.0;
    total_lmem_energy  = 0.0;

    total_mmem_energy_leakage = 0.0;
    total_lmem_energy_leakage = 0.0;
    total_comm_energy_leakage = 0.0;
    total_comp_energy_leakage = 0.0;
    
    total_main_memory_traffic_load  = 0;
    total_main_memory_traffic_store = 0;
    
    layer_stats.clear();
  }
  
  void addLayerStat(TLayerStat& ls) {
    layer_stats.push_back(ls);
    total_comm_latency += ls.comm_latency;
    total_comp_latency += ls.comp_latency;
    total_mmem_latency += ls.mmem_latency;

    total_comm_energy  += ls.comm_energy;
    total_comp_energy  += ls.comp_energy;
    total_mmem_energy  += ls.mmem_energy;
    total_lmem_energy  += ls.lmem_energy;
    
    total_mmem_energy_leakage += ls.mmem_energy_leakage;
    total_lmem_energy_leakage += ls.lmem_energy_leakage;
    total_comm_energy_leakage += ls.comm_energy_leakage;
    total_comp_energy_leakage += ls.comp_energy_leakage;
    
    total_main_memory_traffic_load += ls.main_memory_traffic_load;
    total_main_memory_traffic_store += ls.main_memory_traffic_store;	
  }
  
} TGlobalStats;

// ----------------------------------------------------------------------

typedef struct HistorySample
{
  int nactive_cores;
  int per_core_fm_size;

  void setSample(int nac, long pcfms) {
    nactive_cores = nac;
    per_core_fm_size = pcfms;
  }
} THistorySample;

// ----------------------------------------------------------------------

class Estimation
{

public:

  Estimation(NoC& _noc, CNN& _cnn) : noc(_noc), cnn(_cnn) { }

  bool stime(TGlobalStats& stats);

  void showStats(TGlobalStats& stats);

  NoC noc;
  CNN cnn;

  
private:

  bool stime(int layer_no, TLayer& layer, TLayerStat& layer_stat);

  void stimeLeakage(TLayerStat& layer_stat);

  void stimeConv(int layer_no, TLayer& layer, TLayerStat& layer_stat, bool depthwiseconv);
  void stimeFC(int layer_no, TLayer& layer, TLayerStat& layer_stat);
  void stimePool(int layer_no, TLayer& layer, TLayerStat& layer_stat);

  int requiredCores(int ntasks);

  void stimeLoadFeatureMap(int layer_no, TLayer& layer,
			   TLayerStat& layer_stat,
			   int nactive_cores,
			   bool depthwiseconv);

  void stimeStoreFeatureMap(TLayer& layer, TLayerStat& layer_stat,
			    int nactive_cores);

  void hline(int n, char c);

  vector<THistorySample> history;
};

#endif

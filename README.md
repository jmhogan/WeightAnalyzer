# WeightAnalyzer
Calculate negative weights, pileup weights, tag efficiencies, etc. This writes to LPC nobackup area, so use sparingly! Intended for once-in-a-while calculation that write small files with a few histograms.

Copy plugins/WeightAnalyzer<X>.orig onto plugins/WeightAnalyzer.cc and compile

Set config parameters in python/runWeightAnalyzer_template_cfg.py (test if desired)

Run from python/condor_submitargs.py very similar to LJMet.

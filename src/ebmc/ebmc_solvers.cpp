/*******************************************************************\

Module: EBMC's Solvers

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include <fstream>
#include <iostream>

#include <util/i2string.h>
#include <util/cmdline.h>

#include <trans-netlist/compute_ct.h>

#include <solvers/cvc/cvc_dec.h>
#include <solvers/dplib/dplib_dec.h>
#include <solvers/smt1/smt1_dec.h>
#include <solvers/smt2/smt2_dec.h>
#include <solvers/flattening/boolbv.h>
#include <solvers/sat/dimacs_cnf.h>
#include <solvers/sat/satcheck.h>

#ifdef HAVE_PROVER
#include <prover/prover_sat.h>
#include <prover/lifter.h>
#endif

#include "ebmc_base.h"
#include "version.h"

/*******************************************************************\

Function: ebmc_baset::do_dimacs

  Inputs:

 Outputs:

 Purpose: invoke main modules

\*******************************************************************/

int ebmc_baset::do_dimacs()
{
  dimacs_cnft dimacs_cnf;
  int result=do_ebmc(dimacs_cnf, true);
  if(result!=0) return result;

  statistics() << dimacs_cnf.no_variables() << " variables and "
               << dimacs_cnf.no_clauses() << " clauses" << eom;

  if(cmdline.isset("outfile"))
  {
    const std::string filename=cmdline.get_value("outfile");
    std::ofstream out(filename.c_str());
  
    if(!out)
    {
      error() << "Failed to open `"
              << filename
              << "'" << messaget::eom;
      return 1;
    }

    dimacs_cnf.write_dimacs_cnf(out);
  }
  else
    dimacs_cnf.write_dimacs_cnf(std::cout);

  return 0;
}

/*******************************************************************\

Function: ebmc_baset::do_smt1

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

int ebmc_baset::do_smt1()
{
  const namespacet ns(symbol_table);

  if(cmdline.isset("outfile"))
  {
    const std::string filename=cmdline.get_value("outfile");
    std::ofstream out(filename.c_str());
  
    if(!out)
    {
      std::cerr << "Failed to open `"
                << filename
                << "'" << '\n';
      return 1;
    }

    smt1_convt smt1_conv(
      ns,
      "ebmc",
      "Generated by EBMC " EBMC_VERSION,
      "QF_AUFBV",
      smt1_convt::Z3,
      out);
     
    return do_ebmc(smt1_conv, true);
  }

  smt1_convt smt1_conv(
    ns,
    "ebmc",
    "Generated by EBMC " EBMC_VERSION,
    "QF_AUFBV",
    smt1_convt::Z3,
    std::cout);
   
  return do_ebmc(smt1_conv, true);
}

/*******************************************************************\

Function: ebmc_baset::do_smt2

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

int ebmc_baset::do_smt2()
{
  const namespacet ns(symbol_table);

  if(cmdline.isset("outfile"))
  {
    const std::string filename=cmdline.get_value("outfile");
    std::ofstream out(filename.c_str());
  
    if(!out)
    {
      std::cerr << "Failed to open `"
                << filename
                << "'" << '\n';
      return 1;
    }

    smt2_convt smt2_conv(
      ns,
      "ebmc",
      "Generated by EBMC " EBMC_VERSION,
      "QF_AUFBV",
      smt2_convt::Z3,
      out);
     
    return do_ebmc(smt2_conv, true);
  }

  smt2_convt smt2_conv(
    ns,
    "ebmc",
    "Generated by EBMC " EBMC_VERSION,
    "QF_AUFBV",
    smt2_convt::Z3,
    std::cout);
   
  return do_ebmc(smt2_conv, true);
}

/*******************************************************************\

Function: ebmc_baset::do_mathsat

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

int ebmc_baset::do_mathsat()
{
  const namespacet ns(symbol_table);

  smt2_dect smt2_dec(
    ns,
    "ebmc",
    "Generated by EBMC " EBMC_VERSION,
    "QF_AUFBV",
    smt2_dect::MATHSAT);
                     
  return do_ebmc(smt2_dec, false);
}

/*******************************************************************\

Function: ebmc_baset::do_z3

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

int ebmc_baset::do_z3()
{
  const namespacet ns(symbol_table);

  smt2_dect smt2_dec(
    ns,
    "ebmc",
    "Generated by EBMC " EBMC_VERSION,
    "QF_AUFBV",
    smt2_dect::Z3);
                     
  return do_ebmc(smt2_dec, false);
}

/*******************************************************************\

Function: ebmc_baset::do_cvc4

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

int ebmc_baset::do_cvc4()
{
  const namespacet ns(symbol_table);

  smt2_dect smt2_dec(
    ns,
    "ebmc",
    "Generated by EBMC " EBMC_VERSION,
    "QF_AUFBV",
    smt2_dect::CVC4);
                     
  return do_ebmc(smt2_dec, false);
}

/*******************************************************************\

Function: ebmc_baset::do_yices

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

int ebmc_baset::do_yices()
{
  const namespacet ns(symbol_table);
  
  smt2_dect smt2_dec(
    ns,
    "ebmc",
    "Generated by EBMC " EBMC_VERSION,
    "QF_AUFBV",
    smt2_dect::YICES);
                     
  return do_ebmc(smt2_dec, false);
}

/*******************************************************************\

Function: ebmc_baset::do_boolector

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

int ebmc_baset::do_boolector()
{
  const namespacet ns(symbol_table);

  smt1_dect smt1_dec(
    ns,
    "ebmc",
    "Generated by EBMC " EBMC_VERSION,
    "QF_AUFBV",
    smt1_dect::BOOLECTOR);
                     
  return do_ebmc(smt1_dec, false);
}

/*******************************************************************\

Function: ebmc_baset::do_sat

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

int ebmc_baset::do_sat()
{
  satcheckt satcheck;
  satcheck.set_message_handler(get_message_handler());

  if(cmdline.isset("aig"))
  {
    return do_ebmc(satcheck, false);
  }
  else
  {
    const namespacet ns(symbol_table);
    boolbvt boolbv(ns, satcheck);
    return do_ebmc(boolbv, false);
  }
}

/*******************************************************************\

Function: ebmc_baset::do_prover

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

int ebmc_baset::do_prover()
{
  #ifdef HAVE_PROVER
  const namespacet ns(symbol_table);
  prover_satt prover_sat(ns);
  return do_ebmc(prover_sat, false);
  #else
  error() << "Support for prover not linked in" << eom;
  return 1;
  #endif
}

/*******************************************************************\

Function: ebmc_baset::do_lifter

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

int ebmc_baset::do_lifter()
{
  #ifdef HAVE_PROVER
  const namespacet ns(symbol_table);
  liftert lifter(ns);
  return do_ebmc(lifter.prop_conv(), false);
  #else
  error() << "Support for lifter not linked in" << eom;
  return 1;
  #endif
}

/*******************************************************************\

Function: ebmc_baset::do_compute_ct

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

int ebmc_baset::do_compute_ct()
{
  // make net-list
  status() << "Making Netlist" << eom;

  netlistt netlist;
  if(make_netlist(netlist)) return 1;

  status() << "Latches: " << netlist.var_map.latches.size()
           << ", nodes: " << netlist.number_of_nodes() << eom;

  status() << "Making LDG" << eom;
  
  ldgt ldg;
  ldg.compute(netlist);

  std::cout << "CT = " << compute_ct(ldg) << '\n';
  
  return 0;
}


/*******************************************************************\

Module: Verilog Flattening

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#ifndef CPROVER_VERILOG_FLATTENING_H
#define CPROVER_VERILOG_FLATTENING_H

#include <util/options.h>

#include "verilog_typecheck_base.h"
#include "verilog_symbol_table.h"

bool verilog_flattening(
  symbol_tablet &symbol_table,
  const irep_idt &module,
  message_handlert &message_handler,
  const optionst &options);

/*******************************************************************\

   Class: verilog_flatteningt

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

class verilog_flatteningt:
  public verilog_typecheck_baset,
  public verilog_symbol_tablet
{
public:
  verilog_flatteningt(
    symbol_tablet &_symbol_table,
    const irep_idt &_module,
    const optionst &_options,
    message_handlert &_message_handler):
    verilog_typecheck_baset(ns, _message_handler),
    verilog_symbol_tablet(_symbol_table),
    ns(_symbol_table),
    options(_options),
    module(_module)
  {
  }

  virtual void typecheck();

protected:
  const namespacet ns;
  const optionst &options;
  const irep_idt &module;

  typedef std::map<irep_idt, exprt> replace_mapt;
  exprt::operandst dest;
  
  // module items
  void flatten_module(symbolt &symbol);

  void flatten_module_item(class verilog_module_itemt &module_item);
  void flatten_expr(exprt &expr);
  void flatten_inst(class verilog_instt &inst);
  void flatten_inst(const symbolt &symbol, const exprt &op);
  void flatten_function_call(class function_call_exprt &call);

  bool replace_symbols(const replace_mapt &what, exprt &dest);
  void replace_symbols(const irep_idt &target, exprt &dest);
  
  void instantiate_ports(
    const irep_idt &instance,
    const exprt &inst,
    const symbolt &symbol,
    const replace_mapt &replace_map);

  void instantiate_port(
    const exprt &symbol_expr,
    const exprt &value,
    const replace_mapt &replace_map);
};

#endif

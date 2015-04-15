//
// bus/controller.c: System bus controller.
//
// CEN64: Cycle-Accurate Nintendo 64 Simulator.
// Copyright (C) 2014, Tyler J. Stachecki.
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//

#include "common.h"
#include "ai/controller.h"
#include "bus/address.h"
#include "bus/controller.h"
#include "bus/memorymap.h"
#include "dd/controller.h"
#include "pi/controller.h"
#include "ri/controller.h"
#include "si/controller.h"
#include "rdp/cpu.h"
#include "rdp/interface.h"
#include "rsp/cpu.h"
#include "rsp/interface.h"
#include "vi/controller.h"
#include "vr4300/cpu.h"
#include "vr4300/interface.h"

#define NUM_MAPPINGS 19

struct bus_controller_mapping {
  memory_rd_function read;
  memory_wr_function write;
  uint32_t address;
  uint32_t length;
};

// Initializes the bus component.
int bus_init(struct bus_controller *bus) {
  unsigned i;

  static const struct bus_controller_mapping mappings[NUM_MAPPINGS] = {
    {read_ai_regs, write_ai_regs, AI_REGS_BASE_ADDRESS, AI_REGS_ADDRESS_LEN},
    {read_dd_regs, write_dd_regs, DD_REGS_BASE_ADDRESS, DD_REGS_ADDRESS_LEN},
    {read_dp_regs, write_dp_regs, DP_REGS_BASE_ADDRESS, DP_REGS_ADDRESS_LEN},
    {read_mi_regs, write_mi_regs, MI_REGS_BASE_ADDRESS, MI_REGS_ADDRESS_LEN},
    {read_pi_regs, write_pi_regs, PI_REGS_BASE_ADDRESS, PI_REGS_ADDRESS_LEN},
    {read_ri_regs, write_ri_regs, RI_REGS_BASE_ADDRESS, RI_REGS_ADDRESS_LEN},
    {read_si_regs, write_si_regs, SI_REGS_BASE_ADDRESS, SI_REGS_ADDRESS_LEN},
    {read_sp_regs, write_sp_regs, SP_REGS_BASE_ADDRESS, SP_REGS_ADDRESS_LEN},
    {read_vi_regs, write_vi_regs, VI_REGS_BASE_ADDRESS, VI_REGS_ADDRESS_LEN},

    {read_cart_rom, write_cart_rom, ROM_CART_BASE_ADDRESS, ROM_CART_ADDRESS_LEN},
    {read_dd_c2s_buffer, write_dd_c2s_buffer, DD_C2S_BUFFER_ADDRESS, DD_C2S_BUFFER_LEN},
    {read_dd_ds_buffer, write_dd_ds_buffer, DD_DS_BUFFER_ADDRESS, DD_DS_BUFFER_LEN},
    {read_dd_ms_ram, write_dd_ms_ram, DD_MS_RAM_ADDRESS, DD_MS_RAM_LEN},
    {read_dd_ipl_rom, write_dd_ipl_rom, DD_IPL_ROM_ADDRESS, DD_IPL_ROM_LEN},
    {read_pif_ram, write_pif_ram, PIF_RAM_BASE_ADDRESS, PIF_RAM_ADDRESS_LEN},
    {read_pif_rom, write_pif_rom, PIF_ROM_BASE_ADDRESS, PIF_ROM_ADDRESS_LEN},
    {read_rdram_regs, write_rdram_regs, RDRAM_REGS_BASE_ADDRESS, RDRAM_REGS_ADDRESS_LEN},
    {read_sp_regs2, write_sp_regs2, SP_REGS2_BASE_ADDRESS, SP_REGS2_ADDRESS_LEN},
    {read_sp_mem, write_sp_mem, SP_MEM_BASE_ADDRESS, SP_MEM_ADDRESS_LEN},
  };

  void *instances[NUM_MAPPINGS] = {
    bus->ai,
    bus->dd,
    bus->rdp,
    bus->vr4300,
    bus->pi,
    bus->ri,
    bus->si,
    bus->rsp,
    bus->vi,

    bus->pi,
    bus->dd,
    bus->dd,
    bus->dd,
    bus->dd,
    bus->si,
    bus->si,
    bus->ri,
    bus->rsp,
    bus->rsp
  };

  create_memory_map(&bus->map);

  for (i = 0; i < NUM_MAPPINGS; i++)
    if (map_address_range(&bus->map, mappings[i].address, mappings[i].length,
      instances[i], mappings[i].read, mappings[i].write))
      return 1;

  return 0;
}

// Issues a read request to the bus.
int bus_read_word(void *component, uint32_t address, uint32_t *word) {
  const struct memory_mapping *node;
  struct bus_controller *bus;

  memcpy(&bus, component, sizeof(bus));

  if (address < RDRAM_BASE_ADDRESS_LEN)
    return read_rdram(bus->ri, address, word);

  else if ((node = resolve_mapped_address(&bus->map, address)) == NULL) {
    debug("bus_read_word: Failed to access: 0x%.8X\n", address);

    *word = 0x00000000U;
    return 0;
  }

  return node->on_read(node->instance, address, word);
}

// Issues a write request to the bus.
int bus_write_word(void *component,
  uint32_t address, uint32_t word, uint32_t dqm) {
  const struct memory_mapping *node;
  struct bus_controller *bus;

  memcpy(&bus, component, sizeof(bus));

  if (address < RDRAM_BASE_ADDRESS_LEN)
    return write_rdram(bus->ri, address, word & dqm, dqm);

  else if ((node = resolve_mapped_address(&bus->map, address)) == NULL) {
    debug("bus_write_word: Failed to access: 0x%.8X\n", address);

    return 0;
  }

  return node->on_write(node->instance, address, word & dqm, dqm);
}

